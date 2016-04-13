#pragma once
/**
 * \file atomicOps.h
 *
 * \date Sep 27, 2012
 **/

#if 0   // disable for a while using std atomic type and its opers

#include <atomic>
using std::atomic_int;

inline int atomic_dec_and_fetch(atomic_int *ptr)
{
    return --(*ptr);
}

inline int atomic_inc_and_fetch(atomic_int *ptr)
{
    return ++(*ptr);
}

#else

//namespace corecvs {

typedef int atomic_int;


#if defined (__GNUC__)

    inline int atomic_dec_and_fetch(atomic_int *ptr)
    {
	    return __sync_sub_and_fetch (ptr, 1);
    }

    inline int atomic_inc_and_fetch(atomic_int *ptr)
    {
	    return __sync_add_and_fetch (ptr, 1);
    }

#elif defined(_MSC_VER)

#   include <stdio.h>
#   include <windows.h>
#   undef max
#   undef min
#   undef small
#   undef DIFFERENCE

    STATIC_ASSERT(sizeof(int) == sizeof(LONG), wrongUsageOfAtomicOperationsOnMSVC);

    inline int atomic_dec_and_fetch(atomic_int *ptr)
    {
        if ((int)ptr & 3) {
            printf("Stop: unsupported pointer value %p in the function atomic_dec_and_fetch!\n", ptr);
            exit(-1);
        }
        return (int)InterlockedDecrement((LONG *)ptr);
    }

    inline int atomic_inc_and_fetch(atomic_int *ptr)
    {
        if ((int)ptr & 3) {
            printf("Stop: unsupported pointer value %p in the function atomic_inc_and_fetch!\n", ptr);
            exit(-1);
        }
        return (int)InterlockedIncrement((LONG *)ptr);
    }

#else // _MSC_VER

#   warning ("Compiling without atomic support, your code could crash")

    inline int atomic_dec_and_fetch(atomic_int *ptr)
    {
	    return --(*ptr);
    }

    inline int atomic_inc_and_fetch(atomic_int *ptr)
    {
	    return ++(*ptr);
    }

#endif // !_MSC_VER && !__GNUC__

//} //namespace corecvs

#endif
