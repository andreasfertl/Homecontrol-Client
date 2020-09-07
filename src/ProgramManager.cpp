#include <iostream>
#include <string>
#include <vector>
#include <sstream>


#include "project.h"
#include "thread.h"
#include "BluetoothManager.h"
#include "util.h"
#include "XMLParser.h"
#include "TelldusManager.h"
#include "TCPManager.h"
#include "Configuration.h"
#include "threadsid.h"
#include "ProgramManager.h"



//generate all objects
ProgramManager::ProgramManager()
{
	Init();
	Logger_Write(LoggLevel::DBG1, "Starting up");

	//read configuration
	Configuration Conf;
	XMLParser parser = XMLParser();
    parser.ReadFile("../LinuxClientConfig.xml", Conf);
    
    //and use it
    ConfigureSystem(Conf.GetConf());
}     


//and delete the objects
ProgramManager::~ProgramManager()
{
	//delete all threads to fire up all the threads
	for (auto& Thread : m_pThreads)
	{
		thread *pThread = Thread.second;
		if (pThread)
        {
			delete pThread;
            pThread = nullptr;
        }
	}   
    
	//remove all threads from the list
	m_pThreads.clear();
	//all subscribers
	m_pSubscribers.clear();
	
	Stop();
}

//apply the configuration
int ProgramManager::ConfigureSystem(const Config *pConf)
{
    if (!pConf) 
        return RETVAL::NULLPTR;
        
    //newup/generate all the threads they will not start, we need to call RequestRun() what happens later
	if (!pConf->m_Threads.empty())
    {
        for (auto &Thread : pConf->m_Threads)
            GenerateThread(Thread, *pConf);
    }

	//Set logger config
	Logger::Instance()->SetLogLevel(pConf->Logger.LogLvl);
	Logger::Instance()->SetDebugLevel(pConf->Logger.DebugLvl);

	//cycle through all my configured persons 
	if (!pConf->m_ConfPersons.empty())
	{
		//register every single person
		for (auto person : pConf->m_ConfPersons)
		{
            Handle BtHandle;
            
            if (m_pBTManager)
            {
			    //register BTDevice corresponding to person
			    BtHandle = m_pBTManager->RegisterBTDevice(person.DeviceId, person.BTDeviceAddr);
            }
            
			std::ostringstream strMsg;
			strMsg << "Configuration: " << person.ToString();
			Logger_Write(LoggLevel::DBG1, strMsg.str());
		}
	}	
	
	
	//build up subscriptions
	if (!pConf->m_MsgMap.empty())
	{
		//register every single map
		for (auto MsgMap : pConf->m_MsgMap)
		{
			Logger_Write(LoggLevel::DBG2, ((std::string)"Register " + MsgMap.MsgID + (std::string)" to " + MsgMap.ThreadID));
			m_pSubscribers.insert(std::pair<MESSAGE::ID, class thread *>(MESSAGE::IdFromString(MsgMap.MsgID), FindThreadById(THREADSID::FromString(MsgMap.ThreadID))));
		}
	}
    
	//now its time to fire up all the threads
	for (auto& Thread : m_pThreads)
	{
		thread *pThread = Thread.second;
		if (pThread)
			pThread->RequestRun();
	}   
    
    return RETVAL::OK;
}




void ProgramManager::AddThread(std::map<THREADSID::ThreadID, class thread *> &Threads, thread *pThread)
{
	Threads.insert(std::pair<THREADSID::ThreadID, class thread *>(pThread->GetThreadId(), pThread));
}


int ProgramManager::GenerateThread(const ConfThreads& Thread, const Config& config)
{
    thread * pThread = NULL;
    
    THREADSID::ThreadID ThreadId = THREADSID::FromString(Thread.ThreadID);
    
    switch (ThreadId)
    {
	case THREADSID::ThreadID::BLUETOOTHMANAGER:
            if (m_pBTManager)
                delete m_pBTManager;

        	pThread = m_pBTManager       = new BluetoothManager();
            break;

        case THREADSID::ThreadID::TELLDUSMANAGER:
            if (m_pTelldusManager)
                delete m_pTelldusManager;

        	pThread = m_pTelldusManager  = new TelldusManager();
            break;

		case THREADSID::ThreadID::TCPMANAGER:
			if (m_pTCPManager)
				delete m_pTCPManager;

			pThread = m_pTCPManager = new TCPManager(config.TCPClient.IP, config.TCPClient.Port, config.TCPClient.MsgIDs);
			break;

        default:
            return RETVAL::UNKNOWN_THREAD_ID;
    }
    
    if (pThread)
	{	
		if (Thread.CycleTime != -1)
			pThread->SetCycleTime(Thread.CycleTime);
		if (Thread.HystereseTime != -1)
			pThread->SetHystereseTime(Thread.HystereseTime);
    	AddThread(m_pThreads, (thread *)pThread);
    }    
    return RETVAL::OK;
}



class thread * ProgramManager::FindThreadById(THREADSID::ThreadID nThreadId)
{
	if (m_pThreads.find(nThreadId) != m_pThreads.end()) //just means the Dest Id is in the list
		return m_pThreads[nThreadId];
	else //to avoid crashes we take just pick one
		return m_pThreads[THREADSID::ThreadID::BROADCAST];
}

//go through all threads and copy messages back and forth
void ProgramManager::Run()
{
	//go through all threads and check for messages
	for (auto& Thread : m_pThreads)
	{
		//.second is the thread pointer and .first is the thread id
		thread *pThread = Thread.second;
		
		//just to be sure
		if (pThread)
		{
			//is there a message waiting?
			if (!pThread->m_TxQueue.IsEmpty())
			{
				//ok there is one, retrieve it and see what we need to to
				Message Msg;
				pThread->m_TxQueue.Pop(Msg);
				
				//forward to?
				//is destination in our list?
				THREADSID::ThreadID Dest = Msg.GetDest();
				if (m_pThreads.find(Dest) != m_pThreads.end()) //just means the Dest Id is in the list
				{
					thread *pDestThread = m_pThreads[Dest]; //it is save just to go into this arry, check is above
					if (pDestThread)
					{	
						//forward
						pDestThread->m_RxQueue.Push(Msg);
					}
					else
					{	
						std::ostringstream strMsg;
						strMsg << "Nullpointer - tried to send msg to: " << (int)Msg.GetDest();
						Logger_Write(LoggLevel::ERROR, strMsg.str());
					}
				}
				else if (Dest == THREADSID::ThreadID::ALL_SUBSCRIBERS) //send to all subscribers
				{
					//cycle through all subscribers
					std::pair<std::multimap<MESSAGE::ID, thread *>::iterator, std::multimap<MESSAGE::ID, thread *>::iterator> ppp;
					ppp = m_pSubscribers.equal_range(Msg.GetMsgId());

					//Logger_Write(LoggLevel::DBG4, (std::string)"search for Subscribers");
					for (std::multimap<MESSAGE::ID, thread *>::iterator it = ppp.first;  it != ppp.second; it++)
					{
                        thread *pThread = it->second;
     					if (pThread)
                            pThread->m_RxQueue.Push(Msg);
					}
				}		
				else //id is not in our list, i will send to all (broadcast)
				{
					Logger_Write(LoggLevel::DBG1, (std::string)"send Broadcast");
					for (auto& ThreadBroadcast : m_pThreads)
					{
						thread *pThread = ThreadBroadcast.second;
						if (pThread)
							pThread->m_RxQueue.Push(Msg);
					}
				}		
			}
		}
	}
}

#include "ArctechSwitch.h"


//NO objects are initialized until now - you can't use them
void ProgramManager::Init()
{
	UTIL::InitRandom();
	Logger::Instance()->OpenLogFile((std::string)"LinuxClientLog.txt");

	//!!!!!!!!!!!!!! TODO test
	//auto ar = ArctechSwitch(1, 2, 0, true);
/*	ArctechSwitch ar2 = ArctechSwitch::from_json(js);
	std::ostringstream ostr;
	ostr << std::boolalpha << ar2.IsOn();
	Logger_Write(LoggLevel::DBG1, ostr.str().c_str());*/
}

//NO objects are alive anymore
void ProgramManager::Stop()
{
	Logger_Write(LoggLevel::DBG1, "Shuting down");
	Logger::Instance()->CloseLogFile();
}