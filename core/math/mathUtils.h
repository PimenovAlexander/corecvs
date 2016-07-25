#pragma once
/**
 * \file mathUtils.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Mar 4, 2010
 * \author alexander
 */
#include "global.h"

#ifndef M_PI
#define _USE_MATH_DEFINES
#include <math.h>
#undef  _USE_MATH_DEFINES
#endif

#include <stdlib.h>       // RAND_MAX

namespace corecvs {

/** Useful round* stuff
 */

/**
 *   Unfortunately round is not supported by VSMS compiler.
 *   So we use fround instead
 **/
inline int fround (double value)
{
    return (int)floor(value + 0.5);         // be careful: it's right only for positive values
}

inline int roundUp (double dValue)
{
    return (int)(dValue + .5);
}

inline int roundSign (double dValue, bool bSign)
{
    return (int)(dValue + (bSign ? .5 : -.5));
}

inline int roundSign (double dValue, int iSign)
{
    return roundSign(dValue, iSign >= 0);
}

inline int roundSign (double dValue, double dSign)
{
    return roundSign(dValue, dSign >= 0);
}

inline int roundSign (double dValue)
{
    return roundSign(dValue, dValue);
}

/** Integer division with rounding to the upper integer that works for non-negative integers only
 */
inline unsigned int ceilDiv(unsigned int iNum, unsigned int iDenom)
{
    return (iNum + iDenom - 1) / iDenom;
}

/** This macro is a copy of roundDivUp() function,
 *  but it works even during a compilation, e.g. for usage in STATIC_ASSERT()
 */
#define _roundDivUp(iNum, iDenom) (((iNum) + ((iDenom) >> 1)) / (iDenom))

inline int roundDivUp (int nom, unsigned int nDenom)
{
    return _roundDivUp(nom, nDenom);
}

inline int roundDiv (int nom, unsigned int nDenom)
{
    return (nom + (nom >= 0 ? (nDenom >> 1) : -(int)(nDenom >> 1))) / nDenom;
}

inline int noroundShiftUp (int iVal, unsigned int nShift)
{
    return iVal >> nShift;
}

inline int roundShiftUp (int iVal, unsigned int nShift)
{
    return (iVal + (1 << (nShift - 1))) >> nShift;
}

inline int roundShift (int iVal, unsigned int nShift)
{
    return iVal >= 0 ? roundShiftUp(iVal, nShift) : -roundShiftUp(-iVal, nShift);
}

inline int noroundShift (int iVal, unsigned int nShift)
{
    return iVal >= 0 ? (iVal >> nShift) : -(-iVal >> nShift);
}

/** Useful Swap* stuff
 */
template <typename Type>
inline void SwapXY(Type& a, Type& b) { Type t = a; a = b; b = t; }

/**
 * On intel architecture there could be no gain in xor because
 * often compiler can swap register aliases without any code generation
 * This has been tested and proved 3.22 times slower than SwapXY on Intel machine.
 **/
template <typename Type>
inline void SwapIntsSlow(Type& a, Type& b) { a ^= b; b ^= a; a ^= b; }

/**
 *  Function to get a random value in the given range
 *
 *  TODO: This is terrible in fact. We should use Merssen Twister here, rand() is poor and not fun.
 */
template<typename Type>
inline Type randRanged(Type valMax, Type valMin = Type(0))
{
    return (Type)(valMin + ((double)rand() / RAND_MAX * (valMax - valMin)));   // no round for int to fit into [min, max] range
}

/**
 * Useful function that finds nearest value equals to power of 2 that is not less of the given value
 **/
inline unsigned getNearUpperPowerOf2(unsigned value)
{
    if (CORE_IS_POW2N(value))
        return value;

    value |= value >>  1;
    value |= value >>  2;
    value |= value >>  4;
    value |= value >>  8;
    value |= value >> 16;
    return value ? value + 1 : 0;
}

inline unsigned getNearUpperPowerOf2(unsigned value, unsigned &power)
{
    power = 0;
    value = getNearUpperPowerOf2(value);
    if (value) {
        while ((value & (1 << power)) == 0) power++;
    }
    return value;
}

/** Useful radian to degrees and back conversions
 */
inline double radToDeg (double angle)
{
    return angle / M_PI * 180.0;
}

inline double degToRad (double angle)
{
    return angle / 180.0 * M_PI;
}

template<class TypeName>
TypeName lerp(TypeName x1, TypeName x2, double alpha)
{
    return x1 * (1.0 - alpha) + x2 * alpha;
}

template<class TypeName>
TypeName lerp(TypeName x1, TypeName x2, double value, double intervalStart, double intervalEnd)
{
    return lerp<TypeName>(x1, x2, (value - intervalStart) / (intervalEnd - intervalStart));
}

inline double lerpLimit(double outStart, double outEnd, double value, double intervalStart, double intervalEnd)
{
    if (value <= intervalStart) return outStart;
    if (value >= intervalEnd)   return outEnd;
    return lerp<double>(outStart, outEnd, value, intervalStart, intervalEnd);
}

/** Common functions **/

/**
 *  Probability density function for one-dimention normal distribution
 *
 *  \f[
 *       f(x;\mu,\sigma^2) = \frac{1}{\sigma\sqrt{2\pi}} e^{ -\frac{1}{2}\left(\frac{x-\mu}{\sigma}\right)^2 }.
 *  \f]
 *
 **/
inline double normalPDF(double x, double sigma)
{
    return exp(-x * x / (2.0 * sigma * sigma)) / (sigma * sqrt(2.0 * M_PI));
}


} //namespace corecvs
