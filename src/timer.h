#ifndef _TIMER_INCLUDED
#define _TIMER_INCLUDED

#include <unistd.h>
#include <string>
#include <chrono>

namespace TIMER
{
	typedef uint_least32_t tmr;

	void Start(tmr &Timer);
	tmr Elapsed(tmr &Timer);
	void Delay(tmr Ms);

	void Sleep(unsigned int SleepMS);
	std::string GetCurrentTimeFormated();

	std::uint64_t GetTimeEpochMS();
	std::uint64_t GetTimeEpochS();
}


#endif // _TIMER_INCLUDED
