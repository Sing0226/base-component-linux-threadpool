#ifndef __RS_COMPARABLE_H_
#define __RS_COMPARABLE_H_

// equality_comparable：要求提供==，可自动实现!=;
// less_than_comparable：要求提供<，可自动实现>、<=、>=;

template <class T>
class RsEqualityComparable
{
    friend bool operator!=(const T& lhs, const T& rhs)
    {
        return !(lhs == rhs);
    }
};

template <class T>
class RsLessThanComparable
{
    friend bool operator>=(const T& lhs, const T& rhs)
    {
        return !(lhs < rhs);
    }

    friend bool operator>(const T& lhs, const T& rhs)
    {
        return rhs < lhs;
    }

    friend bool operator<=(const T& lhs, const T& rhs)
    {
        return !(lhs > rhs);
    }
};

#endif
