#ifndef _SUBSCRIBETOMESSAGE_INCLUDED
#define _SUBSCRIBETOMESSAGE_INCLUDED

#include "Message.h"
#include "json.hpp"


//using namespace nlohmann;

using nlohmann::json;

class SubscribeToMessage
{
private:
	MESSAGE::ID m_IdToSubscribeTo;

public:
	SubscribeToMessage(MESSAGE::ID id);
	~SubscribeToMessage();

	json toJson();
};


#endif //_SUBSCRIBETOMESSAGE_INCLUDED
