#include <map>

#include "threadsid.h"

namespace THREADSID
{
	std::map<ThreadID, std::string> ToStringMap = {
		{ThreadID::INVALID,             "INVALID"},
		{ThreadID::PROGRAMMANAGER,      "PROGRAMMANAGER"},
		{ThreadID::BLUETOOTHMANAGER,    "BLUETOOTHMANAGER"},
		{ThreadID::PERSONMANAGER,       "PERSONMANAGER"},
		{ThreadID::TELLDUSMANAGER,      "TELLDUSMANAGER"},
		{ThreadID::TCPMANAGER,          "TCPMANAGER" },
	
		{ThreadID::ALL_SUBSCRIBERS,     "ALL_SUBSCRIBERS"},
		{ThreadID::BROADCAST,           "BROADCAST"}
	};


	std::string ToString(THREADSID::ThreadID ThreadID)
	{
		if (ToStringMap.find(ThreadID) != ToStringMap.end()) //just means the ID is in the list
			return ToStringMap[ThreadID];
		else //not in least return default string
			return ToStringMap[THREADSID::ThreadID::BROADCAST];
	}
	
	THREADSID::ThreadID FromString(std::string ThreadID)
	{
		for (auto StringMap : ToStringMap)
		{
			//.first is the ThreadID and .second is the string representation
			if (StringMap.second == ThreadID) //found it
				return StringMap.first;
		}
		//not found return default
		return THREADSID::ThreadID::BROADCAST;
	}
}
