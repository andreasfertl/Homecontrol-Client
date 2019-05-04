#include <iostream>
#include "../include/telldus-core.h"

#include "project.h"

#include "TelldusDevice.h"


#define TELLDUSDEVICE_RETRANSMIT 3

TelldusDevice::TelldusDevice(int Id, std::string sName)
{
	m_Id = Id;
	m_sName = sName;
}     


TelldusDevice::~TelldusDevice()
{

}

bool TelldusDevice::GetLastSentState(int Id)
{
	return tdLastSentCommand(Id, TELLSTICK_TURNON | TELLSTICK_TURNOFF) == TELLSTICK_TURNON ? true : false;
}

bool TelldusDevice::SetState(bool State)
{
	bool OldState = GetLastSentState(m_Id);
	
	//a change is required -> go for it
	//if (OldState != State) 
	{
        for (int i = 0; i < TELLDUSDEVICE_RETRANSMIT; i++)
        {
            if (State) //should be ON
                tdTurnOn(m_Id);
            else //should be OFF
                tdTurnOff(m_Id);
            //we changed the state of this device just let the Tellstick a little time to react
            TIMER::Sleep(100);
        }
	}

	return OldState;
}







	  