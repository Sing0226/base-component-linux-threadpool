#pragma once

#include <cstring> 
#include <string>
#include <iosfwd>


class RsStringArg
{
public:
    RsStringArg(const char* str);
    RsStringArg(const std::string& str);

    const char* c_str() const;

private:
    const char* str_;
};



/// <summary>
/// RsStringPiece
/// </summary>

class RsStringPiece
{
public:
    //非explict的构造函数，使得const char*和string能隐式转化成StringPiece
    RsStringPiece();
    RsStringPiece(const char* str);
    RsStringPiece(const unsigned char* str);
    RsStringPiece(const std::string& str);
    RsStringPiece(const char* offset, int len);

    //返回的字符指针可能不以‘\0'结尾
    const char* data() const;
    int size() const;
    bool empty() const;
    const char* begin() const;
    const char* end() const;

    //修改操作
    void clear();
    void set(const char* buffer, int len);
    void set(const char* str);
    void set(const void* buffer, int len);
    //下标运算符
    char operator[](int i) const;
    //由于stringPiece不具有字符串的控制权，所以删除只是移动游标和删减长度
    //删除开头n个字符
    void remove_prefix(int n);
    //删除结尾n个字符
    void remove_suffix(int n);

    //重载比较运算符
    bool operator==(const RsStringPiece& x) const;
    bool operator!=(const RsStringPiece& x) const;

    int compare(const RsStringPiece& x) const;
    std::string as_string() const;


    void CopyToString(std::string* target) const;

    // 判断是否以x起始
    bool starts_with(const RsStringPiece& x) const;

    //定义一个宏简化stringpiece的比较操作
#define STRINGPIECE_BINARY_PREDICATE(cmp, auxcmp)                                \
    bool operator cmp(const RsStringPiece &x) const                              \
    {                                                                            \
        int r = memcmp(ptr_, x.ptr_, length_ < x.length_ ? length_ : x.length_); \
        return ((r auxcmp 0) || ((r == 0) && (length_ cmp x.length_)));          \
    }
    STRINGPIECE_BINARY_PREDICATE(< , < );
    STRINGPIECE_BINARY_PREDICATE(<= , < );
    STRINGPIECE_BINARY_PREDICATE(>= , > );
    STRINGPIECE_BINARY_PREDICATE(> , > );
#undef STRINGPIECE_BINARY_PREDICATE


private:
    const char* ptr_;
    int length_;
};
