#ifndef DAKE__HELPER__FUNCTION_HPP
#define DAKE__HELPER__FUNCTION_HPP

namespace dake
{
namespace helper
{

template<typename T>
T sum(const T &x, const T &y)
{
    return x + y;
}


template<typename T>
T difference(const T &x, const T &y)
{
    return x - y;
}


template<typename T>
T product(const T &x, const T &y)
{
    return x * y;
}


template<typename T>
T quotient(const T &x, const T &y)
{
    return x / y;
}


template<typename T>
T conjunction(const T &x, const T &y)
{
    return x & y;
}


template<typename T>
T disjunction(const T &x, const T &y)
{
    return x | y;
}


template<typename T>
T exclusive_disjunction(const T &x, const T &y)
{
    return x ^ y;
}

}
}

#endif
