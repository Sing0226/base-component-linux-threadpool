#ifndef _FLOATING_BUFFER_H__
#define _FLOATING_BUFFER_H__

#include "rslogger_declare.h"
#include<string>


#define RS_SMALL_BUFFER 1024
#define RS_LARGE_BUFFER (1024 * 1024)

class RsStringPiece;

template <int SIZE>
class RsFloatingBuffer
{
public:
    RsFloatingBuffer();
    ~RsFloatingBuffer();

    // 插入长度为len的buf字符数组
    void append(const char* buf, int len);

    const char* data() const;
    int length() const;

    char* current();
    int avail() const;
    void add(size_t len);

    // reset只移动cur_，不修改里面存储的数据
    void reset();
    void bzero();

    //转换成标准的string，在末尾加上‘\0'终止符
    const char* debugString();
    std::string toString() const;
    RsStringPiece toStringPiece() const;

private:
    const char* end() const;
    char* data_;
    char* cur_;       //类似尾后指针，可插入的第一个位置
    size_t alloc_size_;
};

#endif