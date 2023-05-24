/*
 * WIN32 Events for POSIX
 */

#ifndef _SMART_EVENT_H__
#define _SMART_EVENT_H__

    #include "rthreadpooldefine.h"


    #if defined(_WIN32) && !defined(CreateEvent)
    #error Must include Windows.h prior to including pevents.h!
    #endif
    #ifndef WAIT_TIMEOUT
    #include <errno.h>
    #define WAIT_TIMEOUT ETIMEDOUT
    #endif

    #include <stdint.h>

    // Type declarations
    struct smart_event_t_;
    typedef smart_event_t_ *smart_event_t;

    // Function declarations
    smart_event_t CreateEvent(bool manualReset = false, bool initialState = false);
    int DestroyEvent(smart_event_t event);
    int WaitForEvent(smart_event_t event, uint64_t milliseconds = WAIT_INFINITE);
    int SetEvent(smart_event_t event);
    int ResetEvent(smart_event_t event);

    #ifdef WFMO
    int WaitForMultipleEvents(smart_event_t *events, int count, bool waitAll,
                                uint64_t milliseconds);
    int WaitForMultipleEvents(smart_event_t *events, int count, bool waitAll,
                                uint64_t milliseconds, int &index);
    #endif

    #ifdef PULSE
    int PulseEvent(neosmart_event_t event);
    #endif


#endif
