#ifndef DAKE__MATH__FMATRIX_HPP__INSIDE
#error Do not include dake/math/fmatrix/fvec2d-standalone.hpp directly!
#endif


static inline fvec2d operator*(double lhs, const fvec2d &rhs)
{
    return rhs * lhs;
}
