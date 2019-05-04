#ifndef _TELLDUSMANAGER_INCLUDED
#define _TELLDUSMANAGER_INCLUDED

#include <vector>
#include <string>
#include "Queue.h"
#include "Message.h"
#include "TelldusDeviceSet.h"
#include "TelldusDevice.h"

namespace TELLDUSMANAGER
{
	const unsigned int NR_OF_MESSAGES_IN_A_QUEUE = 64;
	const unsigned int MESSAGES_PER_RUN = (NR_OF_MESSAGES_IN_A_QUEUE / 2); //trying to parse a 1/2 of all messages
}

class TelldusManager : public thread
{
	private:
		std::vector<TelldusDevice *> m_vpTelldusDevices;    //a vector of all Telldus devices              

        typedef Queue::Queue<std::string, TELLDUSMANAGER::NR_OF_MESSAGES_IN_A_QUEUE> RawDeviceQueue; //just a simple queue with strings
        RawDeviceQueue  m_RawDeviceQueue;
    
		TelldusDevice *GetSpecificDevice(int Id);
		bool SetState(int Id, bool State);        
		int  ParseMessage(Message &Msg);
		void Run();
		
	protected:
	

	public:
		TelldusManager();
		virtual ~TelldusManager();
        void ParseRawDeviceEvent(const char *data, int controllerId, int callbackId);
};



#endif //_TELLDUSMANAGER_INCLUDED