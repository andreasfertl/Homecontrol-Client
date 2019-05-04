#ifndef _THREAD_INCLUDED
#define _THREAD_INCLUDED

#include <string>
#include <thread>

#include "timer.h"
#include "Queue.h"
#include "util.h"
#include "Message.h"
#include "threadsid.h"


namespace THREAD
{
	const unsigned int NR_OF_MESSAGES_IN_A_QUEUE = 128;
	const unsigned int MESSAGES_PER_RUN = (NR_OF_MESSAGES_IN_A_QUEUE/4); //trying to parse a 1/4 of all messages
	const unsigned int MIN_SLEEP_TIME_MS = 10;
}

typedef Queue::Queue<Message, THREAD::NR_OF_MESSAGES_IN_A_QUEUE> MQ;


class thread
{
	private:
		std::string    m_Name;                    //the name of the thread
		unsigned int   m_SleepMS;                 //idle time in MS (sleep)
		unsigned int   m_SleepHystereseMS;        //if you set this value to non zero you will get a sleep hysterese in around this value (MS)
		THREADSID::ThreadID   m_ThreadId;         //unique thread id
		bool  m_RequestStop;                      //if requested stop = true -> stop the thread
		std::thread    m_Thread;
		TIMER::tmr     m_RoundtripTimerMs;        //for meassuring the roundtrip of the thread, to get a more accurate cycling of the threads
		
		void Sleep();                             //will sleep for m_SleepMS +/- m_SleepHystereseMS
		void Sleep(TIMER::tmr RoundtripTimeMs);   //will sleep for m_SleepMS +/- m_SleepHystereseMS considering the roundtrip time
        
		int ParseMessageGeneral(Message &Msg);
        

		void HandleQueue(MQ &RxQueue)
		{
			//try to parse x times MESSAGES_PER_RUN
			for (unsigned int i = 0; i < THREAD::MESSAGES_PER_RUN; i++)
			{
				//still messages in the queue?
				if (!RxQueue.IsEmpty())
				{
					Message Msg;
					//get the Msg
					RxQueue.Pop(Msg);
					//and give it to the child class for parsing
                    if (ParseMessageGeneral(Msg) != RETVAL::OK) //retval OK means handled by caller
                        ParseMessage(Msg);  //no forward it to child
				}
				else
				{
					break; // nothing to do anymore just break and stop checking
				}
			}
		}
		
		void RunThread()
		{
			//run "forever"
			while (!m_RequestStop)
			{
				try
				{
					TIMER::Start(m_RoundtripTimerMs);

					//message handling
					HandleQueue(m_RxQueue);
					//really run the thread
					Run();

					//and now wait a little bit
					Sleep(TIMER::Elapsed(m_RoundtripTimerMs));
				}
				catch (...)
				{

				}
			}
		}

		protected:

		public:
			thread(std::string Name, unsigned int SleepMS, unsigned int SleepHystereseMS, THREADSID::ThreadID ThreadId);
			virtual ~thread() {};
			void RequestStop();
			void RequestRun();
			void SetCycleTime(unsigned int CycleTime) {m_SleepMS = CycleTime;} //in MS
			void SetHystereseTime(unsigned int HystereseTime) {m_SleepHystereseMS = HystereseTime;} //in MS
			THREADSID::ThreadID GetThreadId() {return m_ThreadId;}
			std::string GetName() {return m_Name;}
			virtual void Run() {};
			virtual int ParseMessage(Message &Msg) {return RETVAL::MESSAGE_NOT_HANDELD;};

			MQ  m_TxQueue;
			MQ  m_RxQueue;
};



#endif // _THREAD_INCLUDED
