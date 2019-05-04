#ifndef __DEBOUNCER_H_
#define __DEBOUNCER_H_


typedef unsigned int DebCounter;


//this debouncer is not thread safe!
class Debouncer
{
   private:
      DebCounter   m_Ctr;     //internal counter of state
      bool         m_State;   //true (debounced) - false (not debounced)
      DebCounter   m_Max;     //at wich point it is HIGH
      DebCounter   m_Inc;     //value to INC
      DebCounter   m_Dec;     //value to DEC

   public:
      //construct Debouncer for example with Maximum = 10, increments = 5, decrements = 1 -> results in 2x increments (>=) true -> after 10x decrement -> false
      //1,1,1 means no debouncing at all (i kind of disabled)
      Debouncer(DebCounter Max, DebCounter Inc, DebCounter Dec); 
      Debouncer(); //will set default debcounters 1,1,1 -> wich means disabled
      ~Debouncer();
      
      void SetParameters(DebCounter Max, DebCounter Inc, DebCounter Dec);
      bool Increment();   //returns TRUE if in HIGH state
      bool Decrement();   //returns TRUE if in HIGH state
};



#endif //__DEBOUNCER_H_