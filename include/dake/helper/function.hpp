#ifndef DAKE__HELPER__FUNCTION_HPP
#define DAKE__HELPER__FUNCTION_HPP

namespace dake
{
namespace helper
{

template<typename T>
static inline T sum(const T &x, const T &y)
{
    return x + y;
}


template<typename T>
static inline T difference(const T &x, const T &y)
{
    return x - y;
}


template<typename T>
static inline T product(const T &x, const T &y)
{
    return x * y;
}


template<typename T>
static inline T quotient(const T &x, const T &y)
{
    return x / y;
}


template<typename T>
static inline T conjunction(const T &x, const T &y)
{
    return x & y;
}


template<typename T>
static inline T disjunction(const T &x, const T &y)
{
    return x | y;
}


template<typename T>
static inline T exclusive_disjunction(const T &x, const T &y)
{
    return x ^ y;
}

}
}

#endif
