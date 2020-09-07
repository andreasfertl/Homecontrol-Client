#include <iostream>


#include "project.h"
#include "thread.h"

#include "BluetoothManager.h"


//cycle time 30s, 1s hysterese
BluetoothManager::BluetoothManager() : thread((std::string)__FILE__, 30000, 1000, THREADSID::ThreadID::BLUETOOTHMANAGER)
{
	//open socket
	m_Socket = hci_open_dev(hci_get_route(nullptr));
}     


BluetoothManager::~BluetoothManager()
{

	Logger_Write(LoggLevel::WARNING, (std::string)"Destructor from " +  __FILE__);

	//first wait for stopping our thread and then kill all things
	RequestStop();

	//delete all btdevices
	for (auto pBTDevice : m_vpBTDevice)
	{
		delete pBTDevice; //delete my pointer to the BTDevice
		pBTDevice = nullptr;
	}
	m_vpBTDevice.clear(); //and now we can clear the whole list
}


Handle BluetoothManager::RegisterBTDevice(int DeviceId, std::string BTDeviceAddress)
{
	//generate BT device
	BluetoothDevice *pBTDevice = new BluetoothDevice(m_Socket, DeviceId, BTDeviceAddress);
	Logger_Write(LoggLevel::LOGFILE, "Added BTDevice with addr " + BTDeviceAddress);

    m_vpBTDevice.push_back(pBTDevice); //add to my internal list
	return pBTDevice->GetHandle();
}


//searches in our vector of BTdevices after the corresponding device
BluetoothDevice *BluetoothManager::FindBtDevice(BluetoothDevice &BtDeviceToFind)
{
    //cycle through all my devices     
    for (auto pBTDevice : m_vpBTDevice)
    {
        //is this the one i am looking for?
        if (pBTDevice && *pBTDevice == BtDeviceToFind)
            return pBTDevice;
    }
    
    //not found :(
    return nullptr;
}

int BluetoothManager::ParseMessage(Message &Msg)
{
    return RETVAL::MESSAGE_NOT_HANDELD;
}

int BluetoothManager::SendRangeChange(BluetoothDevice& BTDevice, bool inRange)
{
    BLUETOOTHDEVICE::BluetoothDeviceMessage btDevMsg(BTDevice.GetDeviceId(), BTDevice.GetAddress(), inRange);
	Message Msg {MESSAGE::CMD::SET, THREADSID::ThreadID::ALL_SUBSCRIBERS, GetThreadId(), MESSAGE::ID::BLUETOOTHDEVICE_IN_RANGE, btDevMsg};
    
	return m_TxQueue.Push(Msg);
}

//maintain our list of BTDevices - debounce all the btdevices
//this function will be called forever
void BluetoothManager::Run()
{
	//cycle through all my devices     
	for (auto pBTDevice : m_vpBTDevice)
	{
		//and check if they are in range - or not
		if (pBTDevice)
		{
			//check in range
			bool StateChange = false;
			bool InRange = pBTDevice->DebounceIsInRange(&StateChange);

			//just send the state, if there was a change
			if (StateChange)
			{
				//build msg
				SendRangeChange(*pBTDevice, InRange);
	
				//just debug output
				std::ostringstream strMsg2;
				strMsg2 << "BtDeviceId " << pBTDevice->GetDeviceId() << " State Change: " << StateChange << " In Range: " << (InRange ? "YES!!!!" : "NO");
				Logger_Write(LoggLevel::DBG2, strMsg2.str());
			}		
		}
	}
	
	Logger_Write(LoggLevel::DBG2, (std::string)"-------------------------------------------");
}





	  