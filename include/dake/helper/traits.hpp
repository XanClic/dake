#ifndef DAKE__HELPER__TRAITS_HPP
#define DAKE__HELPER__TRAITS_HPP

#include <deque>
#include <list>
#include <queue>
#include <stack>
#include <vector>


namespace dake
{
namespace helper
{

template<typename T> struct has_push_back { static const bool value = false; };
template<typename T, typename Alloc> struct has_push_back<std::vector<T, Alloc>> { static const bool value = true; };
template<typename T, typename Alloc> struct has_push_back<std::list  <T, Alloc>> { static const bool value = true; };
template<typename T, typename Alloc> struct has_push_back<std::deque <T, Alloc>> { static const bool value = true; };

template<typename T> struct has_push { static const bool value = false; };
template<typename T, typename Alloc> struct has_push<std::queue<T, Alloc>> { static const bool value = true; };
template<typename T, typename Alloc> struct has_push<std::stack<T, Alloc>> { static const bool value = true; };

template<typename T> struct is_pushable { static const bool value = has_push_back<T>::value || has_push<T>::value; };

}
}

#endif
