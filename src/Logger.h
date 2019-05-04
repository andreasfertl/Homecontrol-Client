#ifndef _LOGGER_INCLUDED
#define _LOGGER_INCLUDED

#include <string>
#include <fstream>

#include <sstream>
#include <iostream>
#include "timer.h"
#include "Singleton.h"


namespace LoggLevel
{
   enum eLoggLevel { NONE = 0, LOGFILE = 1, DBG1 = 2, DBG2 = 3, DBG3 = 4, DBG4 = 5, WARNING = 6, ERROR = 7};
}


class LoggerClass 
{
   private:
      std::ofstream m_FileStream;
      LoggLevel::eLoggLevel m_LoggLevel;      //loglevel for the output to the logfile
      LoggLevel::eLoggLevel m_DebugLevel;     //loglevel for the output to the console
      
   public:
      LoggerClass() {};
      ~LoggerClass() {};
      void OpenLogFile(std::string FileName);
      void Log(LoggLevel::eLoggLevel LogLevel, std::string LogMsg);
      void CloseLogFile();
      void SetLogLevel(LoggLevel::eLoggLevel LoggLevel);
      void SetDebugLevel(LoggLevel::eLoggLevel LoggLevel);

};


typedef Singleton<LoggerClass> Logger;

//just send in _Lvl and a string to print to log
#define Logger_Write(_Lvl, _str) do {  std::ostringstream strBuilder;                              \
                                       strBuilder  << __FILE__ << " "                              \
                                       << "Line: " << __LINE__ << " "                              \
                                       << TIMER::GetCurrentTimeFormated() << " "                   \
                                       << (std::string)_str << std::endl;                          \
                                       Logger::Instance()->Log(_Lvl, strBuilder.str());            \
                                     } while(false)




#endif //_LOGGER_INCLUDED