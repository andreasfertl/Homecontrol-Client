#include <iostream>

#include "project.h"

#include "Logger.h"



void LoggerClass::OpenLogFile(std::string FileName)
{
	m_LoggLevel  = LoggLevel::WARNING;
	m_DebugLevel = LoggLevel::DBG1;
	m_FileStream.open(FileName, std::ios_base::app); //(app = append)
}

//loggs to file and to console if wanted
void LoggerClass::Log(LoggLevel::eLoggLevel LogLevel, std::string LogMsg)
{
	//logging to file
	if (m_FileStream.is_open() && ((LogLevel >= m_LoggLevel) || (LogLevel == LoggLevel::LOGFILE)))
		m_FileStream << LogMsg;

	//debug output to console
	if (LogLevel >= m_DebugLevel)
        std::cout << LogMsg;
}

void LoggerClass::CloseLogFile()
{
	m_FileStream.close();
}

void LoggerClass::SetLogLevel(LoggLevel::eLoggLevel LoggLevel)
{
	m_LoggLevel = LoggLevel;
}

void LoggerClass::SetDebugLevel(LoggLevel::eLoggLevel LoggLevel)
{
	m_DebugLevel = LoggLevel;
}










	  