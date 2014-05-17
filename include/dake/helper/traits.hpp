#ifndef DAKE__HELPER__TRAITS_HPP
#define DAKE__HELPER__TRAITS_HPP

#include <utility>

namespace dake
{
namespace helper
{

#define _def_member_test(name, funcall) \
    template<typename T> struct has_##name \
    { \
        template<typename U> static char (&_(...))[2]; \
        template<typename U> static char _(decltype(std::declval<U &>().funcall) *); \
        static bool const value = sizeof(_<T>(nullptr)) == sizeof(char); \
    }

_def_member_test(push,      push     (std::declval<typename U::value_type>()));
_def_member_test(push_back, push_back(std::declval<typename U::value_type>()));
_def_member_test(insert,    insert   (std::declval<typename U::value_type>()));

_def_member_test(begin, begin());
_def_member_test(end,   end  ());

template<typename T> struct is_pushable { static const bool value = has_push_back<T>::value || has_push<T>::value; };
template<typename T> struct is_iterable { static const bool value = has_begin<T>::value && has_end<T>::value; };

#undef _def_member_test

}
}

#endif
