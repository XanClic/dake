#ifndef DAKE__CONTAINER__ALGORITHM_HPP
#define DAKE__CONTAINER__ALGORITHM_HPP

#include <iterator>
#include <vector>


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
template<class InputContainer, class Function> static inline auto inject(const InputContainer &input, const _cvt(input) &initial, Function fn) -> _cvt(input)
{ return inject(input.begin(), input.end(), initial, fn); }

// Template resolver for whole containers
template<class InputContainer> static inline auto inject(const InputContainer &input, const _cvt(input) &initial, _cvt(input) (*fn)(const _cvt(input) &, const _cvt(input) &)) -> _cvt(input)
{ return inject(input.begin(), input.end(), initial, fn); }


// General implementation
template<class OutputContainer, class InputIterator, class Function>
static inline OutputContainer map(InputIterator first, InputIterator last, Function fn)
{
    OutputContainer out;

    while (first != last)
        out.push_back(fn(*(first++)));

    return out;
}

// Default for OutputContainer is std::vector
template<class InputIterator, typename T = typename std::iterator_traits<InputIterator>::value_type, class Function> static inline std::vector<T> map(InputIterator first, InputIterator last, Function fn)
{ return map<T>(first, last, fn); }

// User-defined output + template resolver
template<class OutputContainer, class InputIterator, typename T = typename std::iterator_traits<InputIterator>::value_type>
static inline OutputContainer map(InputIterator first, InputIterator last, T (*fn)(const T &))
{
    OutputContainer out;

    while (first != last)
        out.push_back(fn(*(first++)));

    return out;
}

// std::vector + template resolver
template<class InputIterator, typename T = typename std::iterator_traits<InputIterator>::value_type> static inline std::vector<T> map(InputIterator first, InputIterator last, T (*fn)(const T &))
{ return map<T>(first, last, fn); }

// user-defined + general function + whole container
template<class OutputContainer, class InputContainer, class Function> static inline OutputContainer map(const InputContainer &input, Function fn)
{ return map<OutputContainer>(input.begin(), input.end(), fn); }

// user-defined + template resolver + whole container
template<class OutputContainer, class InputContainer> static inline OutputContainer map(const InputContainer &input, _cvt(input) (*fn)(const _cvt(input) &))
{ return map<OutputContainer>(input.begin(), input.end(), fn); }

// std::vector + general function + whole container
template<class InputContainer, class Function> static inline auto map(const InputContainer &input, Function fn) -> std::vector<_cvt(input)>
{ return map<std::vector<_cvt(input)>>(input.begin(), input.end(), fn); }

// std::vector + template resolver + whole container
template<class InputContainer> static inline auto map(const InputContainer &input, _cvt(input) (*fn)(const _cvt(input) &)) -> std::vector<_cvt(input)>
{ return map<std::vector<_cvt(input)>>(input.begin(), input.end(), fn); }


#undef _cvt

}
}

#endif
