#ifndef __RS_TIMESTAMP_H__
#define __RS_TIMESTAMP_H__

#include "comparable.h"

#include <cinttypes> //int64_t
#include <string>

#ifdef __linux__
#include <sys/time.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <sys/types.h>
#endif




//提供更精确的Unix Timestamp
//系统时间只精确到秒，现在精确到微妙
//其含义是1970年01月01日00时00分00秒起至现在的的总微秒数
class RsTimestamp : public RsEqualityComparable<RsTimestamp>,
    public RsLessThanComparable<RsTimestamp>
{
public:
    RsTimestamp() : microSecondsSinceEpoch_(0) {}
    explicit RsTimestamp(int64_t microSecondsSinceEpochArg)
        : microSecondsSinceEpoch_(microSecondsSinceEpochArg)
    {
    }

    void swap(RsTimestamp& that)
    {
        std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
    }
    //转换成"seconds.microseconds"的格式小数点后保留6位，例如”123.213122“
    std::string toString() const;
    //转换成格式化的时间
    //
    std::string toFormattedString(bool showMicroseconds = true) const;

    bool valid() const { return microSecondsSinceEpoch_ > 0; }

    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
    time_t secondsSinceEpoch() const
    {
        return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
    }

    static RsTimestamp now();
    static RsTimestamp invalid()
    {
        return RsTimestamp();
    }
    static RsTimestamp fromUnixTime(time_t t)
    {
        return fromUnixTime(t, 0);
    }
    static RsTimestamp fromUnixTime(time_t t, int microseconds)
    {
        return RsTimestamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds);
    }
    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t microSecondsSinceEpoch_;
};

inline bool operator<(RsTimestamp lhs, RsTimestamp rhs)
{
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(RsTimestamp lhs, RsTimestamp rhs)
{
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

// 返回秒
inline double timeDifference(RsTimestamp high, RsTimestamp low)
{
    int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / RsTimestamp::kMicroSecondsPerSecond;
}

// 传入的参数是秒
inline RsTimestamp addTime(RsTimestamp timestamp, double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * RsTimestamp::kMicroSecondsPerSecond);
    return RsTimestamp(timestamp.microSecondsSinceEpoch() + delta);
}


#endif
