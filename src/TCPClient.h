#pragma once

#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>


class TCPClient
{
private:
	int m_Sock;
	std::string m_Address;
	unsigned int m_Port;
	struct sockaddr_in m_Server;
	unsigned int m_TimeoutInSec;

	std::pair<bool, std::string> Receive(int recLength);

public:
	TCPClient(const std::string& address, unsigned int port, unsigned int timeoutInSec = 1);
	bool Connect();
	void Disconnect();
	bool SendLine(const std::string& data); //send a \r\n line
	std::string ReceiveLine(); //receive a \r\n line
};


