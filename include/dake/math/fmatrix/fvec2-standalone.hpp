#ifndef DAKE__MATH__FMATRIX_HPP__INSIDE
#error Do not include dake/math/fmatrix/fvec2-standalone.hpp directly!
#endif


static inline fvec2 operator*(float lhs, const fvec2 &rhs)
{
    return rhs * lhs;
}
