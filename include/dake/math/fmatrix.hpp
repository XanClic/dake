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


// Every type should have (at least) the size it requires as alignment so that
// packed vectors work

// Needs to be aligned to eight bytes (TODO: Does it really? Not 16?)
static_assert(sizeof(fvec2) == 0x08, "sizeof(fvec2) != 0x08");

// Need to be aligned to 16 bytes
static_assert(sizeof(fvec2d) == 0x10, "sizeof(fvec2d) != 0x10");
static_assert(sizeof(fvec3)  == 0x10, "sizeof(fvec3) != 0x10");
static_assert(sizeof(fvec4)  == 0x10, "sizeof(fvec4) != 0x10");
static_assert(sizeof(fmat3)  == 0x30, "sizeof(fmat3) != 0x30");
static_assert(sizeof(fmat4)  == 0x40, "sizeof(fmat4) != 0x40");

#ifdef __AVX__
// Need to be aligned to 32 bytes
static_assert(sizeof(fvec3d) == 0x20, "sizeof(fvec3d) != 0x20");
static_assert(sizeof(fvec4d) == 0x20, "sizeof(fvec4d) != 0x20");
#else
// Need to be aligned to 16 bytes
static_assert(sizeof(fvec3d) == 0x20, "sizeof(fvec3d) != 0x20");
static_assert(sizeof(fvec4d) == 0x20, "sizeof(fvec4d) != 0x20");
#endif

}
}

#endif
