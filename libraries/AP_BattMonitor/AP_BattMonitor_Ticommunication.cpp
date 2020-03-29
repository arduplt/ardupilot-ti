#include <AP_HAL/AP_HAL.h>
#include "AP_BattMonitor_Ticommunication.h"
#include <GCS_MAVLink/GCS.h>
#include <AP_RTC/AP_RTC.h>
//#include <AP_Ticommunication/AP_Ticommunication.h>           // Sonin Aero
//#include "AP_Ticommunication.h"


/*
Sonin Aero, get the Battery info from the external Hybrid Microcontroller
 */
extern const AP_HAL::HAL& hal;

//#define FUELFLOW_MIN_PULSE_DELTA_US 10


/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// *********************************** TICOM Library





/* AP_Ticommunication::AP_Ticommunication(void)
{
    //AP_Param::setup_object_defaults(this, var_info);

    if (_singleton != nullptr) {
        AP_HAL::panic("AP_Ticommunication must be singleton");
    }
    _singleton = this;
}*/

AP_BattMonitor_Ticommunication *AP_BattMonitor_Ticommunication::singleton;



// Initialize parameters

//AP_BattMonitor_Ticommunication Ticom;   // Creating 1 Object Ticom

AP_BattMonitor_Ticommunication::AP_BattMonitor_Ticommunication(AP_BattMonitor &mon,
                                                 AP_BattMonitor::BattMonitor_State &mon_state,
                                                 AP_BattMonitor_Params &params) :
    AP_BattMonitor_Backend(mon, mon_state, params)
{


    _state.voltage = 5.0; // show a fixed voltage of 5v



    // We can tell if this is healthy with the is healthy function
    _state.healthy = is_healthy() ;

    singleton = this;

}

// Initialize backends based on existing params
void AP_BattMonitor_Ticommunication::init(void)
{
	// Search for the Port assigned to Ticommunication
	//port = hal.uartD;
	port = AP::serialmanager().find_serial(AP_SerialManager::SerialProtocol_Ticommunication, 0);

	if (port != NULL) 		gcs().send_text(MAV_SEVERITY_WARNING, "We have found the port");
	else 		gcs().send_text(MAV_SEVERITY_WARNING, "We have not found the TI port");


        return;
}



void AP_BattMonitor_Ticommunication::update(void)
{
	//port = hal.uartD;
	//gcs().send_text(MAV_SEVERITY_WARNING, "We are in BattmonTicom update");
	//   if (port == hal.uartD)
	// {
	// 	gcs().send_text(MAV_SEVERITY_WARNING, "port is port uartD");
	// }


    int16_t nbytes = port->available();

	bool message_start_found = false;
    int buffer_count;

    // if (nbytes >0) {
    // port->print("<1>");
    //port->print("AAA");
    //}
    if (nbytes<=0)
    {
    	port->print("0");
    	gcs().send_text(MAV_SEVERITY_WARNING, "We are not receiving data from HC, nbytes <0");

    }
    //gcs().send_text(MAV_SEVERITY_WARNING, "We have assigned uartD");
	//port->write('<');

	//port->printf((char)nbytes);
	//port->write('1');
	//port->write('>');
	//}

	if (nbytes >0) {

		//gcs().send_text(MAV_SEVERITY_WARNING, "We are receiving bytes nbytes>0");

	    //mavlink_system_time_t *utime;
	    //uint64_t now = 0;
	    //AP::rtc().get_utc_usec(now);


	   // int now = time_unix / 1000000;
	    // rtc.set_utc_usec(kinda_now, AP_RTC::SOURCE_MAVLINK_SYSTEM_TIME);
	       // hal.console->printf("%s: Test run %u\n", __FILE__, (unsigned)run_counter++);
	   /*     uint32_t now = 0;
	        if (!rtc.get_utc_usec(0)) {
	        	hal.console->printf("failed to get rtc");
	            return;
	        }
	        hal.console->printf("Now=%llu\n", (long long unsigned)now);
	        if (now < kinda_now) {
	        	hal.console->printf("time going backwards");
	            return;
	        }*/

	    //int time_b;
	    //time_b = time_s/1000;
	    //std::string s;
	    //s = std:: to_string(time_s);
	    //char time_b[10];
	    //time_b=	(char [10])time_s;
	    //const char *s = &time_s;
	    //int b = now/1000000;
		//AP_RTC &rtc = AP::rtc();


    while (nbytes-- > 0) {                   // While Loop continues until all bytes in the Buffer are read
        char c = port->read();
         //const char *b = &c;
         //port-> print(b);

        if (c == '<' ) {                     // Start of one message
        //	gcs().send_text(MAV_SEVERITY_WARNING, "we have found the start of the message");
            buffer_count = 0;
			message_start_found = true;
        }

		if (message_start_found == false) continue;  // we did not find the start of the message yet, continue searching for it

		// This code only executes if the message Start "<" was found
		if (c == ' ') continue;
        buffer[buffer_count] = c;
		buffer_count++;

        if (buffer_count ==3 && buffer[1]=='2' && buffer[2]=='>'){
        	  gcs().send_text(MAV_SEVERITY_INFO, "<2> request in buffer, sending timestamp");
              send_timestamp();
			  message_start_found = false;
			  buffer_count=0;
			  continue;
        }
		if (buffer_count==16) {
			//gcs().send_text(MAV_SEVERITY_INFO, "Buffer = 15 check message");
			if (check_message ()) {
			    // A valid message is in the buffer
				//gcs().send_text(MAV_SEVERITY_INFO, "A valid message is in the buffer");
				port->print("<1>");  // send <1> to HC if message is valid
				last_updated_ms = AP_HAL::millis();
				break;             // break the while loop
			}
			else {                                   // If the message is not valid, look for a new valid message
				gcs().send_text(MAV_SEVERITY_INFO, "Message NOT Valid");
				message_start_found = false;
			    continue;	}
		}
	}



	}   else  {
	 	gcs().send_text(MAV_SEVERITY_INFO, "we are not receiving bytes from HC");
	 	return;
	 }// Endif nbytes >0

	// if (is_healthy()) {
	// gcs().send_text(MAV_SEVERITY_WARNING, "connection healthy");
	//	port-> print ("<1>");// Send Heartbeat <1> if connection is Healthy
	// }

   return;
}


void AP_BattMonitor_Ticommunication::send_timestamp(){
			uint64_t now;
			//mavlink_system_time_t *utime;
			//now = &utime.time_unix_usec;
		    AP::rtc().get_utc_usec(now); // may fail, leaving time_unix at 0

	        int inow= (int)(now /1000000); // from us to s

	        //port -> read

		    gcs().send_text(MAV_SEVERITY_INFO, "RTC EPOCH Time: %d s.", inow);
		    char t[10];
			//char t[10]={'1','3','4','6','7','8','1','7','c','9'};
		    itoa(inow , t , 10);
			port-> print ("<2,");
			port-> print(t);
			/*for (int j=0; j< 10 ; j++)
			{
			port->print(&t[j]);
			}*/
			port->print(">");
}

bool AP_BattMonitor_Ticommunication::check_message()
{

    //if (buffer[15]!='>') return false;
	if (buffer[0] != '<' || buffer[2] != ',' || buffer[5] != ',' || buffer[9] != ',' || buffer[15]!= '>') return false;  // Check message structure < x,xx,xxx,xxxxx>
	if ( buffer[1] != '1') return false;
	int number_index[]={1,3,4,6,7,8,10,11,12,13,14};

	for (int i=0; i <= 9 ; i++)
	{
		if ( buffer[number_index[i]] > '9' || buffer[number_index[i]] < '0' ) return false;
		else continue; // check if x xx xxx are numbers from 0-9
	}


	return true;    // if everything is ok message is valid

}



bool AP_BattMonitor_Ticommunication::is_healthy()       // Sonin Aero: Check Signal, if Healthy return TRUE and send <1> to Ticommunication
{
	if (port  && (AP_HAL::millis() - last_updated_ms) < 3000 ) {     // check if last Update is less than 3000 ms ago
		return true;
	}
    else return false;
}



int  AP_BattMonitor_Ticommunication::get_soc() {
	//gcs().send_text(MAV_SEVERITY_WARNING, "Calculating SOC");
	int soc;
	char soc_buffer [2];
	soc_buffer[0]= buffer [3];
	soc_buffer[1]= buffer [4];

	soc = atoi(soc_buffer);

	return soc;
}

int AP_BattMonitor_Ticommunication::get_remaining_flight_time () {

	int remaining_flight_time;
	char rft_buffer [3];
	rft_buffer[0]= buffer [6];
	rft_buffer[1]= buffer [7];
	rft_buffer[2]= buffer [8];

	remaining_flight_time = atoi(rft_buffer);

	return remaining_flight_time;	// in minutes
}

int AP_BattMonitor_Ticommunication::get_RPM () {

	int RPM;
	char rpm_buffer [5];
	rpm_buffer[0]= buffer [10];
	rpm_buffer[1]= buffer [11];
	rpm_buffer[2]= buffer [12];
	rpm_buffer[3]= buffer [13];
	rpm_buffer[4]= buffer [14];

	RPM = atoi(rpm_buffer);
	//gcs().send_text(MAV_SEVERITY_WARNING, "in get RPM function");

	return RPM;	// in rounds per minute
}

/*
namespace AP {
AP_Ticommunication *Ticommunication()
{
    return AP_Ticommunication::get_singleton();
}
}
*/



//#endif // Ticommunication_ENABLED




/// Constructor


/*
  handle interrupt on an instance
 */


/*
  read - read the data from Ticommunication
*/
void AP_BattMonitor_Ticommunication::read()

{
	update();
	// Most of the read() variables will be read from the Ticommunication object Ticom
	//Ticommunication.port = hal.uartD;
	//if (Ticommunication.is_healthy() == false){
	// gcs().send_text(MAV_SEVERITY_WARNING, "in Battmonitor connection not healthy");
	//}

    uint32_t now_us = AP_HAL::micros();
    if (_state.last_time_micros == 0) {
        // need initial time, so we can work out expected pulse rate
        _state.last_time_micros = now_us;
        return;
    }

    _state.last_time_micros = now_us;

    /*if (!Ticommunication.is_healthy())
    {
        gcs().send_text(MAV_SEVERITY_CRITICAL, "TI connection not healthy!");
        _state.current_amps = 0;
        _state.consumed_mah = 0;
        _state.consumed_wh =  0;
        _state.state_of_charge= 0;
        _state.state_of_charge = 0;
    }
    else {*/
    // map amps to liters /hour
    _state.current_amps = 1 ; // We can get this variable if needed from the Ticom using Ticom.current_amps (not yet implemented)

    // map consumed_mah to consumed milliliters
    _state.consumed_mah = 10;   // has to be sent from Ticom.consumedlitres (not yet implemented)

    // map consumed_wh using fixed voltage of 1
    _state.consumed_wh = 40;   // has to be sent from Ticom.consumedWH

     // We get this info from Ticom get_soc() function
    if (get_soc() == 0){ _state.state_of_charge= oldSOC;}


    if (get_soc() > 0) {
    	//gcs().send_text(MAV_SEVERITY_WARNING, "SOC is bigger than 0");
    	_state.state_of_charge = get_soc();
    	oldSOC=_state.state_of_charge;
    }


//    _state.state_of_charge = Ticommunication.get_soc();  // We get this info from Ticom get_soc() function

    _state.time_remaining = get_remaining_flight_time(); // Get remaining flight time in minutes
     gcs().send_text(MAV_SEVERITY_CRITICAL, "Flight time remaining: %d min.", (int)_state.time_remaining);

   // } // end of else statement
}

/*static AP_BattMonitor_Ticommunication *get_singleton(void) {
    return singleton;
}*/


namespace AP {
AP_BattMonitor_Ticommunication *Ticom()
{
    return AP_BattMonitor_Ticommunication::get_singleton();
}
}



