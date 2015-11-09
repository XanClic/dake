#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef __MINGW32__

int setenv(const char *name, const char *value, int overwrite)
{
    if (!name || name[0] == 0 || strchr(name, '=')) {
        errno = EINVAL;
        return -1;
    }

    if (!overwrite && getenv(name)) {
        return 0;
    }

    if (!value) {
        value = "";
    }

    char *buf;
    asprintf(&buf, "%s=%s", name, value);
    return putenv(buf) ? -1 : 0;
}

#endif
