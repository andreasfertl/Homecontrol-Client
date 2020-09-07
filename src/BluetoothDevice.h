#ifndef _BLUETOOTHDEVICE_INCLUDED
#define _BLUETOOTHDEVICE_INCLUDED

#include <string>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include "Debouncer.h"
#include "Handle.h"
#include "threadsid.h"
#include "Message.h"

#include "json.hpp"

//using namespace nlohmann;

using nlohmann::json;

namespace BLUETOOTHDEVICE
{
    const unsigned int IN_RANGE_MAX         =  10;
    const unsigned int IN_RANGE_UP_COUNT    =  10;
    const unsigned int IN_RANGE_DOWN_COUNT  =  2;
    
    struct BluetoothDeviceMessage
    {

	public:
		BluetoothDeviceMessage() :
			deviceId(0),
			address(0),
			inRange(false)
		{
		}
		BluetoothDeviceMessage(int deviceId, const std::string& addr, bool inRange) :
			deviceId(deviceId),
			address(addr),
			inRange(inRange)
        {
        }

		json toJson(){
			return json{ { "$type", "Messanger.Msg, Messanger" },{ "Value",{ { "$type", "Messanger.BluetoothDevMessage, Messanger" },{ "InRange", inRange },{ "BTMacAddr", address },{ "DeviceId", deviceId } } },{ "Destination", THREADSID::ThreadID::ALL_SUBSCRIBERS },{ "Source", 0 },{ "RemoteHandle", 0 },{ "Command", MESSAGE::CMD::SET },{ "CommandType", MESSAGE::ID::BLUETOOTHDEVICE_IN_RANGE },{ "ObjectType", "Messanger.BluetoothDevMessage, Messanger, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null" } };
		}


		static BluetoothDeviceMessage from_json(const std::string& str) {
			try
			{
				auto json = json::parse(str);
				auto deviceId = json.at("DeviceId").get<int>();
				auto addr = json.at("BTMacAddr").get<std::string>();
				auto inRange = json.at("InRange").get<bool>();

				return BluetoothDeviceMessage(deviceId, addr, inRange);
			}
			catch (...)
			{
				return BluetoothDeviceMessage();
			}
		}

	private:
		int         deviceId;
		std::string address;
		bool        inRange;
	};
}


//represents a BT Device                    
class BluetoothDevice
{
   private:
      int          m_Socket;                    //bluetooth socket (this is your bluetooth dongle)
      bdaddr_t     m_BdAddr;                    //the low level addres like (28:E1:4C:CA:2F:1D) in the internal format
	  std::string  m_Address;                   //the same address just in string format
	  Handle       m_Handle;
      std::string  m_Name;                      //the actual name of the device (will be received from the BTDevice)
	  Debouncer    m_Deb;
	  bool         m_InRange;                   //reflects with TRUE or FALSE if the device is in range or not
	  int          m_DeviceId;
      
      void Init(int Socket, bdaddr_t &BdAddr);  //just a little helper to unify code from several constructors
      bool IsBTDeviceInRange();                 //asks for the name of the BT device to find out if it is in range or not

   protected:
      bool ThisBtDevice(bdaddr_t &BdAddr);

   public:

      BluetoothDevice(int Socket, int DeviceId, std::string &sBdAddr); //construct with the BT Address like (28:E1:4C:CA:2F:1D) as a string           and a handle to the socket of the BT Dongle
      BluetoothDevice(int Socket, int DeviceId, bdaddr_t BdAddr);      //construct with the BT Address like (28:E1:4C:CA:2F:1D) as a internal format  and a handle to the socket of the BT Dongle
      ~BluetoothDevice();

      inline bool operator==(const BluetoothDevice &other) const {return (bacmp(&m_BdAddr, &other.m_BdAddr) == 0);}
      inline bool operator!=(const BluetoothDevice &other) const {return (bacmp(&m_BdAddr, &other.m_BdAddr) != 0);}

      bool InRange(unsigned int InRange);               //evaluets the internal representation and returns true or false
      bool DebounceIsInRange(bool *pStateChange);     //asks for name, debounces in Range information and returns true or false to determine if in range or not
	  
	  Handle GetHandle() const { return m_Handle; };
	  std::string GetAddress() const { return m_Address; };
	  bdaddr_t GetBdAddress() const { return m_BdAddr; };
	  int GetDeviceId() const { return m_DeviceId; };
};



#endif //_BLUETOOTHDEVICE_INCLUDED