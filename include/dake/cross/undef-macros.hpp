#ifndef DAKE__CROSS__UNDEF_MACROS_HPP
#define DAKE__CROSS__UNDEF_MACROS_HPP

// Undefine badly named system macros; this header is *not* included by
// dake/cross.hpp, because you may actually not want it. Always include it
// *last* and do not include it in another header unless you know exactly what
// you are doing!

#ifdef DIFFERENCE
#undef DIFFERENCE
#endif

#ifdef Success
#undef Success
#endif

#ifdef near
#undef near
#endif

#ifdef NEAR
#undef NEAR
#endif

#ifdef far
#undef far
#endif

#ifdef FAR
#undef FAR
#endif

#ifdef FALSE
#undef FALSE
#endif

#ifdef TRUE
#undef TRUE
#endif

#ifdef BOOL
#undef BOOL
#endif

#ifdef IN
#undef IN
#endif

#ifdef OUT
#undef OUT
#endif

#ifdef OPTIONAL
#undef OPTIONAL
#endif

#ifdef CALLBACK
#undef CALLBACK
#endif

#ifdef CONST
#undef CONST
#endif

// While the definition of these two is generally sane, applications generally
// should not assume them to be defined already, so it's better to undefine them
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#endif
