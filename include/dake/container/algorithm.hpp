#ifndef DAKE__CONTAINER__ALGORITHM_HPP
#define DAKE__CONTAINER__ALGORITHM_HPP

#include <iterator>


namespace dake
{
namespace container
{

// container value type
#define _cvt(container) typename std::iterator_traits<decltype((container).begin())>::value_type


// General implementation
template<class InputIterator, typename T = typename std::iterator_traits<InputIterator>::value_type, class Function>
static inline T inject(InputIterator first, InputIterator last, const T &initial, Function fn)
{
    T value(initial);

    while (first != last)
        value = fn(value, *(first++));

    return value;
}

// This automatically resolves template functions given as function parameter
template<class InputIterator, typename T = typename std::iterator_traits<InputIterator>::value_type>
static inline T inject(InputIterator first, InputIterator last, const T &initial, T (*fn)(const T &, const T &))
{
    T value(initial);

    while (first != last)
        value = fn(value, *(first++));

    return value;
}

// General implementation for whole containers as input
template<class InputContainer, class Function>
auto inject(const InputContainer &input, const _cvt(input) &initial, Function fn) -> _cvt(input)
{
    return inject(input.begin(), input.end(), initial, fn);
}

// Template resolver for whole containers
template<class InputContainer>
auto inject(const InputContainer &input, const _cvt(input) &initial, _cvt(input) (*fn)(const _cvt(input) &, const _cvt(input) &)) -> _cvt(input)
{
    return inject(input.begin(), input.end(), initial, fn);
}


#undef _cvt

}
}

#endif
