#ifndef DAKE__HELPER__FUNCTION_HPP
#define DAKE__HELPER__FUNCTION_HPP

#include <utility>


namespace dake
{
namespace helper
{

#define _generate_helper(name, nf_code, f_code) \
    template<typename U, typename V> auto name(const U &x, const V &y) -> decltype(nf_code) { return nf_code; } \
    template<typename U, typename V> auto name(U &&x, V &&y) -> decltype(f_code) { return f_code; }

_generate_helper(sum,                   x + y, std::forward(x) + std::forward(y));
_generate_helper(difference,            x - y, std::forward(x) - std::forward(y));
_generate_helper(product,               x * y, std::forward(x) * std::forward(y));
_generate_helper(quotient,              x / y, std::forward(x) / std::forward(y));
_generate_helper(conjunction,           x & y, std::forward(x) & std::forward(y));
_generate_helper(disjunction,           x | y, std::forward(x) | std::forward(y));
_generate_helper(exclusive_disjunction, x ^ y, std::forward(x) ^ std::forward(y));
_generate_helper(maximum,               (x > y) ? x : y, (std::forward(x) > std::forward(y)) ? std::forward(x) : std::forward(y));
_generate_helper(minimum,               (x < y) ? x : y, (std::forward(x) < std::forward(y)) ? std::forward(x) : std::forward(y));

#undef _generate_helper

}
}

#endif
