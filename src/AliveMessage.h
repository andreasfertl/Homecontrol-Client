#ifndef _ALIVEMESSAGE_INCLUDED
#define _ALIVEMESSAGE_INCLUDED

#include "json.hpp"

using nlohmann::json;

class AliveMessage
{
private:
	bool m_Alive;     //true = Alive

public:
	AliveMessage(bool alive = true) :
		m_Alive(alive)
	{
	}

	json toJson() {
		return json{ { "Value",{ { "alive", m_Alive } } },{ "Destination", THREADSID::ThreadID::ALL_SUBSCRIBERS },{ "Source", 0 },{ "RemoteHandle", 0 },{ "Command", MESSAGE::CMD::SET },{ "CommandType", MESSAGE::ID::ALIVE_MESSAGE },{ "MsgType", "Messanger.AliveMessage, Messanger, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null" } };
	}

	static AliveMessage from_json(const std::string& str) {
		try
		{
			auto json = json::parse(str);
			auto alive = json.at("alive").get<bool>();

			return AliveMessage(alive);
		}
		catch (...)
		{
			return AliveMessage();
		}
	}


};



#endif //_ALIVEMESSAGE_INCLUDED
#pragma once
