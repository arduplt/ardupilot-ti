#pragma once

#include "AP_BattMonitor.h"
#include "AP_BattMonitor_Backend.h"
//#include <AP_Ticommunication/AP_Ticommunication.h>           // Sonin Aero

#pragma once


#include <AP_Common/AP_Common.h>
#include <AP_Param/AP_Param.h>
#include <GCS_MAVLink/GCS.h>
#include <AP_HAL/AP_HAL.h>
#include <AP_Math/AP_Math.h>

#include <AP_Logger/AP_Logger.h>

#include <AP_SerialManager/AP_SerialManager.h>

#include <AP_RTC/AP_RTC.h>

#include <string>

//#include <../Plane.h>

class AP_Ticommunication {
public:


	//AP_Ticommunication();  // Constructor
    // Initializes backend
    void init(void);          // chkd

    // Requests backend to update the frontend. Should be called at 10Hz.
    void update();   // main loop function Sonin Aero ************************************************


	//  check if message is valid

	bool check_message();


	char buffer[16];   // used to store incoming values


    // Returns the State of charge
    int get_soc() ;

	// Returns remaining Flight time

	int get_remaining_flight_time () ;

	int get_RPM();

	// Send heartbeat from Pixhawk to Ti

	void send_heartbeat();


    bool is_healthy();


private:
    // port
    AP_HAL::UARTDriver *port;
    // Front End Parameters
    //AP_Int8 type;



	uint32_t last_updated_ms;

    // Tracking backends


};


//AP_Ticommunication Ticommunication;  // Sonin Aero creating an object of the Ticommunication Class

namespace AP {
    AP_Ticommunication *Ticommunication();
};



 //**********************************************************************DELETE******************************************************************







class AP_BattMonitor_Ticommunication : public AP_BattMonitor_Backend
{
public:
	friend class AP_Ticommunication;

    AP_Ticommunication Ticommunication;  // Sonin Aero creating an object of the Ticommunication Class


    /// Constructor
    AP_BattMonitor_Ticommunication(AP_BattMonitor &mon, AP_BattMonitor::BattMonitor_State &mon_state, AP_BattMonitor_Params &params);



    /// Read the battery voltage and current.  Should be called at 10hz
    void read() override;


    /// returns true if battery monitor provides consumed energy info
    bool has_consumed_energy() const override { return true; }

    /// returns true if battery monitor provides current info
    bool has_current() const override { return true; }

    void init(void) override {}

    uint8_t oldSOC;

private:


};

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

// Sonin Aero seasoning




/*
*This library is for communication with the Texas Instruments Microcontroller, connected via Serial
*
*
 */

