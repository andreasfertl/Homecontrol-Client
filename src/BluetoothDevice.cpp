#include <string>
#include <iostream>

#include "project.h"
#include "thread.h"

#include "BluetoothDevice.h"



//init helper function for constructurs
void BluetoothDevice::Init(int Socket, bdaddr_t &BdAddr)
{
	//remmbert the socket, will be used later for trying to read the name of the BT Device
	m_Socket = Socket;

	//remember for later usage
	m_BdAddr = BdAddr;

	//preset with in range for debugging to get an event
	m_InRange = false;
	m_Deb.SetParameters(BLUETOOTHDEVICE::IN_RANGE_MAX, BLUETOOTHDEVICE::IN_RANGE_UP_COUNT, BLUETOOTHDEVICE::IN_RANGE_DOWN_COUNT);
}


//construct with the BT Address like (28:E1:4C:CA:2F:1D) as a string
BluetoothDevice::BluetoothDevice(int Socket, int DeviceId, std::string &sBdAddr) :
    m_Handle(),
	m_DeviceId(DeviceId)
{
	bdaddr_t BdAddr;
	
	//convert the string to the device addr and init
	str2ba(sBdAddr.c_str(), &BdAddr);
	
 	Init(Socket, BdAddr);

	//save the string
	m_Address = sBdAddr;
	
	std::ostringstream ostr; ostr << "Create BTDevcie: " << sBdAddr; Logger_Write(LoggLevel::DBG1, ostr.str());
}

//construct with the BT Address like (28:E1:4C:CA:2F:1D) as a internal format
BluetoothDevice::BluetoothDevice(int Socket, int DeviceId, bdaddr_t BdAddr) :
    m_Handle(),
	m_DeviceId(DeviceId)
{
	Init(Socket, BdAddr);

	//convert to string and save
	char addr[19] = { 0 };
	ba2str(&BdAddr, addr);

	m_Address = addr;
}



BluetoothDevice::~BluetoothDevice()
{
}



//tries to read the remote name to find out if the Device is in range or not
bool BluetoothDevice::IsBTDeviceInRange()
{
	char cBtName[128];
	memset(cBtName, 0, sizeof(cBtName));

	if (m_Socket < 0)
		return false; //definitly not in range - i am missing a SOCKET :(

	//try to read name for check if in range or not
	if (hci_read_remote_name(m_Socket, &m_BdAddr, _countof(cBtName), cBtName, 20000) < 0)
	{
		std::ostringstream strMsg;
		strMsg << "BTDevice " << m_Name << " No Not in Range";
		Logger_Write(LoggLevel::DBG2, strMsg.str());
		return false;
	}   
	else
	{
		std::ostringstream strMsg;
		strMsg << "BTDevice " << m_Name << " Yes in Range";
		Logger_Write(LoggLevel::DBG2, strMsg.str());
		m_Name = cBtName; //remember the name
		return true; //in range
	}
}


//request the name and evaluate the debouncing
bool BluetoothDevice::DebounceIsInRange(bool *pStateChange)
{
	//remember the old state to be able to report it back
	bool OldStateInRange = m_InRange;
	bool NewStateInRange;

	//debounce the bluetooth device
	if (IsBTDeviceInRange()) //in range?
		NewStateInRange = m_Deb.Increment();
	else
		NewStateInRange = m_Deb.Decrement();

	//convert m_InRange to true / false
	if (pStateChange)
	{
		if (NewStateInRange != OldStateInRange) //a state change
			*pStateChange = true;
		else
			*pStateChange = false;

		if (*pStateChange)
		{
			std::ostringstream strMsg;
			strMsg << "Debounce State Change In Range: " << (NewStateInRange ? (std::string) "Yes" : (std::string) "No");
			Logger_Write(LoggLevel::DBG1, strMsg.str());
		}
	}

	m_InRange = NewStateInRange;
	
	return NewStateInRange;
}


