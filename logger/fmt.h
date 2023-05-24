#ifndef __RS_FMT_H__
#define __RS_FMT_H__
// 格式化输出
// 以fmt的格式输出T
// fmt类似printf函数的输出格式

class Fmt
{
public:
    template <typename T>
    Fmt(const char* fmt, T val);

    const char* data() const;
    int length() const;

private:
    char buf_[32];
    int length_;
};

#endif // FMT_H