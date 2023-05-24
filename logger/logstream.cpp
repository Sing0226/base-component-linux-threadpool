#include "logstream.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <stdio.h>   //snprintf
#include <cinttypes> //PRId64
#include <limits>    //numeric_limits
#include "stringpiece.h"
#include "floatingbuffer.h"
#include "fmt.h"

const int kSmallBuffer = 1024;
const int kLargeBuffer = 1024 * 1024;
const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
static_assert(sizeof(digits) == 20, "wrong number of digits");
const char digitsHex[] = "0123456789ABCDEF";
static_assert(sizeof(digitsHex) == 17, "wrong number of digitsHex");


// int到string转换函数,value->buf，10进制
template <typename T>
size_t convert(char buf[], T value)
{
    T i = value;
    char* p = buf;
    //当value为0时，也会往buf中加入一个字符0
    //如果有while()，则会得到一个空的Buf
    do
    {
        //T的类型可能不是int，只有int能有%运算符
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0)
    {
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);
    //返回得到的字符串的长度
    return p - buf;
}

// 16进制转化，int->char[]
size_t convertHex(char buf[], uintptr_t value)
{
    uintptr_t i = value;
    char* p = buf;

    do
    {
        int lsd = static_cast<int>(i % 16);
        i /= 16;
        *p++ = digitsHex[lsd];
    } while (i != 0);

    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

class RsLogStreamImpl
{
public:
    typedef RsFloatingBuffer<kSmallBuffer> Buffer;

    RsLogStreamImpl(RsLogStream& logStream);
    ~RsLogStreamImpl() {}
    RsLogStream& operator<< (const Buffer& v);
    //往data里加入len长的data
    const Buffer& buffer() const;
    void resetBuffer();
    Buffer buffer_;
    RsLogStream& m_logStream;
};

RsLogStreamImpl::RsLogStreamImpl(RsLogStream& logStream)
    : m_logStream(logStream)
{
}

RsLogStream::RsLogStream()
{
    m_impl = new RsLogStreamImpl(*this);
}

RsLogStream::~RsLogStream()
{
    if (m_impl)
    {
        delete m_impl;
        m_impl = nullptr;
    }
}


// 把intz转换成字符串再输入
template <typename T>
void RsLogStream::formatInteger(T v)
{
    if (m_impl->buffer_.avail() >= kMaxNumericSize)
    {
        size_t len = convert(m_impl->buffer_.current(), v);
        m_impl->buffer_.add(len);
    }
}

RsLogStream& RsLogStream::operator<< (bool v)
{
    m_impl->buffer_.append(v ? "1" : "0", 1);
    return *this;
}

// short提升到相应的int
RsLogStream& RsLogStream::operator<<(short v)
{
    *this << static_cast<int>(v);
    return *this;
}

RsLogStream& RsLogStream::operator<<(unsigned short v)
{
    *this << static_cast<unsigned int>(v);
    return *this;
}
// int,long,long long转换成字符串后插入
RsLogStream& RsLogStream::operator<<(int v)
{
    formatInteger(v);
    return *this;
}

RsLogStream& RsLogStream::operator<<(unsigned int v)
{
    formatInteger(v);
    return *this;
}

RsLogStream& RsLogStream::operator<<(long v)
{
    formatInteger(v);
    return *this;
}

RsLogStream& RsLogStream::operator<<(unsigned long v)
{
    formatInteger(v);
    return *this;
}

RsLogStream& RsLogStream::operator<<(long long v)
{
    formatInteger(v);
    return *this;
}

RsLogStream& RsLogStream::operator<<(unsigned long long v)
{
    formatInteger(v);
    return *this;
}

// 输出指针
RsLogStream& RsLogStream::operator<<(const void* p)
{
    uintptr_t v = reinterpret_cast<uintptr_t>(p);
    if (m_impl->buffer_.avail() >= kMaxNumericSize)
    {
        char* buf = m_impl->buffer_.current();
        buf[0] = '0';
        buf[1] = 'x';
        size_t len = convertHex(buf + 2, v);
        m_impl->buffer_.add(len + 2);
    }
    return *this;
}

RsLogStream& RsLogStream::operator<<(double v)
{
    if (m_impl->buffer_.avail() >= kMaxNumericSize)
    {
#ifdef __linux__
        int len = snprintf(m_impl->buffer_.current(), kMaxNumericSize, "%.12g", v);
#elif defined(_WIN64) || defined(_WIN32)
        int len = sprintf_s(m_impl->buffer_.current(), kMaxNumericSize, "%.12g", v);
#endif
        m_impl->buffer_.add(len);
    }
    return *this;
}

//重载浮点输出
RsLogStream& RsLogStream::operator<<(float v)
{
    *this << static_cast<double>(v);
    return *this;
}

//重载字符输出
RsLogStream& RsLogStream::operator<<(char v)
{
    m_impl->buffer_.append(&v, 1);
    return *this;
}

RsLogStream& RsLogStream::operator<<(const char* str)
{
    if (str)
    {
        m_impl->buffer_.append(str, strlen(str));
    }
    else
    {
        m_impl->buffer_.append("(null)", 6);
    }
    return *this;
}

//重载字符串输出
RsLogStream& RsLogStream::operator<<(const string& v)
{
    m_impl->buffer_.append(v.c_str(), v.size());
    return *this;
}

RsLogStream& RsLogStream::operator<<(const RsStringPiece& v)
{
    m_impl->buffer_.append(v.data(), v.size());
    return *this;
}

RsLogStream& RsLogStreamImpl::operator<<(const Buffer& v)
{
    this->m_logStream << v.toStringPiece();
    return this->m_logStream;
}

//往data里加入len长的data
void RsLogStream::append(const char* data, int len)
{ 
    m_impl->buffer_.append(data, len);
}

void RsLogStream::resetBuffer()
{
    m_impl->resetBuffer();
}

const char* RsLogStream::buff()
{
    return m_impl->buffer_.data();
}
int RsLogStream::len()
{
    return m_impl->buffer_.length();
}

const RsFloatingBuffer<kSmallBuffer>& RsLogStreamImpl::buffer() const
{ 
    return buffer_;
}

void RsLogStreamImpl::resetBuffer()
{ 
    buffer_.reset(); 
}

///////////////////////////////////////////////////////////////
RsLogStream& operator<< (RsLogStream& s, const Fmt& fmt)
{
    s.append(fmt.data(), fmt.length());
    return s;
}
