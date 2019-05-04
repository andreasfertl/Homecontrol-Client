#include <string>
#include <iostream>

#include "project.h"
#include "Debouncer.h"



Debouncer::Debouncer(DebCounter Max, DebCounter Inc, DebCounter Dec):
                m_Max(Max),
                m_Inc(Inc),
                m_Dec(Dec)
{
    m_Ctr = 0;
    m_State = false;
}

Debouncer::Debouncer()
{
    m_Max = 1;
    m_Inc = 1;
    m_Dec = 1;
    m_Ctr = 0;
    m_State = false;
}

Debouncer::~Debouncer()
{
}

void Debouncer::SetParameters(DebCounter Max, DebCounter Inc, DebCounter Dec)
{
    m_Max = Max;
    m_Inc = Inc;
    m_Dec = Dec;
}

//increment to MAX
bool Debouncer::Increment()
{
    DebCounter Ctr = m_Ctr;

    Ctr += m_Inc;
		  
	if (Ctr >= m_Max)
    {
		Ctr = m_Max;   //limit the Ctr
        m_State = true;
    }

    //remmber the ctr
    m_Ctr = Ctr; 

    return m_State;
}

//decrement to 0
bool Debouncer::Decrement()
{
    DebCounter Ctr = m_Ctr;

    //ensure possible downount
	if (Ctr >= m_Dec)
        Ctr -= m_Dec;
    else
        Ctr = 0; //reset to 0 we are at the end

    if (!Ctr) //we are back on 0
       m_State = false;

    //remmber the ctr
    m_Ctr = Ctr; 
    
    return m_State;
}


