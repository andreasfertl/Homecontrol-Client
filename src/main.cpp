#include <string>
#include <iostream>
#include <signal.h>

#include "project.h"
#include "timer.h"
#include "ProgramManager.h"
#include "timer.h"


#include "main.h"

sig_atomic_t gSigStopflag = 0;
sig_atomic_t gSigStatus = 0;
void SigFunc(int sig)
{
	// can be called asynchronously
    gSigStatus = sig; //remember status

    //break our endless running
    gSigStopflag = 1;
    
  	std::ostringstream strMsg;
	strMsg << "Received Signal " <<  sig;
	Logger_Write(LoggLevel::WARNING, strMsg.str());
}


int main(int argc, const char * argv[])
{
	// Register signals
	signal(SIGINT, SigFunc);
	signal(SIGUSR1, SigFunc);
    
    while (1)
    {
	    //the ProgramManager is responsible for start and stop (generate classes - destroy objects)
	    ProgramManager* pProgramManager = new ProgramManager();

	    while (!gSigStopflag)
	    {
		    //just run our program at a quite high speed
		    pProgramManager->Run();
		    TIMER::Sleep(10);
	    }

	    //now stop everything
	    delete pProgramManager;
        
        //check signal reload
        if (gSigStatus == SIGINT)
        {
            break; //just break the while and exit
        }
        else
        {
            //continue
            gSigStopflag = 0;
            gSigStatus = 0;
        }
    }

	return 0;
}



