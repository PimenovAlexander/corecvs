#pragma once
/**
 * \file global.h
 * \brief This file holds the global includes, mostly for debuging, tracing and profiling
 *
 *
 * \ingroup cppcorefiles
 * \date Apr 13, 2009
 * \author Alexander Pimenov
 */

#define     CORE_MIN(a, b)              ((a) > (b) ? (b) : (a))
#define     CORE_MAX(a, b)              ((a) < (b) ? (b) : (a))

#define     CORE_ABS(x)                 ((x) > 0 ? (x) : -(x))

#define     CORE_COUNT_OF(arr)          (sizeof(arr) / sizeof((arr)[0]))

#define     CORE_CLEAR_MEMORY(pm, sz)   memset(pm, 0x00, sz)
#define     CORE_FILL_MEMORY( pm, sz)   memset(pm, 0xFF, sz)
#define     CORE_CLEAR_STRUCT(obj)      CORE_CLEAR_MEMORY(&(obj), sizeof(obj))

/* TODO: try to use std offsetof() while it's present. */
#define     CORE_OFFSET_OF(s, m)        offsetof(s, m)
//#define   CORE_OFFSET_OF(s, m)        (int)&(((s *)0)->m)

#define     CORE_UNUSED(arg)            (void)arg

#define     CORE_IS_POW2N(x)            (((x) & ((x) - 1)) == 0)

/** Define useful types
 */
typedef const int          cint;
typedef const unsigned int cuint;
typedef const char         cchar;
#ifndef OPENCL_KERNEL_CODE
typedef unsigned int       uint;
#endif
typedef int                bool_t;                          // fast Boolean type for safe usage on different platforms

#ifndef OPENCL_KERNEL_CODE
# ifdef     __cplusplus
#  define is__cplusplus                                     // it duplicates std "__cplusplus" except the OpenCL platform
# endif
#endif

#ifdef is__cplusplus
    extern "C" {
#endif

// Hooks for memory profiling and leaks search
#if defined(ASSERTS) || defined(TRACE)
#   include <stdlib.h>
#   include <stdio.h>
#endif

#ifdef WIN32
#   define strdup     _strdup
#   define strcasecmp _stricmp
#   ifdef _MSC_VER
#    define snprintf sprintf_s
#   endif
#endif

#ifdef PPC440
#   define printf xil_printf
#endif

#define DASSERT_FAIL(X)                                         \
    do {                                                        \
        printf("Assert at %s:%d - %s\n", __FILE__, __LINE__, X);\
        fflush(stdout);                                         \
        stackTraceHandler(0xFF);                                \
        exit (-2);                                              \
    } while (0)

#define DASSERT_FAIL_P(X)                                       \
    do {                                                        \
        printf("Assert at %s:%d - ", __FILE__, __LINE__);       \
        printf X;                                               \
        fflush(stdout);                                         \
        stackTraceHandler(0xFF);                                \
        exit (-2);                                              \
    } while (0)

#ifdef ASSERTS

# define ASSERT_FAIL(X)          DASSERT_FAIL(X)
# define ASSERT_FAIL_P(X)        DASSERT_FAIL_P(X)
# define ASSERT_TRUE(Y, X)       if (!(Y)) { ASSERT_FAIL(X);   }
# define ASSERT_TRUE_S(Y)        if (!(Y)) { ASSERT_FAIL(#Y);  }
# define ASSERT_FALSE(Y, X)      if (Y)    { ASSERT_FAIL(X);   }
# define ASSERT_FALSE_S(Y)       if (Y)    { ASSERT_FAIL(#Y);  }
# define ASSERT_TRUE_P(Y, X)     if (!(Y)) { ASSERT_FAIL_P(X); }
# define ASSERT_FALSE_P(Y, X)    if (Y)    { ASSERT_FAIL_P(X); }

# define ASSERT_DOUBLE_EQUAL_E(X,Y,Eps, Text) \
    ASSERT_TRUE((X) > ((Y) - Eps) && (X) < ((Y) + Eps), Text)

# define ASSERT_DOUBLE_EQUAL(X,Y, Text) \
    ASSERT_DOUBLE_EQUAL_E(X, Y, 1e-10, Text)

#else // ASSERTS

# define ASSERT_FAIL(X)
# define ASSERT_TRUE_S(Y)
# define ASSERT_TRUE(Y, X)
# define ASSERT_FALSE_S(Y)
# define ASSERT_FALSE(Y, X)
# define ASSERT_TRUE_P(Y, X)
# define ASSERT_FALSE_P(Y, X)
# define ASSERT_DOUBLE_EQUAL_E(X,Y,Eps, Text)
# define ASSERT_DOUBLE_EQUAL(X,Y,Text)

# ifdef DEBUG
#  define DASSERT_TRUE_P(Y, X)   if (!(Y)) { DASSERT_FAIL_P(X); }
# else
#  define DASSERT_TRUE_P(Y, X)
#  undef  DASSERT_FAIL
#  define DASSERT_FAIL(X)
#  undef  DASSERT_FAIL_P
#  define DASSERT_FAIL_P(X)
# endif

#endif // !ASSERTS


/* Static assert */
/*#if defined(__GNUC__)
#define STATIC_ASSERT(CONDITION, VALUE) static_assert((CONDITION), (#VALUE))
#elif defined( __clang__ )
#define STATIC_ASSERT(CONDITION, VALUE) static_assert((CONDITION), (#VALUE))
#else*/
#define STATIC_ASSERT(CONDITION, VALUE) typedef int foo_##VALUE[(CONDITION) ? 1 : -1]
/*#endif*/

#define SYNC_PRINT(X) \
do {                  \
    printf X;         \
    fflush(stdout);   \
} while (0)

/** TODO: Add flush*/
#ifdef TRACE
#   define DOTRACE(X) SYNC_PRINT(X)
#else
#   define DOTRACE(X)
#endif

#ifndef PROFILE_MEMORY
#   define dv_malloc        malloc
#   define dv_calloc        calloc
#   define dv_free          free
#else
#   define dv_malloc(X)     malloc_hook(X)
#   define dv_calloc(X,Y)   calloc_hook(X,Y)
#   define dv_free(X)       free_hook(X)
#endif

#ifndef OPENCL_KERNEL_CODE

void stackTraceHandler(int sig);
void setSegVHandler();

#endif // !OPENCL_KERNEL_CODE

#ifdef _MSC_VER
#   define ALIGN_DATA(X)    __declspec(align(X))
#else
#   define ALIGN_DATA(X)    __attribute__((aligned(X)))
#endif

#if defined(__GNUC__)
#   define FORCE_INLINE     __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#   define FORCE_INLINE     __forceinline
#else
#   define FORCE_INLINE     inline
#endif

/* Fixing problem with stack alignment on Windows XP with gcc 4.4 */
#if defined(WIN32) && !defined(_MSC_VER)
#   define ALIGN_STACK_SSE  __attribute__((force_align_arg_pointer))
#else
#   define ALIGN_STACK_SSE
#endif

#if !defined(_MSC_VER) && !defined(Q_OS_MAC) && !defined(OPENCL_KERNEL_CODE)
#   define __STDC_FORMAT_MACROS
#   include <inttypes.h>
#   undef __STDC_FORMAT_MACROS
#else
#   ifndef PRIu64
#   define PRIu64 "I64u"
#   endif
#   ifndef PRIi64
#   define PRIi64 "I64d"
#   endif
#endif

#define REFLECTION_IN_CORE

#ifdef is__cplusplus
    } //     extern "C"
#endif

#ifdef is__cplusplus

/** Useful safe functions for string formatting */
#include <stdarg.h>
#include <stdio.h>  // vsnprintf,  Linux: size_t

template <size_t size>
inline int snprintf2buf(char (&d)[size], cchar* fmt, ...)
{
    va_list  varList;
    va_start(varList, fmt);
    int iLen = vsnprintf((char*)d, size, fmt, varList);
    va_end(varList);
    ASSERT_TRUE_S(iLen < (int)size);
    return iLen;
}

/** Function for safe deleting objects and arrays */
#include <stdlib.h>

template<typename Type>
inline void delete_safe (Type * &ptr)
{
    delete ptr;
    ptr = NULL;
}

template<typename Type>
inline void deletearr_safe (Type * &ptr)
{
    delete[] ptr;
    ptr = NULL;
}

/** Compatibility with old STL library versions without data() method of the vector object */
#if defined(_MSC_VER) && _MSC_VER <= 1500
# define GET_VEC_DATA_PTR(vec)  &(vec[0])
#else
# define GET_VEC_DATA_PTR(vec)  vec.data()
#endif

#endif // is__cplusplus

/* EOF */
