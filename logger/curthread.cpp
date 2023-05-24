#include "curthread.h"
#include "timestamp.h"
#include <time.h>

#ifdef __linux__
    #include <sys/syscall.h>
    #include <unistd.h>
#elif defined(_WIN64) || defined(_WIN32)
    #include <Windows.h>
#endif


namespace CurrentThread
{
#ifdef __linux__
    pid_t gettid()
    {
      return static_cast<pid_t>(::syscall(SYS_gettid));
    }

    __thread int t_cachedTid = 0;
    __thread char t_tidString[32];
    __thread int t_tidStringLength = 6;
    __thread const char *t_threadName = "unknown";

    // 在本人系统上pid_t是int64_t类型，先注释
    // static_assert(std::is_same<int, pid_t>::value, "pid_t should be int64_t");
    void cacheTid()
    {
      if (t_cachedTid == 0)
      {
        t_cachedTid = gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%5d ", t_cachedTid);
      }
    }

    bool isMainThread()
    {
      // getpid获取的是进程号，每个进程的第一个线程号等于进程号
        return tid() == ::getpid();
    }
#elif defined(_WIN64) || defined(_WIN32)
    DWORD t_cachedTid = 0;
    int t_tidStringLength = 8;
    char t_tidString[32];
    void cacheTid()
    {
        t_cachedTid = GetCurrentThreadId();
        sprintf_s(t_tidString, "%08d", ::GetCurrentThreadId());
    }
#endif

 

    void sleepUsec(int64_t usec)
    {
#ifdef  __linux__ //  __linux__
        struct timespec ts = { 0, 0 };
        ts.tv_sec = static_cast<time_t>(usec / RsTimestamp::kMicroSecondsPerSecond);
        ts.tv_nsec = static_cast<long>(usec % RsTimestamp::kMicroSecondsPerSecond * 1000);
        ::nanosleep(&ts, NULL);
#elif defined(_WIN64) || defined(_WIN32)
        HANDLE timer;
        LARGE_INTEGER interval;
        interval.QuadPart = (10 * usec);

        timer = CreateWaitableTimer(NULL, TRUE, NULL);
        SetWaitableTimer(timer, &interval, 0, NULL, NULL, 0);
        WaitForSingleObject(timer, INFINITE);
        CloseHandle(timer);
#endif 
    }
}

