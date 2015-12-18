#ifndef GENERICMATH_H_
#define GENERICMATH_H_

#include <stdint.h>
#include "global.h"

/*
 * genericMath.h
 *
 *  Created on: Sep 12, 2012
 *      Author: alexander
 */


namespace corecvs
{

/**
 *   Some functions are not expressed with binary operators, so we need to add functions as
 *   static functions.
 *
 **/
template<typename Type>
class GenericMath
{
public:
template <int divisor, class DividableType>
    static DividableType div(const DividableType &val)
    {
        return DividableType(val / divisor);
    }

template <int multiplier>
    static Type mul(const Type &val)
    {
        return Type(val * multiplier);
    }

/* Cleanup the design for larger data types */
    static inline Type branchlessMask(const Type &val);

    static inline Type selector(const Type &condition, const Type &ifTrue, const Type &ifFalse)
    {
        return condition ? ifTrue : ifFalse;
    }

    template<class InputType>
    inline static InputType max (const InputType &left, const InputType &right)
    {
        return (left > right) ? left : right;
    }

    template<class InputType>
    inline static InputType min (const InputType &left, const InputType &right)
    {
        return (left > right) ? right : left;
    }


    inline static Type difference (const Type &left, const Type &right) {
    	if (left > right) return left - right;
    	return right - left;
    }

    inline static Type difference15bit (const Type &left, const Type &right) {
    	return difference(left, right);
    }

    template<class InputType>
    inline static InputType abs (const InputType &value) {
    	return max(value, InputType(-value));
    }

    /* Some logical functions that SSE does in one instruction */
    inline static Type andNot(const Type &left, const Type &right) {
        return (left & (~right));
    }

    template<class InputType>
    inline static void fillOnes(InputType &var)
    {
        var = 0;
        var = ~var;
    }
};

template<>
uint16_t inline GenericMath<uint16_t>::branchlessMask(const uint16_t &val)
{
    return -(val);
}

template<>
int16_t inline GenericMath<int16_t>::branchlessMask(const int16_t &val)
{
    return -(val);
}

} /* namespace corecvs */
#endif /* GENERICMATH_H_ */
