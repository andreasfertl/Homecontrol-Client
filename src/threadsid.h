#ifndef _THREADSID_INCLUDED
#define _THREADSID_INCLUDED

//this file is necessary to avoid a circular dependency from Message <-> thread

#include <string>

namespace THREADSID
{
	enum class ThreadID : unsigned int
	{
		INVALID = 0,
		PROGRAMMANAGER,
		BLUETOOTHMANAGER,
		PERSONMANAGER,
		TELLDUSMANAGER,
		TCPMANAGER,
        
		ALL_SUBSCRIBERS = 0xFFFFFFFE,
		BROADCAST = 0xFFFFFFFF
	}; //update ToString/FromString mapping
	
	std::string ToString(THREADSID::ThreadID ThreadID);
	THREADSID::ThreadID   FromString(std::string ThreadID);
}


#endif // _THREADSID_INCLUDED
