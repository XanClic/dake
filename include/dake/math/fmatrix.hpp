#ifndef DAKE__MATH__FMATRIX_HPP
#define DAKE__MATH__FMATRIX_HPP

#include <cmath>

#include "dake/math/matrix.hpp"

#ifndef __SSE__
#error SSE support is required for dake/math/fmatrix.hpp!
#endif


namespace dake
{
namespace math
{

static inline float apr_rcp(float x)
{
    float r;
    __asm__ ("rcpss %1,%0" : "=x"(r) : "x"(x));
    return r;
}

static inline float apr_rsqrt(float x)
{
    float r;
    __asm__ ("rsqrtss %1,%0" : "=x"(r) : "x"(x));
    return r;
}


class fvec4;
class fvec4d;
class fvec3;
class fvec3d;
class fvec2;
class fvec2d;


#define DAKE__MATH__FMATRIX_HPP__INSIDE


#include "dake/math/fmatrix/fvec4.hpp"

#ifdef __AVX__
#include "dake/math/fmatrix/fvec4d-avx.hpp"
#else
#include "dake/math/fmatrix/fvec4d-noavx.hpp"
#endif


#include "dake/math/fmatrix/fvec3.hpp"

#ifdef __AVX__
#include "dake/math/fmatrix/fvec3d-avx.hpp"
#else
#include "dake/math/fmatrix/fvec3d-noavx.hpp"
#endif


#include "dake/math/fmatrix/fvec2.hpp"
#include "dake/math/fmatrix/fvec2d.hpp"


#include "dake/math/fmatrix/fvec4-standalone.hpp"
#include "dake/math/fmatrix/fvec4d-standalone.hpp"
#include "dake/math/fmatrix/fvec3-standalone.hpp"
#include "dake/math/fmatrix/fvec3d-standalone.hpp"
#include "dake/math/fmatrix/fvec2-standalone.hpp"
#include "dake/math/fmatrix/fvec2d-standalone.hpp"


#include "dake/math/fmatrix/fmat4.hpp"
#include "dake/math/fmatrix/fmat3.hpp"


#undef DAKE__MATH__FMATRIX_HPP__INSIDE

}
}

#endif
