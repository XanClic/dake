#ifndef DAKE__MATH__FMATRIX_HPP__INSIDE
#error Do not include dake/math/fmatrix/fvec3-standalone.hpp directly!
#endif


static inline fvec3 operator*(float lhs, const fvec3 &rhs)
{
    return rhs * lhs;
}
