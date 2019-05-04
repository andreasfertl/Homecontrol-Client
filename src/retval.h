#ifndef _RETVAL_INCLUDED
#define _RETVAL_INCLUDED


namespace RETVAL
{
    const int OK                          =   0;
    const int ERR                         =  -1;

    const int NULLPTR                     =  -2;
    const int WRONG_AMOUNT_OF_ARGUMENTS   =  -3;

    const int QUEUE_FULL                  = -10;

    const int XMLPARSER_MISSING_FILE      = -20;
    const int XMLPARSER_MISSING_NODE      = -21;
    
    const int UNKNOWN_THREAD_ID           = -30;

    const int MESSAGE_NOT_HANDELD         = -40;
    const int MESSAGE_WRONG_TYPE          = -41;
}



#endif // _RETVAL_INCLUDED
