#ifndef DAKE__MATH__FMATRIX_HPP__INSIDE
#error Do not include dake/math/fmatrix/fvec4d-standalone.hpp directly!
#endif


static inline fvec4d operator*(double lhs, const fvec4d &rhs)
{
    return rhs * lhs;
}


inline fvec4d fvec4d::direction(const fvec3d &ov)
{
    return fvec4d(ov.x(), ov.y(), ov.z(), 0.);
}

inline fvec4d fvec4d::position(const fvec3d &ov)
{
    return fvec4d(ov.x(), ov.y(), ov.z(), 1.);
}
