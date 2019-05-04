#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <algorithm>
#include <sstream>
#include <iostream>
#include <iterator>
#include <iomanip>
#include <thread>
  
#include "project.h"
#include "timer.h"

//implements a very simple MS timer wich is easy to ceck like if(Elapsed(timer) ? return : wait
namespace TIMER
{
	//Sleep for given milliseconds
	void Sleep(unsigned int SleepMS)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(SleepMS));
	}

	std::string GetCurrentTimeFormated()
	{
	    struct timeval tv;
	    time_t curtime;                                                                    
	    struct tm * timeinfo;
		 std::ostringstream strDateTime;
	    
	    gettimeofday(&tv, nullptr);                                                           
	    curtime = tv.tv_sec;                                                                 
	    timeinfo = localtime(&curtime);


		 strDateTime << std::setfill('0') << std::setw(4) << timeinfo->tm_year+1900 << "-" \
		             << std::setfill('0') << std::setw(2) << timeinfo->tm_mon+1     << "-" \
		             << std::setfill('0') << std::setw(2) << timeinfo->tm_mday      << " " \
		             << std::setfill('0') << std::setw(2) << timeinfo->tm_hour      << ":" \
		             << std::setfill('0') << std::setw(2) << timeinfo->tm_min       << ":" \
		             << std::setfill('0') << std::setw(2) << timeinfo->tm_sec       << ":" \
		             << std::setfill('0') << std::setw(3) << (unsigned int)((tv.tv_usec+0.5)/1000); 
	    

	    return strDateTime.str();
	}

	//returns time in Milliseconds unsigned since 1970
	std::uint64_t GetTimeEpochMS(void)
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}  

	//returns time in Seconds unsigned since 1970
	std::uint64_t GetTimeEpochS(void)
	{
		return (GetTimeEpochMS()+0.5)/1000;
	}  

	//returns ticker in MS
	tmr GetTicker()
	{
	   struct timeval tv;
	   gettimeofday(&tv,nullptr);
	   return (tmr)(tv.tv_sec*1000 + (tv.tv_usec / 1000));
	}

	//Start
	void Start(tmr &Timer)
	{
		Timer = GetTicker();
	}

	//Timer elapsed? - returns MS ticks
	//
	//usage periodicly check timer
	//
	//	TIMER::Start(CheckTimer);
	//
	// do
	// if (RxChar != 0)
	//     return RETVAL::RxedChar;
	// if (TIMER::Elapsed(CheckTimer) >= 2000)
	//     return RETVAL::Timeout
	//  
	tmr Elapsed(tmr &Timer)
	{
	   tmr Now = GetTicker();
		return (Now - Timer);
	}

	//busy delay for given MS
  	//
	//usage for 5 seconds:
	//
	//	TIMER::Delay(5000);
	void Delay(tmr Ms)
	{
	   tmr Timer;
	   
	   Start(Timer);
	   while(Elapsed(Timer) < Ms);
	}
}


