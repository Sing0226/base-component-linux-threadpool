#include "rslogging.h"
#if defined(_WIN64) || defined(_WIN32)
#include <Windows.h>
#include <time.h>
#include <string.h>
#endif
#include <cerrno>
#include <cstring>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include "timestamp.h"
#include "curthread.h"
#include "rslog.h"
#include "floatingbuffer.h"
#include "fmt.h"
#include "timestamp.h"

#ifdef __linux__
__thread char t_errnobuf[512];
__thread char t_time[64];
__thread time_t t_lastSecond;
#elif defined(_WIN64) || defined(_WIN32)
char t_errnobuf[512];
char t_time[64];
time_t t_lastSecond;
#endif // __LINUX


const char *strerror_tl(int savedErrno)
{
#ifdef __linux__
    return strerror_r(savedErrno, t_errnobuf, sizeof(t_errnobuf));
#elif defined(_WIN64) || defined(_WIN32)
    strerror_s(t_errnobuf, sizeof(t_errnobuf), savedErrno);
    return t_errnobuf;
#endif // __LINUX
}

const char* source_file(const char* filePath)
{
    const char* fileName = nullptr;
    const char *slash = strrchr(filePath, '/');
    if (slash)
    {
        fileName = slash + 1;
    }

    return fileName;
}

// 方便传递string对象
class T
{
public:
    T(const char *str, unsigned len)
        : str_(str),
          len_(len)
    {
        assert(strlen(str) == len_);
    }

    const char *str_;
    const unsigned len_;
};

inline RsLogStream &operator<<(RsLogStream &s, T v)
{
    s.append(v.str_, v.len_);
    return s;
}

inline RsLogStream &operator<<(RsLogStream &s, const SourceFile &v)
{
    s.append(v.data_, v.size_);
    return s;
}

class RsLoggerImpl
{
public:
    RsLoggerImpl(ERsLogLevel level, int old_errno, const SourceFile& file, int line);
    ~RsLoggerImpl();
    void formatTime();
    void finish();

    RsTimestamp time_;
    RsLogStream stream_;
    ERsLogLevel level_;
    int line_;
    SourceFile basename_;
};

RsLoggerImpl::RsLoggerImpl(ERsLogLevel level, int savedErrno, const SourceFile &file, int line)
    : time_(RsTimestamp::now()),
      stream_(),
      level_(level),
      line_(line),
      basename_(file)
{
    formatTime();         //先记录时间
    // 输入日志级别
    stream_ << " [" << T(g_RsLog.getLogLevelName(level), strlen(g_RsLog.getLogLevelName(level))) << "] - ";
    CurrentThread::tid(); //取得该线程的tid
    // 输入线程id
    stream_ << T(CurrentThread::tidString(), CurrentThread::tidStringLength()) << " - ";

    // 如果出错则记录错误信息
    if (savedErrno != 0)
    {
        stream_ << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
    }
}

RsLoggerImpl::~RsLoggerImpl()
{

}

void RsLoggerImpl::formatTime()
{
    int64_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / RsTimestamp::kMicroSecondsPerSecond);
    int microseconds = static_cast<int>(microSecondsSinceEpoch % RsTimestamp::kMicroSecondsPerSecond);
    if (seconds != t_lastSecond)
    {
        t_lastSecond = seconds;
        struct tm tm_time;
#ifdef __linux__
        localtime_r(&seconds, &tm_time);
#elif defined(_WIN64) || defined(_WIN32)
        localtime_s(&tm_time, &seconds);
#endif // __linux__

#ifdef __linux__
        int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
                           tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                           tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
#elif defined(_WIN64) || defined(_WIN32)
        int len = sprintf_s(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
            tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
#endif
        assert(len == 17);
        (void)len;
    }

    Fmt us(".%06d ", microseconds);
    assert(us.length() == 8);
    stream_ << T(t_time, 17) << T(us.data(), 8);
}

void RsLoggerImpl::finish()
{
    stream_ << " - " << basename_ << ':' << line_ << '\n';
}

RsLogger::RsLogger(SourceFile file, int line)
    : m_impl_(new RsLoggerImpl(ERsLogLevel::RS_LOG_INFO, 0, file, line))
{
}

RsLogger::RsLogger(SourceFile file, int line, ERsLogLevel level, const char* func)
    : m_impl_(new RsLoggerImpl(level, 0, file, line))
{
    m_impl_->stream_ << func << ": ";
}

RsLogger::RsLogger(SourceFile file, int line, ERsLogLevel level)
    : m_impl_(new RsLoggerImpl(level, 0, file, line))
{
}

RsLogger::RsLogger(SourceFile file, int line, bool toAbort)
    : m_impl_(new RsLoggerImpl(toAbort ? ERsLogLevel::RS_LOG_FATAL : ERsLogLevel::RS_LOG_ERROR, 
        errno, file, line))
{
}

// 析构Loggger时，将stream里的内容通过g_output写入设置的文件中
RsLogger::~RsLogger()
{
    m_impl_->finish();
    g_RsLog.output(stream().buff(), stream().len());
    if (m_impl_)
    {
        delete m_impl_;
        m_impl_ = nullptr;
    }
}

ERsLogLevel RsLogger::logLevel()
{
    return g_RsLog.getLogLevel();
}

void RsLogger::setLogLevel(ERsLogLevel level)
{
    g_RsLog.setLogLevel(level);
}

RsLogStream& RsLogger::stream()
{
    return m_impl_->stream_;
}

