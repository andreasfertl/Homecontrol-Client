#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
 
#include "project.h"
#include "timer.h"
#include "util.h"

#include "thread.h"


//constructor for generating the thread
thread::thread(std::string Name, unsigned int SleepMS, unsigned int SleepHystereseMS, THREADSID::ThreadID ThreadId):
					m_Name(Name),
					m_SleepMS(SleepMS),
					m_SleepHystereseMS(SleepHystereseMS),
					m_ThreadId(ThreadId),
					m_RequestStop(false)
{
	//ensure valid values for the Sleep function
	if (m_SleepMS <= THREAD::MIN_SLEEP_TIME_MS)
		m_SleepMS = THREAD::MIN_SLEEP_TIME_MS;

	//m_SleepHystereseMS is not allowed to be higher than m_SleepMS! -> Sleep =  m_SleepMS - m_SleepHystereseMS
	if (m_SleepHystereseMS >= m_SleepMS)
		m_SleepHystereseMS = m_SleepMS/2;
	
	Logger_Write(LoggLevel::DBG1, "Created Thread: " + m_Name);
}

/*thread::~thread()
{
} */

//starts the actual thread, the static function RunCaller will call the child function
void thread::RequestRun()
{
	std::ostringstream LogMsg;
	LogMsg << "Request to run with id " << (int)m_ThreadId << " and name " << m_Name << " and ID String " << THREADSID::ToString(m_ThreadId);
	Logger_Write(LoggLevel::DBG2, LogMsg.str());
	m_Thread = std::thread(&thread::RunThread, this);
}

void thread::RequestStop()
{
    //send stop message to ourself
    //we can write safely from this context (main)
    //to our RX queue because the program Manager thread copying queueing already stopped
	Message Msg {MESSAGE::CMD::SET, GetThreadId(), GetThreadId(), MESSAGE::ID::STOP_ALL};		
	m_RxQueue.Push(Msg); 		
    
	//wait for thread to stop
	m_Thread.join();
	std::ostringstream LogMsg;
	LogMsg << "Thread with id " << (int)m_ThreadId << " and name " << m_Name << " stopped";
	Logger_Write(LoggLevel::DBG4, LogMsg.str());
}

void thread::Sleep()
{
	Sleep(0);
}


void thread::Sleep(TIMER::tmr RoundtripTimeMs)
{
	unsigned int Sleep;
	
	if (m_SleepHystereseMS)
	{
 		//hystere is wished -> +/-
		int sign = UTIL::GetRandom(1, 2);

		//sleep for +/- random value in range from 0 to m_SleepHystereseMS
		if (sign == 1)
			Sleep = m_SleepMS + UTIL::GetRandom(0, m_SleepHystereseMS);
		else
			Sleep = m_SleepMS - UTIL::GetRandom(0, m_SleepHystereseMS);
	}
	else
	{
		Sleep = m_SleepMS;
	}
	
	//shall we adjust according to roundtriptime?
	if (RoundtripTimeMs)
	{
		//adjust
		if (Sleep > RoundtripTimeMs)
			Sleep -= RoundtripTimeMs;
		else
			Sleep = 1;
	}
	
	TIMER::Sleep(Sleep);
}



int thread::ParseMessageGeneral(Message &Msg)
{
    if (Msg.GetCmd() == MESSAGE::CMD::SET && Msg.GetMsgId() == MESSAGE::ID::STOP_ALL)
    {
        m_RequestStop = true; //trigger stop
        return RETVAL::OK;
    }
    else    
        return RETVAL::MESSAGE_NOT_HANDELD;
}


