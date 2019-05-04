#ifndef _TELLDUSDEVICE_INCLUDED
#define _TELLDUSDEVICE_INCLUDED

#include <string>

class TelldusDevice
{
    private:
    	unsigned int m_Id;              //unique identifier
        std::string m_sName;            //just a name
        bool GetLastSentState(int Id);

    public:
    	TelldusDevice(int Id, std::string sName);
		~TelldusDevice();

		//return true if the device is equal, false otherwise
        
        inline bool operator == (const TelldusDevice &other) const
{ return other.m_Id == m_Id; }

		bool SetState(bool State); //set ON/OFF - returns the old state

};


#endif //_TELLDUSDEVICE_INCLUDED
