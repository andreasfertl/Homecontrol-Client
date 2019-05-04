#include <stdlib.h>
#include <time.h>


namespace UTIL
{

	void InitRandom()
	{
		srand(time(nullptr)*3.14f/5*98+1);
	}

	//e.g GetRandom(1, 10) -> results between 1 and 10
	unsigned int GetRandom(unsigned int Min, unsigned int Max)
	{
	  //generate secret number between Min and Max:
	  return ((rand() % Max) + Min);
	}
}

