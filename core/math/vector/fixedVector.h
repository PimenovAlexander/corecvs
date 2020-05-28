#ifndef FIXEDVECTOR_H_
#define FIXEDVECTOR_H_

/**
 * \file fixedVector.h
 * \brief This file holds the description of the commonly used template -
 * a vector with the fixed number of components.
 *
 * \ingroup cppcorefiles
 * \date Feb 27, 2010
 * \author alexander
 */

#include <stdio.h>
#include "math/vector/vectorOperations.h"


namespace corecvs {
/**
 * This class is a template for a vector of the fixed length.
 *
 * This template could be parameterized by different types and vector lengths.
 *
 *
 *
 * In the code below we pray for static loop unrolling and good optimization.
 * Thus there will be no performance loss.
 *
 *
 */

template<
        typename ReturnType,             /**<  The type that will be returned from the operator and constuctive functions  */
        typename ElementType,            /**<  Internal type */
        int length                       /**<  Vector length */
       >
class FixedVectorBase : public VectorOperationsBase<FixedVectorBase<ReturnType, ElementType, length>, ElementType, ReturnType>
{
public:

    static const int LENGTH = length;
    typedef VectorOperationsBase<FixedVectorBase<ReturnType, ElementType, length>, ElementType, ReturnType> BaseClass;
    typedef ElementType InnerElementType;

    //union {
        ElementType element[length];
        // While debugging you favorite datatype could be added here
    //};

    /**
     * This functions creates the object from the other fixed vector
     **/
    template<typename OtherReturnType>
    inline FixedVectorBase(const FixedVectorBase<OtherReturnType,typename OtherReturnType::InnerElementType, length> &V)
    {
        for (int i = 0; i < length; i++)
            this->element[i] = (ElementType)V.element[i];
    }

    /**
     * This functions creates the fixed vector object from the other fixed vector of smaller size, adding an
     * element at the end
     **/
    template<typename OtherReturnType>
    inline FixedVectorBase(
            const FixedVectorBase<OtherReturnType,typename OtherReturnType::InnerElementType, length - 1> &V,
            const typename OtherReturnType::InnerElementType &lastElement)
    {
        for (int i = 0; i < length - 1; i++)
            this->element[i] = (ElementType)V.element[i];
        this->element[length - 1] = lastElement;
    }

    /**
     *   Copy constructor.
     *   Acts the same way as assignment constructor
     *
     **/
    explicit inline FixedVectorBase(const ElementType &x)
    {
        BaseClass::operator =(x);
    }

    /**
     *   Constructor from raw data.
     **/
    explicit inline FixedVectorBase(const ElementType* x)
    {
        for (int i = 0; i < length; i++)
            this->element[i] = x[i];
    }


    inline FixedVectorBase() {}

    int size() const
    {
        return length;
    }

    ElementType& operator [](int n)
    {       
        return element[n];
    }

    const ElementType& operator [](int n) const
    {
        return element[n];
    }

    ElementType& at(int n)
    {
        return element[n];
    }

    const ElementType& at(int n) const
    {
        return element[n];
    }

    inline ReturnType createVector(int /*length*/) const {
        return ReturnType();
    }

    static ReturnType NaN(){
        return ReturnType(std::numeric_limits<ElementType>::quiet_NaN());
    }

};

#if 0
/**
 *   This is a base class for data structures that work on top of the vector.
 *   For example Matrix33 can be a small 9 element vector. All inherited data types will take
 *   FixedVectorBase as input and produce Matrix33.
 **/
template<typename ReturnType, typename ElementType, int length>
class FixedVectorBase: public FixedVectorDataContainer< FixedVectorDataContainer<ReturnType, ElementType, length>, ElementType, length, ReturnType>
{
public:
    class FixedVectorDataContainer< FixedVectorDataContainer<ReturnType, ElementType, length>, ElementType, length, ReturnType> BaseClass;

    template<typename OtherRealType>
    inline FixedVectorBase(const FixedVectorBase<OtherRealType, typename OtherRealType::InnerElementType, length> &V) :
        BaseClass(V)
    {}

    explicit inline FixedVectorBase(const ElementType &x) : BaseClass(x) {}
    explicit inline FixedVectorBase(const ElementType* x) : BaseClass(x) {}
    explicit inline FixedVectorBase() {}

};
#endif

template<typename ElementType, int length >
class FixedVector : public FixedVectorBase<FixedVector<ElementType, length>, ElementType, length>
{
protected:
    typedef FixedVectorBase<FixedVector<ElementType, length>, ElementType, length> BaseClass;

public:

    template<typename OtherElementType>
    inline FixedVector(const FixedVector<OtherElementType, length> &V)
    {
        for (int i = 0; i < length; i++)
            this->element[i] = V.element[i];
    }

    template<typename SmallSibling>
    inline FixedVector(
            const FixedVectorBase<SmallSibling, ElementType, length - 1> &V,
            const ElementType &lastElement) :
        BaseClass(V, lastElement)
    {}

/*
    template<template <typename, int> class OtherVector>
    inline FixedVector(
            const FixedVectorBase<OtherVector<ElementType, length - 1>, ElementType, length - 1> &V,
            const ElementType &lastElement) :
        BaseClass(V, lastElement)
    {}
*/

    /**
     *   Copy constructor.
     *   Acts the same way as assignment constructor
     *
     **/
    explicit inline FixedVector(const ElementType &x) : BaseClass(x) {}

    /**
     *   Constructor from raw data.
     **/
    explicit inline FixedVector(const ElementType* x)
    {
        for (int i = 0; i < length; i++)
            this->element[i] = x[i];
    }


    inline FixedVector() {}

};



} //namespace corecvs
#endif /* FIXEDVECTOR_H_ */

