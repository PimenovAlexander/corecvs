#ifndef VECTOR_TRAITS_H
#define VECTOR_TRAITS_H

#include <stdint.h>

#include "global.h"

#include "sseWrapper.h"
#include "vectorAlgebra.h"

namespace corecvs {

template<class _Type>
class TraitGeneric
{
public:
    typedef TraitGeneric FallbackTraits;

    typedef _Type Type;
    typedef _Type SignedType;
    typedef _Type ExtendedType;

    typedef _Type InternalType;
    static const int step = 1;
};

class TraitG12Buffer {
public:
    typedef TraitG12Buffer FallbackTraits;

    typedef uint16_t Type;
    typedef int16_t SignedType;
    typedef uint32_t ExtendedType;

    typedef Type InternalType;
    static const int step = 1;
};

class TraitG8Buffer {
public:
    typedef TraitG8Buffer FallbackTraits;

    typedef uint8_t Type;
    typedef int8_t SignedType;
    typedef uint16_t ExtendedType;

    typedef Type InternalType;
    static const int step = 1;
};

#ifdef WITH_SSE
class TraitG12BufferVector {
public:
    typedef TraitG12Buffer FallbackTraits;

    typedef FallbackTraits::Type InternalType;
    static const int step = 8;

    typedef UInt16x8 Type;
    typedef Int16x8 SignedType;
    typedef Int32x8v ExtendedType;
};

/*
class TraitG8BufferVector {
public:
    typedef TraitG8Buffer FallbackTraits;

    typedef FallbackTraits::Type InternalType;
    static const int step = 8;

    typedef UInt8x16 Type;
    typedef Int8x16 SignedType;
    typedef Int16x8 ExtendedType;
};
*/
#endif

#if defined ( WITH_AVX )
class TraitDoubleBufferVector {
public:
    typedef TraitGeneric<double> FallbackTraits;

    typedef TraitGeneric<double>::Type InternalType;
    static const int step = Doublex4::SIZE;

    typedef Doublex4 Type;
    typedef Doublex4 SignedType;
    typedef Doublex4 ExtendedType;
};

class TraitDoubleBufferVector8 {
public:
    typedef TraitGeneric<double> FallbackTraits;

    typedef TraitGeneric<double>::Type InternalType;
    static const int step = Doublex8::SIZE;

    typedef Doublex8 Type;
    typedef Doublex8 SignedType;
    typedef Doublex8 ExtendedType;
};

class TraitDoubleBufferVector4x5 {
public:
    typedef TraitGeneric<double> FallbackTraits;

    typedef TraitGeneric<double>::Type InternalType;
    static const int step = DoublexT4<3>::SIZE;

    typedef DoublexT4<3> Type;
    typedef DoublexT4<3> SignedType;
    typedef DoublexT4<3> ExtendedType;
};

class TraitFloatBufferVector8x5 {
public:
    typedef TraitGeneric<float> FallbackTraits;

    typedef TraitGeneric<float>::Type InternalType;
    static const int step = FloatT8<5>::SIZE;

    typedef FloatT8<3> Type;
    typedef FloatT8<3> SignedType;
    typedef FloatT8<3> ExtendedType;
};

#elif defined( WITH_SSE )
class TraitDoubleBufferVector {
public:
    typedef TraitGeneric<double> FallbackTraits;

    typedef TraitGeneric<double>::Type InternalType;
    static const int step = Doublex2::SIZE;

    typedef Doublex2 Type;
    typedef Doublex2 SignedType;
    typedef Doublex2 ExtendedType;
};
#endif




template<int inputNumber=1, int ouputNumber=1>
class G12BufferAlgebraScalar {
public:
    typedef ScalarAlgebraMulti<TraitG12Buffer, TraitG12Buffer, inputNumber, ouputNumber> Type;
};

#ifdef WITH_SSE

template<int inputNumber=1, int ouputNumber=1>
class G12BufferAlgebra {
public:
    typedef VectorAlgebraMulti<TraitG12BufferVector, inputNumber, ouputNumber> Type;
};

template<int inputNumber=1, int ouputNumber=1>
class G12BufferAlgebraStreaming {
public:
    typedef VectorAlgebraMultiStreaming<TraitG12BufferVector, inputNumber, ouputNumber> Type;
};
#else
template<int inputNumber=1, int ouputNumber=1>
class G12BufferAlgebra {
public:
    typedef ScalarAlgebraMulti<TraitG12Buffer, TraitG12Buffer,inputNumber, ouputNumber> Type;
};
#endif

/* Algebra for G8 buffer */
template<int inputNumber, int ouputNumber>
class G8BufferAlgebra {
public:
    typedef ScalarAlgebraMulti<TraitG8Buffer, TraitG8Buffer,inputNumber, ouputNumber> Type;
};



template<int inputNumber, int outputNumber>
class ScalarAlgebraUInt16_t
{
public:
    typedef TraitGeneric<uint16_t> TraitUInt16;
    typedef ScalarAlgebraMulti<TraitUInt16, TraitUInt16, inputNumber, outputNumber> Type;
};

template<int inputNumber, int outputNumber>
class ScalarAlgebraInt16_t
{
public:
    typedef TraitGeneric<int16_t> TraitInt16;
    typedef ScalarAlgebraMulti<TraitInt16, TraitInt16,inputNumber, outputNumber> Type;
};

template<int inputNumber, int outputNumber>
class ScalarAlgebraDouble
{
public:
    typedef TraitGeneric<double> TraitDouble;
    typedef ScalarAlgebraMulti<TraitDouble, TraitDouble, inputNumber, outputNumber> Type;
};

#if defined(WITH_SSE) || defined(WITH_AVX)
template<int inputNumber = 1, int outputNumber = 1>
class AlgebraDouble
{
public:
    typedef VectorAlgebraMulti<TraitDoubleBufferVector, inputNumber, outputNumber> Type;
};

template<int inputNumber = 1, int outputNumber = 1>
class VectorAlgebraDouble
{
public:
    typedef VectorAlgebraMulti<TraitDoubleBufferVector, inputNumber, outputNumber> Type;
};

#if defined(WITH_AVX)
template<int inputNumber = 1, int outputNumber = 1>
class VectorAlgebraDoubleEx
{
public:
    typedef VectorAlgebraMulti<TraitDoubleBufferVector8, inputNumber, outputNumber> Type;
};

template<int inputNumber = 1, int outputNumber = 1>
class VectorAlgebraDoubleEx5
{
public:
    typedef VectorAlgebraMulti<TraitDoubleBufferVector4x5, inputNumber, outputNumber> Type;
};

template<int inputNumber = 1, int outputNumber = 1>
class VectorAlgebraFloatEx5
{
public:
    typedef VectorAlgebraMulti<TraitFloatBufferVector8x5, inputNumber, outputNumber> Type;
};
#endif

#else
template<int inputNumber = 1, int outputNumber = 1>
class AlgebraDouble
{
public:
    typedef TraitGeneric<double> TraitDouble;
    typedef ScalarAlgebraMulti<TraitDouble, TraitDouble, inputNumber, outputNumber> Type;

};
#endif

template<int inputNumber, int outputNumber>
class ScalarAlgebraFloat
{
public:
    typedef TraitGeneric<float> TraitFloat;
    typedef ScalarAlgebraMulti<TraitFloat, TraitFloat, inputNumber, outputNumber> Type;
};




} //namespace corecvs

#endif
