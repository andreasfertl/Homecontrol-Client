#include <string>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "project.h"
#include "Configuration.h"

#include "XMLParser.h"




XMLParser::XMLParser()
{
} 

XMLParser::~XMLParser()
{
}


//read subkeys (second) in a vector of Strings with the Schema Str2,Str2,.... from v 
unsigned int XMLParser::GetAllSubKeysOf(std::vector<std::string> &FoundSubkeys, std::string Str, boost::property_tree::ptree::value_type const& v)
{
	bool Stilfound = true;
	unsigned int i = 0;
	
	for (i++; Stilfound; i++)
	{
		std::ostringstream StrToSearch;
		StrToSearch << Str << i;
		//try to read  Str1, Str2, .....
		try
		{
			FoundSubkeys.push_back(v.second.get<std::string>(StrToSearch.str()));
			Logger_Write(LoggLevel::DBG1, StrToSearch.str() + " found");
			Stilfound = true;
		}
		catch (...)
		{
			Logger_Write(LoggLevel::DBG1, StrToSearch.str() + " not found anymore");
			Stilfound = false;
		}	
	}
	return i;
}

//builds up the configuration from the ptree to Conf parameter
int XMLParser::GetConfigurationFromXML(boost::property_tree::ptree &pt, Configuration &Conf)
{
	try
	{
		// traverse the tree
		BOOST_FOREACH(boost::property_tree::ptree::value_type const& v, pt.get_child("Configuration"))
		{
			if(v.first == "Person") //generating a Person
			{
				ConfPerson Person;
				Person.Name = v.second.get<std::string>("Name");
				Person.BTDeviceAddr = v.second.get<std::string>("BTDeviceAddr");
				
				Conf.AddConfPerson(Person);
				Logger_Write(LoggLevel::DBG1, "Added " + Person.ToString()); 
			}
			else if (v.first == "Logger") //set the logger
			{
				ConfLogger Logger;
				Logger.DebugLvl = (LoggLevel::eLoggLevel)v.second.get<int>("DbgLvl");
				Logger.LogLvl = (LoggLevel::eLoggLevel)v.second.get<int>("LogLvl");
				
				Conf.SetConfLogger(Logger);
			}
			else if (v.first == "TCPClient")
			{
				ConfTCPClient tcpClient;
				tcpClient.IP = v.second.get<std::string>("IP");
				tcpClient.Port = v.second.get<unsigned int>("Port");

				//build up the forward list of MsgIDs
				std::vector<std::string> FoundSubkeys;
				if (GetAllSubKeysOf(FoundSubkeys, "MsgID", v)) //found at least one
				{
					//cycle through all and add
					for (const auto& SubKey : FoundSubkeys)
					{
						tcpClient.MsgIDs.push_back(MESSAGE::IdFromString(SubKey));
						Logger_Write(LoggLevel::DBG1, " added " + SubKey + " to TCPClient");
					}
				}

				Conf.SetConfTCPClient(tcpClient);
			}
			else if(v.first == "Threads") //threads to start
			{
				ConfThreads ThreadsConf;
				std::vector<std::string> FoundSubkeys;
				int i = 0;
				
				//build up the combinations for ThreadID1...
				if (GetAllSubKeysOf(FoundSubkeys, "ThreadID", v)) //found at least one
				{
					//cycle through all and add
					for (auto SubKey : FoundSubkeys)
					{
						i++; //get the corresponding values
						ThreadsConf.ThreadID = SubKey;
						ThreadsConf.CycleTime = -1;
						ThreadsConf.HystereseTime = -1;
						
						try
						{
							std::ostringstream StrToSearch;
							StrToSearch << "CycleTime" << i;
							ThreadsConf.CycleTime = v.second.get<int>(StrToSearch.str());
						}	
						catch (...)
						{
						}

						try
						{
							std::ostringstream StrToSearch;
							StrToSearch << "HystereseTime" << i;
							ThreadsConf.HystereseTime = v.second.get<int>(StrToSearch.str());
						}	
						catch (...)
						{
						}		
										
						
						Conf.AddConfThreads(ThreadsConf);
						Logger_Write(LoggLevel::DBG1, ThreadsConf.ThreadID + " added");
					}
				}	
			}
			else if(v.first == "MsgMap") //map messages
			{
				ConfMsgMap MsgMap;
				std::vector<std::string> FoundSubkeys;
				
				//build up the combinations for ThreadID / MsgID
				MsgMap.ThreadID = v.second.get<std::string>("ThreadID");
				if (GetAllSubKeysOf(FoundSubkeys, "MsgID", v)) //found at least one
				{
					//cycle through all and add
					for (auto SubKey : FoundSubkeys)
					{
						MsgMap.MsgID = SubKey;
						Conf.AddConfMsgMap(MsgMap);
						Logger_Write(LoggLevel::DBG1, MsgMap.MsgID + " added for " + MsgMap.ThreadID);
					}
				}	
			}
            else if(v.first == "Logic")
            {
                BOOST_FOREACH(boost::property_tree::ptree::value_type const& v2, pt.get_child("Configuration.Logic"))
                {
                    if (v2.first == "HueLight")
                    {
                        ConfLogicHueArctech confLogicHueArctech(v2.second.get<std::string>("Name"),
                                                                v2.second.get<unsigned int>("Lamp"),
                                                                v2.second.get<unsigned int>("Brightness"),
                                                                v2.second.get<unsigned int>("ArctechSwitch"),
                                                                v2.second.get<unsigned int>("ArctechSwitchUnit"),
                                                                v2.second.get<bool>("ArctechSwitchGroup"));
                        
                        Conf.AddConfLogicHueArctech(confLogicHueArctech);
                        Logger_Write(LoggLevel::DBG1, "Added: " + confLogicHueArctech.m_Name);
                    }
                    else if (v2.first == "LightSensor")
                    {
                        ConfLogicLightSensor confLogicLightSensor(v2.second.get<std::string>("NameArctech"),
                                                                 v2.second.get<unsigned int>("ArctechSwitch"),
                                                                  v2.second.get<unsigned int>("ArctechSwitchUnit"));
                        Conf.AddConfLightSensor(confLogicLightSensor);
                        Logger_Write(LoggLevel::DBG1, "Added: " + confLogicLightSensor.m_Name);
                    }
                }
            }
		}
	}
	catch(std::exception const& ex)
	{
		Logger_Write(LoggLevel::ERROR, "Exception in parsing XMLFile iterating over nodes: " + ex.what()); 
		return RETVAL::XMLPARSER_MISSING_FILE;
	}
	catch (...)
	{
		Logger_Write(LoggLevel::ERROR, "Unknown exception in parsing XMLFile iterating over nodes"); 
		return RETVAL::XMLPARSER_MISSING_FILE;
	}
	
	return RETVAL::OK;
}


int XMLParser::ReadFile(std::string FileName, Configuration &Conf)
{
	//parse the xml file
	boost::property_tree::ptree pt;
	try
	{
		read_xml(FileName, pt);
	}
	catch (std::exception const& ex)
	{
		Logger_Write(LoggLevel::ERROR, "Exception in reading XMLFile: " + ex.what()); 
		return RETVAL::XMLPARSER_MISSING_FILE;
	}
	catch (...)
	{
		Logger_Write(LoggLevel::ERROR, "Unknown exception in reading XMLFile: " + FileName); 
		return RETVAL::XMLPARSER_MISSING_FILE;
	}

	//empty xml?
    if(pt.empty())
	{
    	Logger_Write(LoggLevel::ERROR, "XMLFile empty: " + FileName); 
		return RETVAL::XMLPARSER_MISSING_FILE;
	}
	else
	{
		//everything is good to go
		return GetConfigurationFromXML(pt, Conf);
	}
}     










	  