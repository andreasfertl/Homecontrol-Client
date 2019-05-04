#include <iostream>
#include <regex>
#include "../include/telldus-core.h"

#include "project.h"
#include "thread.h"
#include "StringTools.h"
#include "MandolynSensor.h"
#include "ArctechSwitch.h"

#include "TelldusManager.h"


namespace TelldusParser {

    bool ParseSensor(std::string& protocol, std::string& data, Message *pMsg, THREADSID::ThreadID threadId)
    {
        if (protocol == "mandolyn")
        {
            std::string model    = StringTools::GetStringByToken(data, "model:", ";");
            int         id       = StringTools::GetNumberByToken<int>(data, "id:");
            float       temp     = StringTools::GetNumberByToken<float>(data, "temp:");
            int         humidity = StringTools::GetNumberByToken<int>(data, "humidity:");
            
            //just easier handling with an additional MandolynSensor
            MandolynSensor SensorData;
            
            //temp and humidity
            if (model == "temperaturehumidity")
            {
                MandolynSensor Data(id, temp, humidity);
                SensorData = Data;
                
            }
            else //only temp
            {
                MandolynSensor Data(id, temp);
                SensorData = Data;
            }
            
            //report Message back
            Message Msg {MESSAGE::CMD::SET, THREADSID::ThreadID::ALL_SUBSCRIBERS, threadId, MESSAGE::ID::MANDOLYN_SENSOR, SensorData};
            *pMsg = Msg;
            return true;
        }
        return false;
    }
    
    bool ParseCommand(std::string& protocol, std::string& data, Message *pMsg, THREADSID::ThreadID threadId)
    {
        if (protocol == "arctech")
        {
            std::string model = StringTools::GetStringByToken(data, "model:", ";");
            
            if (model == "selflearning")
            {
                int house = StringTools::GetNumberByToken<int>(data, "house:");
                int unit = StringTools::GetNumberByToken<int>(data, "unit:");
                int group = StringTools::GetNumberByToken<int>(data, "group:");
                std::string method = StringTools::GetStringByToken(data, "method:", ";");

                //build switch command
                ArctechSwitch arSwitch(house, unit, group, (method == "turnon") ? true : false);
                //report Message back
                Message Msg {MESSAGE::CMD::SET, THREADSID::ThreadID::ALL_SUBSCRIBERS, threadId, MESSAGE::ID::ARCTECH_SWITCH, arSwitch};
                *pMsg = Msg;
                return true;
            }
        }
        return false;
    }
    

    //reports back true in the case of pMsg was filled in
    bool Parse(std::string& data, Message *pMsg, THREADSID::ThreadID threadId)
    {
        //find out class
        std::string classtype = StringTools::GetStringByToken(data, "class:", ";");
        std::string protocol = StringTools::GetStringByToken(data, "protocol:", ";");
        
        if (classtype == "sensor")
            return ParseSensor(protocol, data, pMsg, threadId);
        else if (classtype == "command")
            return ParseCommand(protocol, data, pMsg, threadId);
        else
            return false;
    }
}



namespace TelldusMgr {
    //typedef void (WINAPI *TDRawDeviceEvent)(const char *data, int controllerId, int callbackId, void *context);
    static void RawDeviceEvent(const char *data, int controllerId, int callbackId, void *context)
    {
        if (!context)
            return;
        //transfer to our instance
        ((TelldusManager *)context)->ParseRawDeviceEvent(data, controllerId, callbackId);
    }
}



TelldusManager::TelldusManager() : thread((std::string)__FILE__, 500, 100, THREADSID::ThreadID::TELLDUSMANAGER)
{
	//open
	tdInit();

	int NrOfDevices = tdGetNumberOfDevices();

	//go through all devices
	for (int i = 0; i < NrOfDevices; i++)
	{
		//collect the corresponding data
		int id = tdGetDeviceId(i);
		char *pName = tdGetName( id );
		std::string sName(pName);
		tdReleaseString(pName); //and release it directly i am working on with the std::string

		//add to our list of devices
		auto *pTelldusDevice = new TelldusDevice(id, sName);
		if (pTelldusDevice)
			m_vpTelldusDevices.push_back(pTelldusDevice); //add all of them to my internal list

		Logger_Write(LoggLevel::DBG2, "Added TelldusDevice with name: " + sName);
	}
	
	
    tdRegisterRawDeviceEvent(&TelldusMgr::RawDeviceEvent, this);
}     


TelldusManager::~TelldusManager()
{
	Logger_Write(LoggLevel::WARNING, (std::string)"Destructor from " +  __FILE__);

	//first wait for stopping our thread and then kill all things
	RequestStop();
	
	//delete devices
	for (auto pTelldusDevice : m_vpTelldusDevices)
	{
		delete pTelldusDevice; //delete my pointer to the Device
		pTelldusDevice = nullptr;
	}
	m_vpTelldusDevices.clear(); //and now we can clear the whole list
	
	//close the connection to the telldusdemon
	tdClose();
}



//handle the raw device event from tellstick
void TelldusManager::ParseRawDeviceEvent(const char *data, int controllerId, int callbackId)
{
    //just forward to our thread
    std::string sData(data);
    m_RawDeviceQueue.Push(sData);
}


//may return nullptr if we don't find the device
TelldusDevice *TelldusManager::GetSpecificDevice(int Id)
{	
	TelldusDevice SearchForTelldusDevice(Id, (std::string)"search");
	
	for (auto pTelldusDevice : m_vpTelldusDevices)
	{
		//should actually not happen that we stored nullpointers in our vector
		if (pTelldusDevice)
		{
			//the device we are looking for?
			if (*pTelldusDevice == SearchForTelldusDevice)
			{	
				//yes - found it
				return pTelldusDevice;
			}	
		}
	}
	
	return nullptr;
}

//sets the state for a given id
bool TelldusManager::SetState(int Id, bool State)
{
	bool OldState = false;
	
	TelldusDevice *pDevice = GetSpecificDevice(Id);
	
	//device exists?
	if (pDevice)
		OldState = pDevice->SetState(State);
	
	return OldState;
}


int TelldusManager::ParseMessage(Message &Msg)
{
    switch (Msg.GetMsgId())
    {
        case MESSAGE::ID::TELLDUSDEVICE_SET_STATE:
        {
			Logger_Write(LoggLevel::ERROR, "Try to set light");
            if (!Msg.IsType(typeid(TELLDUSMANAGER::TeldusDeviceSetMessage)))
                return RETVAL::MESSAGE_WRONG_TYPE;
            auto msg(Msg.GetValue<TELLDUSMANAGER::TeldusDeviceSetMessage>());

            std::ostringstream strMsg2;
            strMsg2 << "SetState for ID: " << msg.m_Id << " ON: " << msg.m_On;
            Logger_Write(LoggLevel::ERROR, strMsg2.str());

            //now set the device
            SetState(msg.m_Id, msg.m_On);
            return RETVAL::OK;
        }
        
        default:
            break;
            
    }
	
    return RETVAL::MESSAGE_NOT_HANDELD;
}




void TelldusManager::Run()
{
    //try to parse maximum x times MESSAGES_PER_RUN
    for (unsigned int i = 0; i < TELLDUSMANAGER::MESSAGES_PER_RUN; i++)
    {
        //still messages in the queue?
        if (!m_RawDeviceQueue.IsEmpty())
        {
            //not empty retrieve the data string
            std::string data;
            m_RawDeviceQueue.Pop(data);
            
            //build a Msg wich will be filled in the case we have something to send
            Message TxMsg;
			if (TelldusParser::Parse(data, &TxMsg, GetThreadId())) //we returned something?
			{
				m_TxQueue.Push(TxMsg); //put it on out Queue
			}
        }
        else
        {
            break; // nothing to do anymore just break and stop checking
        }
    }
}








	  