#ifndef DAKE__MATH__FMATRIX_HPP__INSIDE
#error Do not include dake/math/fmatrix/fvec4-standalone.hpp directly!
#endif


static inline fvec4 operator*(float lhs, const fvec4 &rhs)
{
    return rhs * lhs;
}


inline fvec4 fvec4::direction(const fvec3 &ov)
{
    return fvec4(ov.v[0], ov.v[1], ov.v[2], 0.f);
}

inline fvec4 fvec4::position(const fvec3 &ov)
{
    return fvec4(ov.v[0], ov.v[1], ov.v[2], 1.f);
}
