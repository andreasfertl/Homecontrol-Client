#ifndef _TELLDUSDEVICESET_INCLUDED
#define _TELLDUSDEVICESET_INCLUDED

#include <vector>
#include "json.hpp"

//using namespace nlohmann;

using nlohmann::json;

#include "TelldusDevice.h"

namespace TELLDUSMANAGER
{
	struct TeldusDeviceSetMessage
	{
		unsigned int m_Id;  //the telldus internal ID
		bool         m_On;  //true = on / false = off

		TeldusDeviceSetMessage(unsigned int id, bool on) :
			m_Id(id),
			m_On(on)
		{
		}

		TeldusDeviceSetMessage() :
			m_Id(0),
			m_On(false)
		{
		}
		inline TeldusDeviceSetMessage& operator=(const TeldusDeviceSetMessage& other)
		{
			// check for self-assignment
			if (&other == this)
				return *this;

			//update values
			m_Id = other.m_Id;
			m_On = other.m_On;
			return *this;
		}

		json toJson() {
			return json{ { "datatype", "TeldusDeviceSetMessage" },{ "id",  m_Id },{ "on", m_On } };
		}

		static TeldusDeviceSetMessage from_json(const std::string& str) {
			try
			{
				auto json = json::parse(str);
				auto id = json["Value"]["ID"].get<unsigned int>();
				auto on = json["Value"]["LightState"].get<bool>();
				return TeldusDeviceSetMessage(id, on);
			}
			catch (...)
			{
				return TeldusDeviceSetMessage();
			}
		}
	};

	struct TeldusDeviceSetMessages
	{
		std::vector<TeldusDeviceSetMessage> m_vMessages;

		TeldusDeviceSetMessages(std::vector<TeldusDeviceSetMessage>& vMessages) :
			m_vMessages(vMessages)
		{
		}

		TeldusDeviceSetMessages()
		{
		}

		inline TeldusDeviceSetMessages& operator=(const TeldusDeviceSetMessages& other)
		{
			// check for self-assignment
			if (&other == this)
				return *this;

			//update value
			m_vMessages = other.m_vMessages;
			return *this;
		}
	};

}

#endif