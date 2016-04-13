#pragma once
/**
 * \file global.h
 * \brief This file holds the global includes, mostly for debuging, tracing, profiling and portability
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

// Hooks for memory profiling and leaks search
#if defined(ASSERTS) || defined(TRACE)
#   include <stdlib.h>
#   include <stdio.h>
#endif

#ifdef WIN32
#   define strdup       _strdup
#   define strcasecmp   _stricmp
#   ifdef _MSC_VER
#    define snprintf    sprintf_s
#    define putenv      _putenv
#   endif
#endif

#ifdef PPC440
#   define printf xil_printf
#endif

#ifdef GTEST_INCLUDE_GTEST_GTEST_H_
    // gtest.h is included - the test is being compiled. We should activate our
    //                       internal assertion mechanism if it was switched off!
# ifndef  ASSERTS
#  define ASSERTS
# endif
#endif

#ifdef is__cplusplus
# ifdef ASSERTS

#   include <stdexcept>
    /** This structure need to issue the exception, which nobody catches
     *  that allow gtest to continue tests execution,
     *  but for the application it dues to stop unless we catch this exception.
     */
    struct AssertException : public std::runtime_error
    {
        AssertException(const char* codeExpr) : std::runtime_error(codeExpr) {}
    };

#  define RAISE_ASSERT(text)    throw AssertException(text);
# else
#  define RAISE_ASSERT(text)
# endif
#else
# define  RAISE_ASSERT(text)    exit(-2)
#endif

#define CORE_DASSERT_FAIL(X)                                    \
    do {                                                        \
        printf("Assert at %s:%d - %s\n", __FILE__, __LINE__, X);\
        fflush(stdout);                                         \
        stackTraceHandler(0xFF);                                \
        RAISE_ASSERT(X);                                        \
    } while (0)

#define CORE_DASSERT_FAIL_P(X)                                  \
    do {                                                        \
        printf("Assert at %s:%d - ", __FILE__, __LINE__);       \
        printf X;                                               \
        fflush(stdout);                                         \
        stackTraceHandler(0xFF);                                \
        RAISE_ASSERT(#X);                                       \
    } while (0)


#ifdef ASSERTS

# define CORE_ASSERT_FAIL(X)        CORE_DASSERT_FAIL(X)
# define CORE_ASSERT_FAIL_P(X)      CORE_DASSERT_FAIL_P(X)
# define CORE_ASSERT_TRUE_S(Y)      if (!(Y)) { CORE_ASSERT_FAIL(#Y);  }
# define CORE_ASSERT_TRUE_P(Y, X)   if (!(Y)) { CORE_ASSERT_FAIL_P(X); }
# define CORE_ASSERT_FALSE_S(Y)     if (Y)    { CORE_ASSERT_FAIL(#Y);  }
# define CORE_ASSERT_FALSE_P(Y, X)  if (Y)    { CORE_ASSERT_FAIL_P(X); }

// these both macros without prefix "CORE_" are conflicting with gtest's ones
# define CORE_ASSERT_TRUE(Y, X)     if (!(Y)) { CORE_ASSERT_FAIL(X);   }
# define CORE_ASSERT_FALSE(Y, X)    if (Y)    { CORE_ASSERT_FAIL(X);   }

# define CORE_ASSERT_EQ_P(X, Y, Text) if (X != Y) { CORE_ASSERT_FAIL_P(Text); }

# define CORE_ASSERT_DOUBLE_EQUAL_E(X,Y,Eps, Text) \
         CORE_ASSERT_TRUE((X) > ((Y) - Eps) && (X) < ((Y) + Eps), Text)

# define CORE_ASSERT_DOUBLE_EQUAL_EP(X,Y,Eps, Text) \
         CORE_ASSERT_TRUE_P((X) > ((Y) - Eps) && (X) < ((Y) + Eps), Text)

# define CORE_ASSERT_DOUBLE_EQUAL(X,Y, Text) \
         CORE_ASSERT_DOUBLE_EQUAL_E(X, Y, 1e-10, Text)

# define CORE_ASSERT_DOUBLE_EQUAL_P(X,Y, Text) \
         CORE_ASSERT_DOUBLE_EQUAL_EP(X, Y, 1e-10, Text)

# ifdef GTEST_INCLUDE_GTEST_GTEST_H_
//#  undef  CORE_DASSERT_FAIL
//#  define CORE_DASSERT_FAIL(X)    GTEST_NONFATAL_FAILURE_(X)
//#  undef  CORE_DASSERT_FAIL_P
//#  define CORE_DASSERT_FAIL_P(X)  do { SYNC_PRINT(X); GTEST_NONFATAL_FAILURE_("xxx"); } while (0)
# endif

#else // ASSERTS

# if !defined(DEBUG) || defined(NDEBUG)
#  undef  CORE_DASSERT_FAIL
#  define CORE_DASSERT_FAIL(X)
#  undef  CORE_DASSERT_FAIL_P
#  define CORE_DASSERT_FAIL_P(X)
# endif // !DEBUG

# define CORE_ASSERT_FAIL(X)
# define CORE_ASSERT_FAIL_P(X)
# define CORE_ASSERT_TRUE_S(Y)
# define CORE_ASSERT_TRUE_P(Y, X)
# define CORE_ASSERT_FALSE_S(Y)
# define CORE_ASSERT_FALSE_P(Y, X)

# define CORE_ASSERT_TRUE(Y, X)
# define CORE_ASSERT_FALSE(Y, X)

# define CORE_ASSERT_DOUBLE_EQUAL_E( X,Y,Eps, Text)
# define CORE_ASSERT_DOUBLE_EQUAL_EP(X,Y,Eps, Text)
# define CORE_ASSERT_DOUBLE_EQUAL(   X,Y, Text)
# define CORE_ASSERT_DOUBLE_EQUAL_P( X,Y, Text)

#endif // !ASSERTS


/* Static assert */
/*#if defined(__GNUC__)
#define STATIC_ASSERT(CONDITION, VALUE) static_assert((CONDITION), (#VALUE))
#elif defined( __clang__ )
#define STATIC_ASSERT(CONDITION, VALUE) static_assert((CONDITION), (#VALUE))
#else
#define STATIC_ASSERT(CONDITION, VALUE) typedef int foo_##VALUE[(CONDITION) ? 1 : -1] */
/*#endif*/

#if defined(__GNUC__)
   #define STATIC_ASSERT(CONDITION, VALUE)                         \
     _Pragma("GCC diagnostic push")                                \
     _Pragma("GCC diagnostic ignored \"-Wunused-local-typedefs\"") \
     __attribute__((unused)) typedef int foo_##VALUE[(CONDITION) ? 1 : -1];                \
     _Pragma("GCC diagnostic pop")

#else
   #define STATIC_ASSERT(CONDITION, VALUE) typedef int foo_##VALUE[(CONDITION) ? 1 : -1]
#endif

#define SYNC_PRINT(X) \
do {                  \
    printf X;         \
    fflush(stdout);   \
} while (0)

#ifdef TRACE
#   define DOTRACE(X)       SYNC_PRINT(X)
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

#ifdef is__cplusplus
extern "C" {
#endif
    void stackTraceHandler(int sig);
    void setSegVHandler();
#ifdef is__cplusplus
} // extern "C"
#endif

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
#endif

#ifndef  PRIu64
# define PRIu64 "I64u"
#endif
#ifndef  PRIi64
# define PRIi64 "I64d"
#endif

#define REFLECTION_IN_CORE


/** Useful macros for best compatibility with using development environments
 */
#if defined(WIN32) && _MSC_VER > 1200  // VS > 6.0
# define USE_SAFE_RUNTIME_FUNCS                     // comment this to use old RT-funcs for VS > 6.0
#else
# undef  USE_SAFE_RUNTIME_FUNCS                     // non Windows platforms or old VS compiler
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
#ifdef USE_SAFE_RUNTIME_FUNCS
    int iLen = vsnprintf_s((char*)d, size, size - 1, fmt, varList);
#else
    int iLen = vsnprintf((char*)d, size, fmt, varList);
#endif
    va_end(varList);
    CORE_ASSERT_TRUE_S(iLen < (int)size);
    return iLen;
}

#include <type_traits>
#include <algorithm>
#include <tuple>
namespace std
{
#if 0
    template<typename T>
    struct hash
    {
        using boo = typename std::enable_if<std::is_enum<T>::value, T>::type;
        size_t operator() (const boo &t) const
        {
            return hash<typename underlying_type<boo>::type>()(static_cast<typename underlying_type<boo>::type>(t));
        }
    };
#endif
    template<typename U, typename V>
    struct hash<pair<U, V>>
    {
        std::size_t operator() (const pair<U, V> &p) const
        {
            return std::hash<U>()(p.first) ^ std::hash<V>()(p.second);
        }
    };
    // Numbers were stolen from boost (actually it very similar to LFG ideas)
    template<typename T>
    size_t hash_append(size_t seed, const T &t)
    {
        return std::hash<T>()(t) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    template<typename T, size_t n = tuple_size<T>::value - 1>
    struct hash_calc
    {
        size_t operator() (const T &t, size_t seed) const
        {
            auto foo = hash_calc<T, n - 1>()(t, seed);
            return hash_append(foo, std::get<n>(t));
        }
    };
    template<typename T>
    struct hash_calc<T, 0>
    {
        size_t operator() (const T &t, size_t seed) const
        {
            return hash_append(seed, std::get<0>(t));
        }
    };
    template<typename ... T>
    struct hash<std::tuple<T...>>
    {
        size_t operator() (const std::tuple<T...> &t) const
        {
            return hash_calc<std::tuple<T...>>()(t, 0);
        }
   };
};


#ifdef USE_SAFE_RUNTIME_FUNCS
# define CORE_FOPEN(pFile, sName, sMode)    (fopen_s(&(pFile), (sName), (sMode)) ? ((pFile) = NULL, 0) : (1))
#else
# define CORE_FOPEN(pFile, sName, sMode)    (((pFile) = fopen((sName), (sMode))) != NULL ? (1) : (0))
#endif

/** MSVC: macro to help in memory leaks detection */
#if defined(_MSC_VER) && defined(_DEBUG)
# define USE_MSVC_DEBUG_MEM
# define _CRTDBG_MAP_ALLOC
# define _CRTDBG_MAP_ALLOC_NEW
# include <stdlib.h>
# include <crtdbg.h>
// It's strange that msvc's inlines for new/delete don't work properly...
//#  define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) // this doesn't work for new inplace :(
//#  define new DBG_NEW
inline void * __CRTDECL operator new[](size_t _Size) {
    return ::operator new[](_Size, _NORMAL_BLOCK, __FILE__, __LINE__);
}
inline void * __CRTDECL operator new(size_t _Size) {
    return ::operator new(_Size, _NORMAL_BLOCK, __FILE__, __LINE__);
}
//template<typename T> inline void * operator new (T) {
//    void * p = ::operator new(sizeof(T), _NORMAL_BLOCK, __FILE__, __LINE__);
//    return new (p)T;
//}
//inline void * __CRTDECL operator new(size_t _Size, void *p) {
//    return p;
//}
#endif

/** Function for safe deleting objects and arrays */
#include <stdlib.h>

template<typename Type>
inline void delete_safe (Type * &ptr)
{
    delete ptr;
  //ptr = (Type *)(uintptr_t(NULL) - 1);        /* We are not hiding our mistakes by zeroing the pointer */
    ptr = NULL;
}

template<typename Type>
inline void deletearr_safe (Type * &ptr)
{
    delete[] ptr;
  //ptr = (Type *)(uintptr_t(NULL) - 1);        /* We are not hiding our mistakes by zeroing the pointer */
    ptr = NULL;
}

/** Compatibility with old STL library versions without data() method of the vector object */
#if defined(_MSC_VER) && _MSC_VER <= 1500
# define VEC_DATA_PTR(vec)      &(vec[0])
#else
# define VEC_DATA_PTR(vec)      vec.data()
#endif

#ifdef _WIN32
#   define PATH_SEPARATOR "\\"
#else
#   define PATH_SEPARATOR  "/"
#endif

#if defined(_WIN32) && defined(_MSC_VER) && _MSC_VER < 1800
#   define FOREACH(X, Y) for each (X in Y)      // msvc understands standard since vc12
#else
#   define FOREACH(X, Y) for (X : Y)
#endif

//#define QSTR_DATA_PTR(qstring)        (qstring).toLatin1().data()
#define   QSTR_DATA_PTR(qstring)        (qstring).toStdString().c_str()  // after using textCodecs we should use this

#define   QSTR_HAS_SLASH_AT_END(qstring)  ((qstring).length() > 1 && \
                                          ((qstring)[(qstring).length() - 1] == '/' || \
                                           (qstring)[(qstring).length() - 1] == '\\'))

#define   STR_HAS_SLASH_AT_END(string)  ((string).length() > 1 && \
                                        ((string)[(string).length() - 1] == '/' || \
                                         (string)[(string).length() - 1] == '\\'))

#endif // is__cplusplus

/* EOF */
