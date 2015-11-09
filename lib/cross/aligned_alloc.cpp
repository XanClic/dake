#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <dake/cross.hpp>


namespace dake
{
namespace cross
{

#ifdef __MINGW32__

struct Description {
    void *base;
};


void *aligned_alloc(size_t alignment, size_t size)
{
    void *base = malloc(size + alignment + sizeof(Description) - 1);

    uintptr_t min_ptr = reinterpret_cast<uintptr_t>(base) + sizeof(Description);
    uintptr_t ptr = (min_ptr + alignment - 1) & -alignment;

    void *vptr = reinterpret_cast<void *>(ptr);

    Description *desc = static_cast<Description *>(vptr);
    desc[-1].base = base;

    return vptr;
}


void aligned_free(void *ptr)
{
    if (!ptr) {
        return;
    }

    Description *desc = static_cast<Description *>(ptr);
    free(desc[-1].base);
}

#endif

}
}
