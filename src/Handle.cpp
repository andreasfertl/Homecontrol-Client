#include <iostream>
#include "project.h"

#include "Handle.h"


static HANDLE::Handle m_HandleCtr = 0;

Handle::Handle() :
    m_Handle(0)
{
    //remember my unique handle
    m_Handle = m_HandleCtr;
    //increase the unique handle
    m_HandleCtr++;
    
    //just debug output
    std::ostringstream strMsg;
    strMsg << "Handle generated" << " Value : " << m_Handle;
    Logger_Write(LoggLevel::DBG1, strMsg.str());
}









	  