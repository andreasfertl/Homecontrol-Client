#include <iostream>
#include "project.h"

#include "SubscribeToMessage.h"

SubscribeToMessage::SubscribeToMessage(MESSAGE::ID id) :
	m_IdToSubscribeTo(id)
{
}

SubscribeToMessage::~SubscribeToMessage()
{

}

json SubscribeToMessage::toJson() {
	return json{ { "$type", "Messanger.SubscribeToMsg, Messanger" },{ "Value", m_IdToSubscribeTo },{ "Destination", THREADSID::ThreadID::ALL_SUBSCRIBERS },{ "Source", 0 },{ "RemoteHandle", 0 },{ "Command", MESSAGE::CMD::SET },{ "CommandType", MESSAGE::ID::SUBSCRIBE },{ "ObjectType", "Messanger.CommandType, Messanger, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null" } };
}














