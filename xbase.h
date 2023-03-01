#ifndef XLIB_BASE
#define XLIB_BASE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* =========================================================================
   BASIC TYPES
   ========================================================================= */

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint32_t bool;
typedef float f32;
#define false 0
#define true 1

/* =========================================================================
   OTHER
   ========================================================================= */

#define global static
#define function static

#define PIf 3.141592653589793238462643383279502884197f

#define narray(a) (sizeof(a) / sizeof((a)[0]))

#endif