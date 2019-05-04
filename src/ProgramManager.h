#ifndef _PROGRAMMANAGER_INCLUDED
#define _PROGRAMMANAGER_INCLUDED

#include <vector>
#include <map>
#include <string>

#include "thread.h"
#include "Message.h"
#include "Configuration.h"

class ProgramManager
{
   private:
    class BluetoothManager* m_pBTManager        = nullptr; 
    class PersonManager*    m_pPersonManager    = nullptr;
	class TelldusManager*   m_pTelldusManager   = nullptr;
	class TCPManager*       m_pTCPManager       = nullptr;

	std::map<THREADSID::ThreadID, class thread *> m_pThreads;     //a map of all the threads in the system
	std::multimap<MESSAGE::ID, class thread *> m_pSubscribers;    //a multimap of Subscribers for corresponding MSGIds

    int GenerateThread(const ConfThreads& Thread, const Config& config);
	void AddThread(std::map<THREADSID::ThreadID, class thread *> &Threads, thread *pThread);
	class thread * FindThreadById(THREADSID::ThreadID sThreadId);
    int ConfigureSystem(const Config *pConf);

    void Init();
    void Stop();

   public:
      ProgramManager();
      ~ProgramManager();
      void Run();
};



#endif //_PROGRAMMANAGER_INCLUDED