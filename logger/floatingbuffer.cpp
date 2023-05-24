#include "floatingbuffer.h"
#include "stringpiece.h"
#include <cassert>
#include <cstring> //memset,bzero



using std::string;

const int kSmallBuffer = 1024;
const int kLargeBuffer = 1024 * 1024;

template <int SIZE>
RsFloatingBuffer<SIZE>::RsFloatingBuffer()
    : data_(NULL)
    , cur_(NULL)
    , alloc_size_(0)
{
    if (SIZE < kSmallBuffer)
    {
        alloc_size_ = kSmallBuffer;
    }
    else if (SIZE > kLargeBuffer)
    {
        alloc_size_ = kLargeBuffer;
    }
    else
    {
        alloc_size_ = SIZE;
    }

    cur_ = data_ = (char*)malloc(alloc_size_ * sizeof(char));
    memset(data_, 0, alloc_size_ * sizeof(char));

    assert(cur_ != NULL);
}

template <int SIZE>
RsFloatingBuffer<SIZE>::~RsFloatingBuffer()
{
    free(data_);
    data_ = cur_ = NULL;
}

template <int SIZE>
const char* RsFloatingBuffer<SIZE>::debugString()
{
    *cur_ = '\0';
    return data_;
}

template <int SIZE>
void RsFloatingBuffer<SIZE>::append(const char* buf, int len)
{
    if (avail() > len)
    {
        memcpy(cur_, buf, len);
        cur_ += len;
    }
    else if (alloc_size_ < kLargeBuffer)
    {
        size_t allocSize = alloc_size_;
        if (len >= 1024)
        {
            allocSize += len;
        }
        else
        {
            allocSize += 1024;
        }

        if (allocSize > kLargeBuffer)
        {
            allocSize = kLargeBuffer;
        }

        int curLen = length();
        char* tmpData = (char*) realloc(data_, allocSize);
        assert(tmpData);
        if (tmpData == NULL)
        {
            return;
        }
        data_ = tmpData;
        alloc_size_ = allocSize;
        cur_ = data_ + curLen;
        memset(cur_, 0, alloc_size_ - curLen);

        if (len > alloc_size_ - curLen)
        {
            memcpy(cur_, buf, alloc_size_ - curLen);
            cur_ += alloc_size_ - curLen;
        }
        else
        {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }
    else
    {
        memcpy(cur_, buf, avail());
        cur_ += avail();
    }
}

template <int SIZE>
const char* RsFloatingBuffer<SIZE>::data() const
{
    return data_;
}

template <int SIZE>
int RsFloatingBuffer<SIZE>::length() const
{
    int len = cur_ - data_;
    return len;
}

template <int SIZE>
char* RsFloatingBuffer<SIZE>::current()
{
    return cur_;
}

template <int SIZE>
int RsFloatingBuffer<SIZE>::avail() const
{
    return static_cast<int>(end() - cur_);
}

template <int SIZE>
void RsFloatingBuffer<SIZE>::add(size_t len)
{
    cur_ += len;
}

//reset只移动cur_，不修改里面存储的数据
template <int SIZE>
void RsFloatingBuffer<SIZE>::reset()
{
    cur_ = data_;
}

template <int SIZE>
void RsFloatingBuffer<SIZE>::bzero()
{
    ::memset(data_, 0, sizeof(char)*alloc_size_);
}

template <int SIZE>
string RsFloatingBuffer<SIZE>::toString() const
{
    return string(data_, length());
}

template <int SIZE>
RsStringPiece RsFloatingBuffer<SIZE>::toStringPiece() const
{
    return RsStringPiece(data_, length());
}

template <int SIZE>
const char* RsFloatingBuffer<SIZE>::end() const
{
    return data_ + alloc_size_;
}


// 显式实例化 C++ Primer P597
template class RsFloatingBuffer<RS_SMALL_BUFFER>;
template class RsFloatingBuffer<RS_LARGE_BUFFER>;
