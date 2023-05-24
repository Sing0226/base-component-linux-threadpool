#ifndef __CURTHREAD_H__
#define __CURTHREAD_H__

#if defined(_WIN64) || defined(_WIN32)
#include <Windows.h>
#include <processthreadsapi.h>
#endif // __linux__


#include <string>



namespace CurrentThread
{
#ifdef __linux__
    extern __thread int t_cachedTid;
    // 用string保存线程ID
    extern __thread char t_tidString[32];
    extern __thread int t_tidStringLength;
    extern __thread const char *t_threadName;

    void cacheTid();    // 缓存该线程的tid，即修改t_cachedTid的值

    // 返回线程ID
    inline int tid()
    {
        // 允许程序员将最有可能执行的分支告诉编译器
        // __builtin_expect((x),1)表示 x 的值为真的可能性更大；
        // __builtin_expect((x),0)表示 x 的值为假的可能性更大。
        // 一般情况下都不用调用cacheTid
        if (__builtin_expect(t_cachedTid == 0, 0))
        {
            cacheTid();
        }
        return t_cachedTid;
    }


    inline const char *tidString()
    {
        return t_tidString;
    }

    inline int tidStringLength()
    {
        return t_tidStringLength;
    }

    inline const char *name()
    {
        return t_threadName;
    }
#elif defined(_WIN32) || defined(_WIN64)
    extern char t_tidString[32];
    extern DWORD _cachedTid;
    extern int t_tidStringLength;

    void cacheTid(); 

    inline const char* tidString()
    {
        cacheTid();
        return t_tidString;
    }
    inline int tid()
    {
        return ::GetCurrentThreadId();
    }
    inline int tidStringLength()
    {
        return t_tidStringLength;
    }
#endif

    bool isMainThread();
    void sleepUsec(int64_t usec);
}





#endif // CURTHREAD_H
