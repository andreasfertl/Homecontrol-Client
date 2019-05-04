#ifndef _TCPMANAGER_INCLUDED
#define _TCPMANAGER_INCLUDED

#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include "TCPClient.h"
#include "thread.h"
#include "Message.h"
#include "Queue.h"
#include "timer.h"

class TCPManager : public thread
{
private:
	TCPClient m_TCPClient;
	unsigned int m_State;
	std::vector<MESSAGE::ID> m_MsgsToForward;
	std::atomic<unsigned int> m_ReceiveState;
	std::thread m_ReceiveThread;
	TIMER::tmr m_CyclicSendTimer;

	typedef Queue::Queue<Message, THREAD::NR_OF_MESSAGES_IN_A_QUEUE> msgToSendQueue; //same amount of messages than the baseclass could put into my queue
	msgToSendQueue m_MsgToSendQueue;

	int ParseMessage(Message &Msg);
	void Run();
	void RunReceive();


protected:

public:
	TCPManager(const std::string& IP, unsigned int Port, const std::vector<MESSAGE::ID>& msgsToForward);
	virtual ~TCPManager();
};



#endif //_TCPMANAGER_INCLUDED