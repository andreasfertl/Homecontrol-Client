#ifndef _UTIL_INCLUDED
#define _UTIL_INCLUDED

#include <unistd.h>
#include <string>
#include <chrono>

namespace UTIL
{
   void InitRandom();
	unsigned int GetRandom(unsigned int Min, unsigned int Max);
}


#endif // _UTIL_INCLUDED
