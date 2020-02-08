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

#include "AP_Ticommunication.h"

#include <AP_Logger/AP_Logger.h>

#include <AP_HAL/AP_HAL.h>

#include <AP_SerialManager/AP_SerialManager.h>




extern const AP_HAL::HAL& hal;

/*
AP_Ticommunication::AP_Ticommunication()
{
    //AP_Param::setup_object_defaults(this, var_info);

    if (_singleton != nullptr) {
        AP_HAL::panic("AP_Ticommunication must be singleton");
    }
    _singleton = this;
}
*/

// Initialize backends based on existing params
void AP_Ticommunication::init(void)
{
	// Search for the Port assigned to Ticommunication
	//port = hal.uartD;

	if (port != NULL) 		gcs().send_text(MAV_SEVERITY_WARNING, "We have found the port");
	else 		gcs().send_text(MAV_SEVERITY_WARNING, "We have not found the TI port");


        return;
}



void AP_Ticommunication::update(void)
{




	port = AP::serialmanager().find_serial(AP_SerialManager::SerialProtocol_Ticommunication, 0);
	//port = hal.uartD;

	/*  if (port == hal.uartD)
	 {
	 	gcs().send_text(MAV_SEVERITY_WARNING, "port is port uartD");
	 }*/


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
    }
    //gcs().send_text(MAV_SEVERITY_WARNING, "We have assigned uartD");
	//port->write('<');

	//port->printf((char)nbytes);
	//port->write('1');
	//port->write('>');
	//}
    
	if (nbytes >0) {

		//gcs().send_text(MAV_SEVERITY_WARNING, "We are receiving bytes nbytes>0");
		port->print("<1>");

    while (nbytes-- > 0) {                   // While Loop continues until all bytes in the Buffer are read
        char c = port->read();
         //const char *b = &c;
         //port-> print(b);

        if (c == '<' ) {                     // Start of one message
        	//gcs().send_text(MAV_SEVERITY_WARNING, "we have found the start of the message");
            buffer_count = 0;
			message_start_found = true;
        }
        
		if (message_start_found == false) continue;  // we did not find the start of the message yet, continue searching for it
		
		// This code only executes if the message Start "<" was found
		if (c == ' ') continue;
        buffer[buffer_count] = c;
		buffer_count++;
		
		if (buffer_count==10) {
			if (check_message ()) {
			    // A valid message is in the buffer
				//gcs().send_text(MAV_SEVERITY_WARNING, "A valid message is in the buffer");
				last_updated_ms = AP_HAL::millis();
				break;             // break the while loop
			}
			else {                                   // If the message is not valid, look for a new valid message
				message_start_found = false;
			    continue;	}
		}
	}


	
	}   else  {
	 	gcs().send_text(MAV_SEVERITY_WARNING, "we are not receiving bytes");
	 }// Endif nbytes >0

	// if (is_healthy()) {
	// gcs().send_text(MAV_SEVERITY_WARNING, "connection healthy");
	//	port-> print ("<1>");// Send Heartbeat <1> if connection is Healthy
	// }
	

}
	
bool AP_Ticommunication::check_message()
{
	
	
	if (buffer[0] != '<' || buffer[2] != ',' || buffer[5] != ',' || buffer[9] != '>') return false;  // Check message structure < x,xx,xxx>
	
	int number_index[]={1,3,4,6,7,8};
	
	for (int i=0; i <= 5 ; i++)
	{
		if ( buffer[number_index[i]] > '9' || buffer[number_index[i]] < '0' ) return false;
		else continue; // check if x xx xxx are numbers from 0-9
	}
	
	
	return true;    // if everything is ok message is valid

}
		


bool AP_Ticommunication::is_healthy()       // Sonin Aero: Check Signal, if Healthy return TRUE and send <1> to Ticommunication
{
	if (port  && (AP_HAL::millis() - last_updated_ms) < 3000 ) {     // check if last Update is less than 3000 ms ago
		return true;
	}
    else return false;    
}



int  AP_Ticommunication::get_soc() {
	//gcs().send_text(MAV_SEVERITY_WARNING, "Calculating SOC");
	int soc;
	char soc_buffer [2];
	soc_buffer[0]= buffer [3];
	soc_buffer[1]= buffer [4];
	
	soc = atoi(soc_buffer);
	
	return soc;	
}

int AP_Ticommunication::get_remaining_flight_time () {
	
	int remaining_flight_time;
	char rft_buffer [3];
	rft_buffer[0]= buffer [6];
	rft_buffer[1]= buffer [7];
	rft_buffer[2]= buffer [8];
	
	remaining_flight_time = atoi(rft_buffer);
	
	return remaining_flight_time;	// in minutes
}

/*
namespace AP {
AP_Ticommunication *Ticommunication()
{
    return AP_Ticommunication::get_singleton();
}
}
*/
extern const AP_HAL::HAL &hal;


//#endif // Ticommunication_ENABLED





