#include "project.h"
#include "TCPClient.h"
#include "StringTools.h"
#include "timer.h"

#include<iostream>
#include<stdio.h>
#include<string>
#include<netdb.h>



TCPClient::TCPClient(const std::string& address, unsigned int port, unsigned int timeoutInSec) :
	m_Sock(-1),
	m_Address(address),
	m_Port(port),
	m_Server(),
	m_TimeoutInSec(timeoutInSec)
{
}

void TCPClient::Disconnect()
{
	close(m_Sock);
	m_Sock = -1;
}

bool TCPClient::Connect()
{
	//create socket if it is not already created
	if (m_Sock == -1)
	{
		//Create socket
		m_Sock = socket(AF_INET, SOCK_STREAM, 0);
		if (m_Sock == -1)
		{
			Logger_Write(LoggLevel::DBG1, "Could not create socket");
		}

		struct timeval tv;
		tv.tv_sec = m_TimeoutInSec;  /* 1 Secs Timeout */
		tv.tv_usec = 0;  // Not init'ing this can cause strange errors
		setsockopt(m_Sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(struct timeval));

		Logger_Write(LoggLevel::ERROR, "socket created");
	}

	//setup address structure
	auto retv = inet_addr(m_Address.c_str());
	if ((signed int)(retv) == -1)
	{
		struct hostent *he;
		struct in_addr **addr_list;

		//resolve the hostname, its not an ip address
		if ((he = gethostbyname(m_Address.c_str())) == NULL)
		{
			std::ostringstream strMsg;
			strMsg << "Failed to resolve hostname\n";
			Logger_Write(LoggLevel::DBG1, strMsg.str());
			m_Sock = -1;
			Logger_Write(LoggLevel::ERROR, "gethost failed!");
			return false;
		}

		//Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
		addr_list = (struct in_addr **) he->h_addr_list;

		for (int i = 0; addr_list[i] != NULL; i++)
		{
			m_Server.sin_addr = *addr_list[i];

			std::ostringstream strMsg;
			strMsg << m_Address << " resolved to " << inet_ntoa(*addr_list[i]) << std::endl;
			Logger_Write(LoggLevel::DBG2, strMsg.str());

			break;
		}
	}
	else //plain ip address
	{
		m_Server.sin_addr.s_addr = inet_addr(m_Address.c_str());
		Logger_Write(LoggLevel::ERROR, "added plain ip: " + m_Address);
	}

	m_Server.sin_family = AF_INET;
	m_Server.sin_port = htons(m_Port);
	Logger_Write(LoggLevel::ERROR, "Port " << m_Port);

	//Connect to remote server
	Logger_Write(LoggLevel::ERROR, "Trying to connect");
	if (connect(m_Sock, (struct sockaddr *)&m_Server, sizeof(m_Server)) < 0)
	{
		Logger_Write(LoggLevel::ERROR, "connect failed!");
		m_Sock = -1;
		return false;
	}

	Logger_Write(LoggLevel::ERROR, "should be connected");

	return true;
}

bool TCPClient::SendLine(const std::string& data)
{
	try
	{
		int status = ::send(m_Sock, data.c_str(), data.size(), MSG_NOSIGNAL);
		if (status == -1)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	catch (...)
	{
		return false;
	}

	return true;
}

std::pair<bool, std::string> TCPClient::Receive(int size = 1)
{
	char buffer[size + 1] = { 0 };

	//Receive a reply from the server
	auto retval = recv(m_Sock, buffer, size, 0);

	if (retval < 0)
	{
		//nothing received
		return std::make_pair(false, "");
	}
	else if (retval <= size) //ensure termination of partly received strings
	{
		buffer[retval] = '\0';
		return std::make_pair(true, std::string(buffer));
	}
	else
	{
		Logger_Write(LoggLevel::DBG1, std::string("To Big!\r\n"));
		return std::make_pair(false, "");
	}

}

std::string TCPClient::ReceiveLine()
{
	std::string recievedLine("");

	while (1)
	{
		auto ret = Receive();
		auto received = ret.first;
		auto receivedString = ret.second;

		//ok we received something
		if (received) 
		{
			//did we receive a full message?
			auto pos  = receivedString.find('\r');
			//did we find it in the string?
			if (pos != std::string::npos)
			{
				//yes - got a whole message, but it could be that we are missing the '\n' still
				auto pos2 = receivedString.find('\n');
				if (pos2 == std::string::npos)
				{
					//missing - just receiving the rest of the message which i can throw away
					Receive();
				}
				//there is a '\n' as well so we got the whole message
				auto returnVal = recievedLine.append(receivedString.substr(0, pos));
				//Logger_Write(LoggLevel::DBG1, std::string("RETURNING:\r\n") + returnVal);
				return returnVal;
			}
			else
			{
				//no, wait for \r\n
				recievedLine.append(receivedString.substr(0, pos));
			}
		}
		else
		{
			return std::string(""); //didn't receive anything come back later
		}
		TIMER::Sleep(1);
	}

	return std::string("");
}


