#include "timestamp.h"
#if defined(_WIN64) || defined(_WIN32)
#include <Windows.h>
#include <time.h>
#endif

#ifdef __linux__
    static_assert(sizeof(RsTimestamp) == sizeof(int64_t), "Timestamp is same size as int64_t");
#endif

std::string RsTimestamp::toString() const
{
    char buf[32] = { 0 };
    int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
    int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
#ifdef  __linux__
    snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
#elif defined(_WIN64) || defined(_WIN32)
    sprintf_s(buf, sizeof(buf), "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
#endif //  __linux__
    return buf;
}

std::string RsTimestamp::toFormattedString(bool showMicroseconds) const
{
    char buf[64] = { 0 };
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
    struct tm tm_time;
    // 将se这个秒数转换成以UTC时区为标准的年月日时分秒时间
#ifdef __linux__
    gmtime_r(&seconds, &tm_time);
#elif defined(_WIN64) || defined(_WIN32)
    gmtime_s(&tm_time, &seconds);
#endif

    if (showMicroseconds)
    {
        // 格式为”年月日时:分:秒微妙“
        // 类似于 1994111512:30:29.123123
        int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
#ifdef  __linux__
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
            tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
            microseconds);
#elif defined(_WIN64) || defined(_WIN32)
        sprintf_s(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
            tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
            microseconds);
#endif //  __linux__

    }
    else
    {
#ifdef  __linux__
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
            tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
#elif defined(_WIN64) || defined(_WIN32)
        sprintf_s(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
            tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
#endif //  __linux__
    }
    return buf;
}

RsTimestamp RsTimestamp::now()
{
    struct timeval tv;
#ifdef __linux__
    gettimeofday(&tv, NULL);
#elif defined(_WIN64) || defined(_WIN32)
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;
    clock = mktime(&tm);
    tv.tv_sec = clock;
    tv.tv_usec = wtm.wMilliseconds * 1000;
#endif
    int64_t seconds = tv.tv_sec;
    return RsTimestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}
