#ifndef _CONFIGURATION_INCLUDED
#define _CONFIGURATION_INCLUDED

#include <vector>
#include <string>

#include "Logger.h"
#include "Message.h"

struct ConfLogicLightSensor
{
    std::string m_Name;
    unsigned int m_ArctechSwitch;
    unsigned int m_ArctechSwitchUnit;
    
    ConfLogicLightSensor(std::string Name, unsigned int ArctechSwitch, unsigned int ArctechSwitchUnit) : m_Name(Name), m_ArctechSwitch(ArctechSwitch), m_ArctechSwitchUnit(ArctechSwitchUnit){}
    ConfLogicLightSensor() : m_Name(""), m_ArctechSwitch(0), m_ArctechSwitchUnit(0){}
    
    std::string ToString()
    {
        std::ostringstream ostr;
        ostr << "Name = " << m_Name
        << " ArctechSwitch = " << m_ArctechSwitch
        << " ArctechSwitchUnit = " << m_ArctechSwitchUnit;
        return ostr.str();
    }
};

struct ConfLogicHueArctech
{
    std::string m_Name;
    unsigned int m_Lamp;
    unsigned int m_Brightness;
    unsigned int m_ArctechSwitch;
    unsigned int m_ArctechSwitchUnit;
    bool m_ArctechSwitchGroup;
    
    ConfLogicHueArctech(std::string Name, unsigned int Lamp, unsigned int Brightness, unsigned int ArctechSwitch, unsigned int ArctechSwitchUnit, bool ArctechSwitchGroup) : m_Name(Name), m_Lamp(Lamp), m_Brightness(Brightness), m_ArctechSwitch(ArctechSwitch), m_ArctechSwitchUnit(ArctechSwitchUnit), m_ArctechSwitchGroup(ArctechSwitchGroup){}

    std::string ToString()
    {
        std::ostringstream ostr;
        ostr << "Name = " << m_Name
             << " Lamp = " << m_Lamp
             << " Brightness = " << m_Brightness
             << " ArctechSwitch = " << m_ArctechSwitch
             << " ArctechSwitchUnit = " << m_ArctechSwitchUnit
            << " ArctechSwitchGroup = " << m_ArctechSwitchGroup;
        return ostr.str();
    }
};

typedef struct
{
	LoggLevel::eLoggLevel DebugLvl;
	LoggLevel::eLoggLevel LogLvl;
	std::string ToString() {std::ostringstream ostr; ostr << "DebugLvl = " << DebugLvl << " LogLvl = " << LogLvl; return ostr.str();}
}ConfLogger;

typedef struct
{
	std::string IP;
	unsigned int Port;
	std::vector<MESSAGE::ID>  MsgIDs;
	std::string ToString() { std::ostringstream ostr; ostr << "IP = " << IP << " Port = " << Port; return ostr.str(); }
}ConfTCPClient;


typedef struct
{
	std::string Name;
	std::string BTDeviceAddr;
	
	std::string ToString() {std::ostringstream ostr; ostr << "ConfPerson, Name = " << Name << " BTDeviceAddr = " << BTDeviceAddr; return ostr.str();}
}ConfPerson;

typedef struct
{
	std::string ThreadID;
	std::string MsgID;
	std::string ToString() {std::ostringstream ostr; ostr << "ThreadID = " << ThreadID << " MsgID = " << MsgID; return ostr.str();}
}ConfMsgMap;

typedef struct
{
	std::string ThreadID;
	int CycleTime;
	int HystereseTime;
	std::string ToString() {std::ostringstream ostr; ostr << "ThreadID = " << ThreadID << " CycleTime = " << CycleTime << " HystereseTime " << HystereseTime; return ostr.str();}
}ConfThreads;

typedef struct
{
	ConfLogger Logger;
	ConfTCPClient TCPClient;
	std::vector<ConfPerson>  m_ConfPersons; //a vector of configured persons
	std::vector<ConfThreads> m_Threads;     //a vector of wanted to start threads
	std::vector<ConfMsgMap>  m_MsgMap;      //a vector of configured msg mappings
    std::vector<ConfLogicHueArctech>  m_LogicHueArctech;
    std::vector<ConfLogicLightSensor>  m_LogicLightSensors;
}Config;


class Configuration
{
   private:
      Config   m_Configuration;

   public:
      Configuration() {};
      ~Configuration() {};
	  
	  void SetConfLogger(ConfLogger &Logger)   {m_Configuration.Logger = Logger;}
	  void SetConfTCPClient(ConfTCPClient &tcpClient) { m_Configuration.TCPClient = tcpClient; }
	  void AddConfPerson(ConfPerson &ToAdd)    {m_Configuration.m_ConfPersons.push_back(ToAdd);}
	  void AddConfThreads(ConfThreads &ToAdd)  {m_Configuration.m_Threads.push_back(ToAdd);}
	  void AddConfMsgMap(ConfMsgMap &ToAdd)    {m_Configuration.m_MsgMap.push_back(ToAdd);}
      void AddConfLogicHueArctech(ConfLogicHueArctech &ToAdd)    {m_Configuration.m_LogicHueArctech.push_back(ToAdd);}
      void AddConfLightSensor(ConfLogicLightSensor &ToAdd)    {m_Configuration.m_LogicLightSensors.push_back(ToAdd);}
	  
	  const Config *GetConf(void)   {return &m_Configuration;} //exposes whole configuration for easier access
};



#endif //_CONFIGURATION_INCLUDED