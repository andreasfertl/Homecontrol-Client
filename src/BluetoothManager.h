#ifndef _BLUETOOTHMANAGER_INCLUDED
#define _BLUETOOTHMANAGER_INCLUDED

#include <vector>
#include <string>

#include "BluetoothDevice.h"



class BluetoothManager : public thread
{
   private:
		int m_Socket;                                   //one socket for all devices (your usb bluetooth dongle)
		std::vector<BluetoothDevice *> m_vpBTDevice;    //a vector of all BT devices

		BluetoothDevice *FindBtDevice(BluetoothDevice &BtDeviceToFind); //find corresponding BTDevice in our list
        int SendRangeChange(BluetoothDevice& BTDevice, bool inRange);
		int ParseMessage(Message &Msg);
		void Run();

	protected:

	public:
		BluetoothManager();
		virtual ~BluetoothManager();
		
		Handle RegisterBTDevice(std::string BTDeviceAddress);
};



#endif //_BLUETOOTH_INCLUDED