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


    _state.voltage = 1.0; // show a fixed voltage of 1v



    // We can tell if this is healthy with the is healthy function
    _state.healthy = Ticom.is_healthy() ;
}

/*
  handle interrupt on an instance
 */


/*
  read - read the data from Ticommunication
*/
void AP_BattMonitor_Ticommunication::read()

{
	// Most of the read() variables will be read from the Ticommunication object Ticom

	if (Ticom.is_healthy() == false){
	gcs().send_text(MAV_SEVERITY_WARNING, "Ticommunication could not get input data, connection not healthy");
	}

    uint32_t now_us = AP_HAL::micros();
    if (_state.last_time_micros == 0) {
        // need initial time, so we can work out expected pulse rate
        _state.last_time_micros = now_us;
        return;
    }

    _state.last_time_micros = now_us;

    // map amps to liters /hour
    _state.current_amps = 0.5 ; // We can get this variable if needed from the Ticom using Ticom.current_amps (not yet implemented)

    // map consumed_mah to consumed milliliters
    _state.consumed_mah = 40;   // has to be sent from Ticom.consumedlitres (not yet implemented)

    // map consumed_wh using fixed voltage of 1
    _state.consumed_wh = 20;   // has to be sent from Ticom.consumedWH

    _state.state_of_charge = Ticom.get_soc();  // We get this info from Ticom get_soc() function

    _state.time_remaining = (Ticom.get_remaining_flight_time())*60 ; // Get remaining flight time in seconds
}
