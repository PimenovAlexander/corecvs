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
#include "vectorOperations.h"


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

template<typename RealType, typename ElementType, int length>
class FixedVectorBase : public VectorOperationsBase<RealType, ElementType>
{
public:

    static const int LENGTH = length;
    typedef VectorOperationsBase<RealType, ElementType> BaseClass;
    typedef ElementType InnerElementType;

    //union {
        ElementType element[length];
        // While debugging you favorite datatype could be added here
    //};


    /**
     * This functions creates the object from the other fixed vector
     **/
    template<typename OtherRealType>
    inline FixedVectorBase(const FixedVectorBase<OtherRealType,typename OtherRealType::InnerElementType, length> &V)
    {
        for (int i = 0; i < length; i++)
            this->element[i] = (ElementType)V.element[i];
    }

    /**
     * This functions creates the fixed vector object from the other fixed vector of smaller size, adding an
     * element at the end
     **/
    template<typename OtherRealType>
    inline FixedVectorBase(
            const FixedVectorBase<OtherRealType,typename OtherRealType::InnerElementType, length - 1> &V,
            const typename OtherRealType::InnerElementType &lastElement)
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

    inline RealType createVector(int /*length*/) const {
        return RealType();
    }

};

template<typename ElementType, int length>
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

    inline FixedVector(
            const FixedVector<ElementType, length - 1> &V,
            const ElementType &lastElement) :
        BaseClass(V, lastElement)
    {}

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

