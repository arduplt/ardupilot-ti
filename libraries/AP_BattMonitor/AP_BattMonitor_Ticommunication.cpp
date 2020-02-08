#include <AP_HAL/AP_HAL.h>
#include "AP_BattMonitor_Ticommunication.h"
#include <GCS_MAVLink/GCS.h>



/*
Sonin Aero, get the Battery info from the external Hybrid Microcontroller
 */
extern const AP_HAL::HAL& hal;

//#define FUELFLOW_MIN_PULSE_DELTA_US 10

/// Constructor
AP_BattMonitor_Ticommunication::AP_BattMonitor_Ticommunication(AP_BattMonitor &mon,
                                                 AP_BattMonitor::BattMonitor_State &mon_state,
                                                 AP_BattMonitor_Params &params) :
    AP_BattMonitor_Backend(mon, mon_state, params)
{


    _state.voltage = 5.0; // show a fixed voltage of 5v


    // We can tell if this is healthy with the is healthy function
    _state.healthy = Ticommunication.is_healthy() ;
}

/*
  handle interrupt on an instance
 */


/*
  read - read the data from Ticommunication
*/
void AP_BattMonitor_Ticommunication::read()

{
	Ticommunication.update();
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

    // map amps to liters /hour
    _state.current_amps = 3 ; // We can get this variable if needed from the Ticom using Ticom.current_amps (not yet implemented)

    // map consumed_mah to consumed milliliters
    _state.consumed_mah = 40;   // has to be sent from Ticom.consumedlitres (not yet implemented)

    // map consumed_wh using fixed voltage of 1
    _state.consumed_wh = 20;   // has to be sent from Ticom.consumedWH

     // We get this info from Ticom get_soc() function
    if (Ticommunication.get_soc() == 0){ _state.state_of_charge= oldSOC;}


    if (Ticommunication.get_soc() > 0) {
    	//gcs().send_text(MAV_SEVERITY_WARNING, "SOC is bigger than 0");
    	_state.state_of_charge = Ticommunication.get_soc();
    	oldSOC=_state.state_of_charge;
    }


//    _state.state_of_charge = Ticommunication.get_soc();  // We get this info from Ticom get_soc() function

    _state.time_remaining = (Ticommunication.get_remaining_flight_time())*60 ; // Get remaining flight time in seconds
}
