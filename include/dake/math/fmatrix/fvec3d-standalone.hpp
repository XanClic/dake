#ifndef DAKE__MATH__FMATRIX_HPP__INSIDE
#error Do not include dake/math/fmatrix/fvec3d-standalone.hpp directly!
#endif


static inline fvec3d operator*(double lhs, const fvec3d &rhs)
{
    return rhs * lhs;
}
