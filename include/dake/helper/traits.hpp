#ifndef DAKE__HELPER__TRAITS_HPP
#define DAKE__HELPER__TRAITS_HPP

#include <utility>

namespace dake
{
namespace helper
{

#define _def_member_test(name) \
    template<typename T> struct has_##name \
    { \
        template<typename U> static char (&_(...))[2]; \
        template<typename U> static char _(decltype(std::declval<U &>().name(std::declval<typename U::value_type>())) *); \
        static bool const value = sizeof(_<T>(nullptr)) == sizeof(char); \
    }

_def_member_test(push);
_def_member_test(push_back);
_def_member_test(insert);

template<typename T> struct is_pushable { static const bool value = has_push_back<T>::value || has_push<T>::value; };

#undef _def_member_test

}
}

#endif
