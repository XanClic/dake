#ifndef DAKE__CONTAINER__ALGORITHM_HPP
#define DAKE__CONTAINER__ALGORITHM_HPP

#include <iterator>
#include <type_traits>
#include <vector>

#include "dake/helper/traits.hpp"


namespace dake
{
namespace container
{

// Container Value Type
#define _cvt(type) typename type::value_type
// Iterator Value Type
#define _ivt(type) typename std::iterator_traits<type>::value_type


template<typename T, typename V> static inline void _stuff(T &container, const V &value, typename std::enable_if<helper::has_push_back<T>::value>::type * = 0)
{ container.push_back(value); }
template<typename T, typename V> static inline void _stuff(T &container, const V &value, typename std::enable_if<helper::has_push<T>::value && !helper::has_push_back<T>::value>::type * = 0)
{ container.push(value); }


// General implementation
template<class InputIterator, typename Result, class Function>
static inline Result inject(InputIterator first, InputIterator last, const Result &initial, Function fn)
{
    Result value(initial);

    while (first != last)
        value = fn(value, *(first++));

    return value;
}

// This automatically resolves template functions given as function parameter
template<class InputIterator, typename Result, typename Input = _ivt(InputIterator)>
static inline Result inject(InputIterator first, InputIterator last, const Result &initial, Result (*fn)(const Result &, const Input &))
{
    Result value(initial);

    while (first != last)
        value = fn(value, *(first++));

    return value;
}

// General implementation for whole containers as input
template<class InputContainer, class Function, typename Result>
static inline Result inject(const InputContainer &input, const Result &initial, Function fn)
{ return inject(input.begin(), input.end(), initial, fn); }

// Template resolver for whole containers
template<class InputContainer, typename Result, typename Input = _cvt(InputContainer)>
static inline Result inject(const InputContainer &input, const Result &initial, Result (*fn)(const Result &, const Input &))
{ return inject(input.begin(), input.end(), initial, fn); }


// General implementation
template<class OutputContainer, class InputIterator, class Function>
static inline typename std::enable_if<helper::is_pushable<OutputContainer>::value, OutputContainer>::type map(InputIterator first, InputIterator last, Function fn)
{
    OutputContainer out;

    while (first != last)
        _stuff(out, fn(*(first++)));

    return out;
}

// Default for OutputContainer is std::vector; in this case, however, the OutputType has to be given
template<typename Output, class InputIterator, class Function>
static inline typename std::enable_if<!helper::is_pushable<Output>::value, std::vector<Output>>::type map(InputIterator first, InputIterator last, Function fn)
{ return map<std::vector<Output>>(first, last, fn); }

// User-defined output + template resolver
template<class OutputContainer, class InputIterator, typename Output = _cvt(OutputContainer), typename Input = _ivt(InputIterator)>
static inline typename std::enable_if<helper::is_pushable<OutputContainer>::value, OutputContainer>::type map(InputIterator first, InputIterator last, Output (*fn)(const Input &))
{
    OutputContainer out;

    while (first != last)
        _stuff(out, fn(*(first++)));

    return out;
}

// std::vector + template resolver
template<typename Output, class InputIterator, typename Input = _ivt(InputIterator)>
static inline typename std::enable_if<!helper::is_pushable<Output>::value, std::vector<Output>>::type map(InputIterator first, InputIterator last, Output (*fn)(const Input &))
{ return map<std::vector<Output>>(first, last, fn); }

// user-defined + general function + whole container
template<class OutputContainer, class InputContainer, class Function>
static inline typename std::enable_if<helper::is_pushable<OutputContainer>::value, OutputContainer>::type map(const InputContainer &input, Function fn)
{ return map<OutputContainer>(input.begin(), input.end(), fn); }

// std::vector + general function + whole container
template<typename Output, class InputContainer, class Function>
static inline typename std::enable_if<!helper::is_pushable<Output>::value, std::vector<Output>>::type map(const InputContainer &input, Function fn)
{ return map<std::vector<Output>>(input.begin(), input.end(), fn); }

// user-defined + template resolver + whole container
template<class OutputContainer, class InputContainer, typename Output = _cvt(OutputContainer), typename Input = _cvt(InputContainer)>
static inline typename std::enable_if<helper::is_pushable<OutputContainer>::value, OutputContainer>::type map(const InputContainer &input, Output (*fn)(const Input &))
{ return map<OutputContainer>(input.begin(), input.end(), fn); }

// std::vector + template resolver + whole container
template<typename Output, class InputContainer, typename Input = _cvt(InputContainer)>
static inline typename std::enable_if<!helper::is_pushable<Output>::value, std::vector<Output>>::type map(const InputContainer &input, Output (*fn)(const Input &))
{ return map<std::vector<Output>>(input.begin(), input.end(), fn); }

// And now try to deduce the output type through the InputIterator's value type (if no output container type is given)
template<class InputIterator, class Function, typename Output = _ivt(InputIterator)>
static inline std::vector<Output> map(InputIterator first, InputIterator last, Function fn)
{ return map<std::vector<Output>>(first, last, fn); }

template<class InputIterator, typename Output = _ivt(InputIterator), typename Input = _ivt(InputIterator)>
static inline std::vector<Output> map(InputIterator first, InputIterator last, Output (*fn)(const Input &))
{ return map<std::vector<Output>>(first, last, fn); }

template<class InputContainer, class Function, typename Output = _cvt(InputContainer)>
static inline std::vector<Output> map(const InputContainer &input, Function fn)
{ return map<std::vector<Output>>(input.begin(), input.end(), fn); }

template<class InputContainer, typename Output = _cvt(InputContainer), typename Input = _cvt(InputContainer)>
static inline std::vector<Output> map(const InputContainer &input, Output (*fn)(const Input &))
{ return map<std::vector<Output>>(input.begin(), input.end(), fn); }

}
}

#endif
