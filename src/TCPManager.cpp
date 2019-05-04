#include <iostream>
#include <algorithm>
#include "project.h"
#include "MandolynSensor.h"
#include "BluetoothDevice.h"
#include "TelldusDeviceSet.h"
#include "ArctechSwitch.h"
#include "AliveMessage.h"
#include "SubscribeToMessage.h"
#include <functional>

#include "TCPManager.h"

namespace
{
	template<typename T>
	bool sendMessage(Message& msg, TCPClient& tcpclient)
	{
		T val(msg.GetValue<T>());
		return tcpclient.SendLine(val.toJson().dump().append(std::string("\r\n")));
	}
}

TCPManager::TCPManager(const std::string& IP, unsigned int Port, const std::vector<MESSAGE::ID>& msgsToForward) : thread((std::string)__FILE__, 1, 0, THREADSID::ThreadID::TCPMANAGER),
	m_TCPClient(IP, Port),
	m_State(0),
	m_MsgsToForward(msgsToForward),
	m_ReceiveState(0),
	m_ReceiveThread()
{
	for (auto msgid : m_MsgsToForward)
	{
		Logger_Write(LoggLevel::DBG2, std::string("Subscribed to:\r\n") + std::to_string((int)msgid));
	}
}


TCPManager::~TCPManager()
{
	RequestStop();
}


int TCPManager::ParseMessage(Message &Msg)
{
	auto msgId = Msg.GetMsgId();
	auto found = std::find_if(std::begin(m_MsgsToForward), std::end(m_MsgsToForward), [msgId](MESSAGE::ID id) { return id == msgId; });

	if (found != m_MsgsToForward.end())
	{
		//found it -> forward it to the tcpconnection
		Logger_Write(LoggLevel::DBG2, std::string("Handled id:") + std::to_string((int)msgId));
		m_MsgToSendQueue.Push(Msg);
		return RETVAL::OK;
	}
	else
	{
		Logger_Write(LoggLevel::DBG2, std::string("NOT handled id:") + std::to_string((int)msgId));
		//not handled
		return RETVAL::MESSAGE_NOT_HANDELD;
	}
}

void TCPManager::RunReceive()
{
	while (1)
	{
		switch (m_ReceiveState)
		{
			case 0:
			{
				auto receivedLine = m_TCPClient.ReceiveLine();
				if (receivedLine != std::string(""))
				{
					Logger_Write(LoggLevel::DBG1, std::string("Raw Data:\r\n") + receivedLine);
					try
					{
						std::string datatype;
						try
						{
							auto json = json::parse(receivedLine);

							if (!json["Value"]["$type"].is_null())
							{
								datatype = json["Value"]["$type"].get<std::string>();
								auto cmdTypeInt = json["CommandType"].get<int>();
								MESSAGE::ID cmdType = (MESSAGE::ID)cmdTypeInt;

								if (cmdType == MESSAGE::ID::TELLDUSDEVICE_SET_STATE && datatype == std::string("Messanger.LightMessage, Messanger"))
								{
									auto id = json["Value"]["ID"].get<int>();
									auto on = json["Value"]["LightState"].get<int>();
									auto data = TELLDUSMANAGER::TeldusDeviceSetMessage(id, on == 1);

									Message Msg{ MESSAGE::CMD::SET, THREADSID::ThreadID::ALL_SUBSCRIBERS, GetThreadId(), MESSAGE::ID::TELLDUSDEVICE_SET_STATE, data };
									m_TxQueue.Push(Msg);
									Logger_Write(LoggLevel::ERROR, "Lamp ID and State to change: " + std::to_string(data.m_Id) + " " + std::to_string(data.m_On));
									Logger_Write(LoggLevel::DBG1, data.toJson().dump());
								}
							}
						}
						catch (...)
						{
							Logger_Write(LoggLevel::DBG1, std::string("Exception in parsing"));
						}

						Logger_Write(LoggLevel::DBG1, "Datatype: " + datatype);

						/*if (datatype == std::string("ArctechSwitch"))
						{
							auto data = ArctechSwitch::from_json(receivedLine);
							Message Msg{ MESSAGE::CMD::SET, THREADSID::ThreadID::ALL_SUBSCRIBERS, GetThreadId(), MESSAGE::ID::ARCTECH_SWITCH, data };
							m_TxQueue.Push(Msg);
							Logger_Write(LoggLevel::DBG1, data.toJson().dump());
						}
						else if (datatype == std::string("BluetoothDeviceMessage"))
						{
							auto data = BLUETOOTHDEVICE::BluetoothDeviceMessage::from_json(receivedLine);
							Message Msg{ MESSAGE::CMD::SET, THREADSID::ThreadID::ALL_SUBSCRIBERS, GetThreadId(), MESSAGE::ID::BLUETOOTHDEVICE_IN_RANGE, data };
							m_TxQueue.Push(Msg);
							Logger_Write(LoggLevel::DBG1, data.toJson().dump());
						}
						else if (datatype == std::string("MandolynSensor"))
						{
							auto data = MandolynSensor::from_json(receivedLine);
							Message Msg{ MESSAGE::CMD::SET, THREADSID::ThreadID::ALL_SUBSCRIBERS, GetThreadId(), MESSAGE::ID::MANDOLYN_SENSOR, data };
							m_TxQueue.Push(Msg);
							Logger_Write(LoggLevel::DBG1, data.toJson().dump());
						}*/

					}
					catch (...)
					{
						Logger_Write(LoggLevel::ERROR, std::string("ExceptionInReceive"));
					}
				}
				break;
			}

			case 1:
				//handle disconnect, another thread will do this for me
				return;

			default:
				break;
		}

		TIMER::Sleep(100);
	}


}

void TCPManager::Run()
{
	switch (m_State)
	{
		case 0:
			if (m_TCPClient.Connect())
			{
				Logger_Write(LoggLevel::DBG1, (std::string)"connected");
				m_State = 1;
				//and activate receiver as well
				m_ReceiveState = 0;
				m_ReceiveThread = std::thread(&TCPManager::RunReceive, this);
				TIMER::Start(m_CyclicSendTimer);

				//subscribe to telldusdevice changes
				SubscribeToMessage msg{ MESSAGE::ID::TELLDUSDEVICE_SET_STATE };
				m_TCPClient.SendLine(msg.toJson().dump().append(std::string("\r\n")));
			}
			else
			{
				Logger_Write(LoggLevel::DBG1, (std::string)"connection failed");
				TIMER::Sleep(1000);
			}
			break;

		case 1:
			//connected keep connection alive by sending alive message
			if (TIMER::Elapsed(m_CyclicSendTimer) >= 5000)
			{
				Message Msg{ MESSAGE::CMD::SET, THREADSID::ThreadID::ALL_SUBSCRIBERS, GetThreadId(), MESSAGE::ID::ALIVE_MESSAGE, AliveMessage() };

				if (!sendMessage<AliveMessage>(Msg, m_TCPClient))
				{
					Logger_Write(LoggLevel::DBG2, (std::string)"Couldn't send anymore the alive message to the tcpsocket - reconnect");
					m_State = 2;
					break;
				}
				TIMER::Start(m_CyclicSendTimer);
			}

			//check for messages to send
			for (unsigned int i = 0; i < THREAD::MESSAGES_PER_RUN; i++)
			{
				//still messages in the queue?
				if (!m_MsgToSendQueue.IsEmpty())
				{
					//get the Msg
					Message Msg;
					m_MsgToSendQueue.Pop(Msg);
					bool sendOk = true;

					switch (Msg.GetMsgId())
					{
						case MESSAGE::ID::ARCTECH_SWITCH:
						{
							Logger_Write(LoggLevel::ERROR, (std::string)"Trying to send a ACTECH_SWITCH MESSAGE");
							sendOk = sendMessage<ArctechSwitch>(Msg, m_TCPClient);
							break;
						}

						case MESSAGE::ID::BLUETOOTHDEVICE_IN_RANGE:
						{
							Logger_Write(LoggLevel::ERROR, (std::string)"Trying to send a BLUETOOTHDEVICE_IN_RANGE MESSAGE");
							sendOk = sendMessage<BLUETOOTHDEVICE::BluetoothDeviceMessage>(Msg, m_TCPClient);
							break;
						}

						case MESSAGE::ID::MANDOLYN_SENSOR:
						{
							Logger_Write(LoggLevel::ERROR, (std::string)"Trying to send a MANDOLYN_SENSOR MESSAGE");
							sendOk = sendMessage<MandolynSensor>(Msg, m_TCPClient);
							break;
						}

						case MESSAGE::ID::ALIVE_MESSAGE:
						case MESSAGE::ID::TELLDUSDEVICE_SET_STATE:
						case MESSAGE::ID::STOP_ALL:
						case MESSAGE::ID::INVALID:
						case MESSAGE::ID::SUBSCRIBE:
						case MESSAGE::ID::RUN:
						case MESSAGE::ID::LogMessage:
						case MESSAGE::ID::LogLevel:
						case MESSAGE::ID::SunriseSunset:
						case MESSAGE::ID::CodeswitchButtonMessage:
						case MESSAGE::ID::SSHClientControl:
						case MESSAGE::ID::MusicControl:
						case MESSAGE::ID::MusicMessageState:
						case MESSAGE::ID::ConfigurationMessage:
						case MESSAGE::ID::MusicMessageVolume:
						case MESSAGE::ID::TempMessage:
						case MESSAGE::ID::LIGHTCONTROL:
							break;
					}
					if (!sendOk)
					{
						Logger_Write(LoggLevel::DBG1, (std::string)"Couldn't send anymore to the tcpsocket - reconnect");
						m_State = 2;
						break; //break the forloop, need to send later
					}
				}
				else
				{
					break; // nothing to do anymore just break and stop checking
				}
			}
			break;

		case 2:
		{
			m_TCPClient.Disconnect();
			Logger_Write(LoggLevel::DBG1, (std::string)"disconnect");
			m_State = 0;
			m_ReceiveState = 1;
			Logger_Write(LoggLevel::DBG1, (std::string)"join");
			m_ReceiveThread.join();
			Logger_Write(LoggLevel::DBG1, (std::string)"joined");
			break;
		}
	}
}





