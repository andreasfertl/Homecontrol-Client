#ifndef _QUEUE_INCLUDED
#define _QUEUE_INCLUDED

#include <atomic>
#include "retval.h"

//this is just a LOCK FREE single producer single consumer queue
//feel free to use it in one thread to Push elements and in another
//thread to Pop Elements - BUT not more than that ;)
namespace Queue 
{
   typedef std::atomic<unsigned int> tQueueIdx;
   template<typename ElemtType, unsigned int Size>
   class Queue{                                                                                                             
                                                                                                                          
    public:                                                                                                               
      Queue() : m_TailIdx(0), m_HeadIdx(0){}                                                                              
      ~Queue() {}                                                                                                         
                                                                                                                          
      int Push(const ElemtType& NewElement);                                                                             
      int Pop(ElemtType& NewElement);   
	  bool IsLockFree();                                                                                   
                                                                                                                          
      bool IsEmpty() { return (m_TailIdx.load() == m_HeadIdx.load()); } //just means the queue is empty                                 
      enum {m_Capacity = Size+1}; //you can't use the last element -> so we add one (easier for the user)
                                                                                                                                
    private:                                                                                                              
      tQueueIdx     m_TailIdx;           //points always to next free element                                            
      tQueueIdx     m_HeadIdx;                                                                                   
      ElemtType     m_Buffer[m_Capacity];                                                                        

      unsigned int IncrementIndex(unsigned int Idx);                                                                     
   };                                                                                               
   
   template<typename ElemtType, unsigned int Size>                                                                          
   bool Queue<ElemtType, Size>::IsLockFree()                                             
   {                                                                                                                        
      return m_TailIdx.is_lock_free() ? true : false;
   }              
                                                                                                                          
   template<typename ElemtType, unsigned int Size>                                                                          
   unsigned int Queue<ElemtType, Size>::IncrementIndex(unsigned int CurrentIdx)                                             
   {                                                                                                                        
      return ((++CurrentIdx) % m_Capacity); //inc and wraparound                                                                                    
   }                                                                                                                        
                                                                                                                          
                                                                                                                          
   template<typename ElemtType, unsigned int Size>                                                                          
   int Queue<ElemtType, Size>::Push(const ElemtType& NewElement)                                                            
   {                                                                                                                        
      //point to next free idx
      tQueueIdx NextTailIdx(IncrementIndex(m_TailIdx.load()));
                                                                                                                          
      if(NextTailIdx.load() != m_HeadIdx.load()) //do we have space - THIS is ok due to the fact we are using std::atomic_uint!           
      {                                                                                                                     
         //yes, add the element                                                                                             
         m_Buffer[m_TailIdx.load()] = NewElement;                                                                                  
         m_TailIdx.store(NextTailIdx.load()); //point again to next free idx                                                                                                                   
         return RETVAL::OK;                                                                                                 
      }                                                                                                                     
      else                                                                                                                  
      {                                                                                                                     
         //no space - > we are FULL                                                                                         
         return RETVAL::QUEUE_FULL;                                                                                         
      }
   }                                                                                                                        
                                                                                                                          
   //returns >0 if there is a element available                                                                             
   //returns <0 in error                                                                                                    
   //returns 0 if empty                                                                                                     
   template<typename ElemtType, unsigned int Size>                                                                          
   int Queue<ElemtType, Size>::Pop(ElemtType& PopElement)                                                                     
   {                                                                                                                        
      if(IsEmpty())                                                                                                         
         return 0;                                                                                                          
                                                                                                                          
      //retrieve the element                                                                                                
      PopElement = m_Buffer[m_HeadIdx.load()];                                                                                     
      m_HeadIdx.store(IncrementIndex(m_HeadIdx.load()));                                                                                
      return 1; //return we retrieved one element                                                                           
   }   
}
#endif //_QUEUE_INCLUDED