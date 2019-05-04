#include <cstring>
#include <map>

#include "project.h"

#include "Message.h"



namespace MESSAGE
{
	std::map<ID, std::string> ToStringMap = {
		{ID::INVALID,                           "INVALID"},
		{ID::TELLDUSDEVICE_SET_STATE,           "TELLDUSDEVICE_SET_STATE"},
        {ID::MANDOLYN_SENSOR,                   "MANDOLYN_SENSOR"},
        {ID::ARCTECH_SWITCH,                    "ARCTECH_SWITCH"},
        {ID::BLUETOOTHDEVICE_IN_RANGE,          "BLUETOOTHDEVICE_IN_RANGE"},
	};

	std::string IdToString(ID id)
	{
		if (ToStringMap.find(id) != ToStringMap.end()) //just means the ID is in the list
			return ToStringMap[id];
		else //not in least return default string
			return (std::string)"UNDEFINED";
	}
	
	ID IdFromString(std::string id)
	{
		for (auto StringMap : ToStringMap)
		{
			//.first is the ThreadID and .second is the string representation
			if (StringMap.second == id) //found it
				return StringMap.first;
		}
		//not found return default
		return (ID)0;
	}
}

Message::~Message()
{

}









	  