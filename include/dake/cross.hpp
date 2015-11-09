#ifndef DAKE__CROSS_HPP
#define DAKE__CROSS_HPP

#include <cstddef>
#include <cstdlib>


#ifdef __MINGW32__
extern "C"
{
    char *basename(char *path);
    int setenv(const char *name, const char *value, int overwrite);
}
#else
extern "C"
{
#include <libgen.h>
}

#include <cstdlib>
#endif

namespace dake
{
namespace cross
{

#ifdef __MINGW32__
void *aligned_alloc(size_t alignment, size_t size);
void aligned_free(void *ptr);
#else
static inline void *aligned_alloc(size_t alignment, size_t size)
{ return ::aligned_alloc(alignment, size); }

static inline void aligned_free(void *ptr)
{ return ::free(ptr); }
#endif

}
}

#endif
