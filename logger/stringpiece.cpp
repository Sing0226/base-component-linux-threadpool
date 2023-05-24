#include "stringpiece.h"

RsStringArg::RsStringArg(const char* str)
	: str_(str) 
{

}

RsStringArg::RsStringArg(const std::string& str)
	: str_(str.c_str()) 
{
}

const char* RsStringArg::c_str() const
{
	return str_; 
}

RsStringPiece::RsStringPiece()
    : ptr_(NULL)
    , length_(0) 
{

}


RsStringPiece::RsStringPiece(const char* str)
    : ptr_(str)
    , length_(static_cast<int>(strlen(ptr_))) 
{

}


RsStringPiece::RsStringPiece(const unsigned char* str)
    : ptr_(reinterpret_cast<const char*>(str))
    , length_(static_cast<int>(strlen(ptr_))) 
{

}


RsStringPiece::RsStringPiece(const std::string& str)
    : ptr_(str.data())
    , length_(static_cast<int>(str.size())) 
{

}

RsStringPiece::RsStringPiece(const char* offset, int len)
    : ptr_(offset)
    , length_(len) 
{

}

// 返回的字符指针可能不以‘\0'结尾
const char* RsStringPiece::data() const 
{ 
    return ptr_;
}

int RsStringPiece::size() const 
{ 
    return length_;
}

bool RsStringPiece::empty() const 
{ 
    return length_ == 0; 
}

const char* RsStringPiece::begin() const 
{ 
    return ptr_;
}

const char* RsStringPiece::end() const 
{ 
    return ptr_ + length_;
}

//修改操作
void RsStringPiece::clear()
{
    ptr_ = NULL;
    length_ = 0;
}
void RsStringPiece::set(const char* buffer, int len)
{
    ptr_ = buffer;
    length_ = len;
}
void RsStringPiece::set(const char* str)
{
    ptr_ = str;
    length_ = static_cast<int>(strlen(str));
}
void RsStringPiece::set(const void* buffer, int len)
{
    ptr_ = reinterpret_cast<const char*>(buffer);
    length_ = len;
}
//下标运算符
char RsStringPiece::operator[](int i) const { return ptr_[i]; }
//由于stringPiece不具有字符串的控制权，所以删除只是移动游标和删减长度
//删除开头n个字符
void RsStringPiece::remove_prefix(int n)
{
    ptr_ += n;
    length_ -= n;
}
//删除结尾n个字符
void RsStringPiece::remove_suffix(int n)
{
    length_ -= n;
}
//重载比较运算符
bool RsStringPiece::operator==(const RsStringPiece& x) const
{
    return ((length_ == x.length_) &&
        (memcmp(ptr_, x.ptr_, length_) == 0));
}
bool RsStringPiece::operator!=(const RsStringPiece& x) const
{
    return !(*this == x);
}

int RsStringPiece::compare(const RsStringPiece& x) const
{
    int r = memcmp(ptr_, x.ptr_, length_ < x.length_ ? length_ : x.length_);
    if (r == 0)
    {
        if (length_ < x.length_)
            r = -1;
        else if (length_ > x.length_)
            r = +1;
    }
    return r;
}

std::string RsStringPiece::as_string() const
{
    return std::string(data(), size());
}

void RsStringPiece::CopyToString(std::string* target) const
{
    target->assign(ptr_, length_);
}

// 判断是否以x起始
bool RsStringPiece::starts_with(const RsStringPiece& x) const
{
    return ((length_ >= x.length_) && (memcmp(ptr_, x.ptr_, x.length_) == 0));
}
