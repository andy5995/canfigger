#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "canfigger.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a)[0])
