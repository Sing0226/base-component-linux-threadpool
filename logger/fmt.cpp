#include "fmt.h"
#include <cassert>
#include <string>
#include <cstring>

//以fmt的格式输出T
template <typename T>
Fmt::Fmt(const char* fmt, T val)
{
    static_assert(std::is_arithmetic<T>::value == true, "Must be arithmetic type");
#ifdef __linux__
    length_ = snprintf(buf_, sizeof(buf_), fmt, val);
#elif defined(_WIN64) || defined(_WIN32)
    length_ = sprintf_s(buf_, sizeof(buf_), fmt, val);
#endif
    assert(static_cast<size_t>(length_) < sizeof(buf_));
}

const char* Fmt::data() const
{
    return buf_;
}

int Fmt::length() const
{
    return length_;
}


// 显式实例化
template Fmt::Fmt(const char* fmt, char);

template Fmt::Fmt(const char* fmt, short);
template Fmt::Fmt(const char* fmt, unsigned short);
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, unsigned int);
template Fmt::Fmt(const char* fmt, long);
template Fmt::Fmt(const char* fmt, unsigned long);
template Fmt::Fmt(const char* fmt, long long);
template Fmt::Fmt(const char* fmt, unsigned long long);

template Fmt::Fmt(const char* fmt, float);
template Fmt::Fmt(const char* fmt, double);

