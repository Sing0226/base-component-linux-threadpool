#pragma once
#include "rslogger_declare.h"
#include <cassert>
#include <string>
#include <cstring> 
using std::string;
class RsStringPiece;
class Fmt;
class RsLogStreamImpl;

class RS_CLASS_DECL RsLogStream
{
public:
    RsLogStream();
    ~RsLogStream();
    //C++内置类型，整形(字符，整形，bool)，浮点型，分别重载输出运算符
    //重载布尔值输出
    RsLogStream& operator<< (bool v);
    //重载4中整形输出
    RsLogStream& operator<< (short);
    RsLogStream& operator<< (unsigned short);
    RsLogStream& operator<< (int);
    RsLogStream& operator<< (unsigned int);
    RsLogStream& operator<< (long);
    RsLogStream& operator<< (unsigned long);
    RsLogStream& operator<< (long long);
    RsLogStream& operator<< (unsigned long long);
    //重载指针输出
    RsLogStream& operator<< (const void*);
    //重载浮点输出
    RsLogStream& operator<< (float v);
    RsLogStream& operator<< (double);
    //重载字符输出
    RsLogStream& operator<< (char v);
    RsLogStream& operator<< (const char* str);
    //重载字符串输出
    RsLogStream& operator<< (const string& v);
    RsLogStream& operator<< (const RsStringPiece& v);

    void append(const char* data, int len);
    void resetBuffer();

    const char* buff();
    int len();

    /*
    RsLogStream& operator<< (const Buffer& v);
    //往data里加入len长的data
    void append(const char* data, int len);
    const Buffer& buffer() const;
  
    */

private:
    template <typename T>
    void formatInteger(T);

    RsLogStreamImpl *m_impl;

    // 每个输入的数字最大的显示长度
    static const int kMaxNumericSize = 32;
};

RsLogStream& operator<< (RsLogStream& s, const Fmt& fmt);

