#ifndef _HANDLE_INCLUDED
#define _HANDLE_INCLUDED

namespace HANDLE {
    typedef unsigned int Handle;
}

class Handle
{
private:
    HANDLE::Handle        m_Handle;                //the unique handle
    
public:
    Handle();
    ~Handle() {};

    //return true if it is the same handle, false otherwise
    inline bool operator == (const Handle &other) const { return other.m_Handle == m_Handle; }
    
    //assignment operator
    inline Handle& operator=(const Handle& other)
    {
        // check for self-assignment
        if(&other == this)
            return *this;
        
        //update values
        m_Handle = other.m_Handle;
        return *this;
    }
};

#endif //_HANDLE_INCLUDED