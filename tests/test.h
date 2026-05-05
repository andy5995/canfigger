#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <canfigger.h>

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a)[0])
