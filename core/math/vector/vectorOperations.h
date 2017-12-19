#ifndef VECTOROPERATIONS_H
#define VECTOROPERATIONS_H

/**
 * \file vectorOperations.h
 * \brief This file holds the operations that are commonly used with
 * continuous linear data structures such as arrays and vectors, with
 * both dynamic and static size.
 *
 * This class uses static polymorphism to access the elements of the container.
 * Two (or three) methods should be implemented to use VectorOperations
 *
 * ElementType &at(int i);
 * const ElementType &at(int i) const;
 * int size();
 *
 * Also this implementation expect that RealType has two defined subtype
 * InnerElementType
 *
 * This particular implementation expects the elements to be continuous in memory
 * <pre>
 * (&A[0]) + 1 = &A[1]
 * </pre>
 *
 * This template class has 3 parameters
 *    ReturnType  - The type that should be returned in all constructive functions
 *    RealType    - The type that holds all the data needed for processing
 *    ElementType - The element type
 *
 * TODO: Consider using size_t instead of int
 * TODO: Consider using iterator and function object
 *
 * \date Jun 13, 2011
 * \author alexander
 */

#include <limits>
#include <iostream>

#include <stdio.h>
#include <cmath>

#include "core/utils/global.h"

#include "core/reflection/reflection.h"

using std::numeric_limits;
//using std::istream;
//using std::ostream;
//using std::cout;

/* This is shortcut.We are not proud of it*/
using std::sqrt;
using std::pow;

namespace corecvs {

template<typename RealType, typename ElementType, typename ReturnType = RealType>
class VectorOperationsBase
{
public:
    /**
     *  Static cast functions
     **/
    inline RealType *realThis() {
        return static_cast<RealType *>(this);
    }

    inline const RealType *realThis() const {
        return static_cast<const RealType *>(this);
    }

    inline ReturnType *returnThis() {
        return static_cast<ReturnType *>(this);
    }

    inline const ReturnType *returnThis() const {
        return static_cast<const ReturnType *>(this);
    }

private:
    /**
     *  Interface related functions
     **/
    inline ElementType &_at(int i) {
        return realThis()->at(i);
    }

    inline const ElementType &_at(int i) const {
        return realThis()->at(i);
    }

    inline int _size() const {
        return realThis()->size();
    }

    inline ReturnType _createVector(int length) const {
        return realThis()->createVector(length);
    }

public:

    /**
     *  Assignment that fills all the elements with the type
     *
     *
     **/
    inline ReturnType& operator =(const ElementType &x)
    {
        for (int i = 0; i < _size(); i++)
            _at(i) = x;
        return *(returnThis());

    }

    /**
     * This function copies the vector data from the other vector
     **/

    template<typename OtherRealType>
    inline VectorOperationsBase& operator =(
            const VectorOperationsBase<OtherRealType, typename OtherRealType::InnerElementType> &that)
    {
        OtherRealType *realThat = (OtherRealType *)&that;
        int thisSize = _size();
        int thatSize = realThat->size();
        int minSize = thatSize < thisSize ? thatSize : thisSize;

        for (int i = 0; i < minSize; i++)
            _at(i) = realThat->at(i);
        return *this;
    }


    /**
     * Equality operator
     * Doesn't have much sense in case of doubles but with integers - quite useful
     *
     **/
    inline bool operator ==(const RealType &that) const
    {
        if (_size() != that.size())
            return false;

        for (int i = 0; i < _size(); i++)
            if (_at(i) != that.at(i))
                return false;
        return true;
    }

    /**
     * Inequality operator.
     * Doesn't have much sense in case of doubles but with integers - quite useful
     *
     **/

    inline bool operator !=(const RealType &that) const
    {
        return !(operator ==(that));
    }

    /**
     * Equal with epsilon
     * Doesn't have much sense in case of integers but with doubles - quite useful
     **/
    inline bool notTooFar(const RealType &that, const ElementType &epsilon = ElementType(0)) const
    {
        if (_size() != that.size())
           return false;

        for (int i = 0; i < _size(); i++)
            if (_at(i) > that.at(i) + epsilon ||
                _at(i) < that.at(i) - epsilon )
                return false;
        return true;
    }


    /**
     *  Add-to-this operator
     *  Performs addition on element by element basis
     *
     *    \param V
     *        Reference to the vector to add
     *    \return
     *        Reference to current vector
     **/
    inline ReturnType& operator +=(const RealType& V)
    {
        int length = _size() < V.size() ? _size() : V.size();

        for (int i = 0; i < length; i++)
            _at(i) += V.at(i);
        return *returnThis();
    }

    inline ReturnType& operator -=(const RealType& V)
    {
        int length = _size() < V.size() ? _size() : V.size();

        for (int i = 0; i < length; i++)
            _at(i) -= V.at(i);
        return *returnThis();
    }

    inline ReturnType& operator *=(const ElementType &c)
    {
        for (int i = 0; i < _size(); i++)
            _at(i) *= c;
        return *returnThis();
    }

    inline ReturnType& operator /=(const ElementType &c)
    {
        for (int i = 0; i < _size(); i++)
            _at(i) /= c;
        return *returnThis();
    }

    /**
     * Operator + does the component-specific addition
     *
     * \f[ (A_1 + B_1, A_2 + B_2, \dots A_n + B_n) \f]
     *
     * \param V1
     * \param V2
     *
     **/
    friend inline ReturnType operator +(const RealType &V1, const RealType &V2)
    {
        int length = V1._size() < V2._size() ? V1._size() : V2._size();
        ReturnType result = V1._createVector(length);

        for (int i = 0; i < length; i++)
            result.at(i) = V1._at(i) + V2._at(i);
        return result;
    }

    /**
     * Operator - does the component-specific subtraction
     * \f[ (A_1 - B_1, A_2 - B_2, \dots A_n - B_n) \f]
     *
     * \param V1
     * \param V2
     **/
    friend inline ReturnType operator -(const RealType &V1, const RealType &V2)
    {
        //cout << "Subtract v1: " << V1 << std::endl;
        //cout << "Subtract v2: " << V2 << std::endl;

        int length = V1._size() < V2._size() ? V1._size() : V2._size();
        ReturnType result = V1._createVector(length);

        for (int i = 0; i < length; i++)
            result.at(i) = V1._at(i) - V2._at(i);
        return result;
    }

    /**
     * Operator * does the component-specific multiplication
     * \f[ (A_1 * B_1, A_2 * B_2, \dots A_n * B_n) \f]
     *
     * \param V1
     * \param V2
     **/
    friend inline ReturnType operator *(const RealType &V1, const RealType &V2)
    {
        int length = V1._size() < V2._size() ? V1._size() : V2._size();
        ReturnType result = V1._createVector(length);

        for (int i = 0; i < length; i++)
            result.at(i) = V1._at(i) * V2._at(i);
        return result;
    }

    /**
     * Operator / does the component-specific division
     * \f[ (A_1 / B_1, A_2 / B_2, \dots A_n / B_n) \f]
     *
     * \param V1
     * \param V2
     **/
    friend inline ReturnType operator /(const RealType &V1, const RealType &V2)
    {
        int length = V1._size() < V2._size() ? V1._size() : V2._size();
        ReturnType result = V1._createVector(length);

        for (int i = 0; i < length; i++)
            result.at(i) = V1._at(i) / V2._at(i);
        return result;
    }

    /**
     * Operator * does the component-specific multiplication by constant
     * \f[ (A_1 * c, A_2 * c, \dots A_n * c) \f]
     *
     * TODO: Could be nice to have operator *non-commutative
     *
     * \param c
     * \param V
     **/
    friend inline ReturnType operator *(const ElementType &c, const RealType &V)
    {
        ReturnType result = V._createVector(V._size());

        for (int i = 0; i < V._size(); i++)
            result.at(i) = V._at(i) * c;
        return result;
    }

    friend inline ReturnType operator *(const RealType &V, const ElementType &c)
    {
        return operator *(c, V);

    }

    /**
     * Operator * does the component-specific multiplication by constant
     * \f[ (A_1 / c, A_2 / c, \dots A_n / c) \f]
     *
     * \param c
     * \param V
     **/
    friend inline ReturnType operator /(const RealType &V, const ElementType &c)
    {
        ReturnType result = V._createVector(V._size());

        for (int i = 0; i < V._size(); i++)
            result.at(i) = V._at(i) / c;
        return result;
    }

    /**
     * The dot product of two vectors.
     *
     * \f[\sum_{i=0}^n {V1_i V2_i}\f]
     *
     * \param
     *         V1
     * \param
     *         V2
     *
     *
     **/
    friend inline ElementType operator &(const RealType &V1, const RealType &V2)
    {
        int length = V1._size() < V2._size() ? V1._size() : V2._size();
        ElementType result(0);

        for (int i = 0; i < length; i++)
            result += V1._at(i) * V2._at(i);
        return result;
    }

    ReturnType dotProduct(const RealType &other)
    {
        return (*this) & other;
    }

    friend ReturnType dotProduct(const RealType &V, const RealType &U)
    {
        return V & U;
    }

    /**
     * returns l-inf on [min(abs(x_i), abs(x_i/y_i))]
     */
    inline ElementType absRelInfNorm(const RealType& wrt) const
    {
        CORE_ASSERT_TRUE_S(_size() == wrt._size());
        ElementType norm = 0;
        for (int i = 0; i < _size(); ++i)
        {
            auto wrt_i = wrt._at(i), this_i = _at(i);
            auto norm_i = std::abs(this_i) / (std::abs(wrt_i) > 1.0 ? std::abs(wrt_i) : 1);
            norm = std::max(norm, norm_i);
        }
        return norm;
    }

    /**
     * Sum of the elements of the vector.
     *
     * \f[\sum_{i=0}^n {V_i}\f]
     *
     **/
    inline ElementType sumAllElements() const
    {
        ElementType result(0);
        for (int i = 0; i < _size(); i++)
            result += _at(i);
        return result;
    }

    /**
     * Product of all elements of the vector.
     *
     * \f[\prod_{i=0}^n {V_i}\f]
     *
     **/
    inline ElementType mulAllElements() const
    {
        ElementType result(1.0);
        for (int i = 0; i < _size(); i++)
            result *= _at(i);
        return result;
    }

    /**
     * Sum of the squares of element of the vector.
     *
     * \f[\sum_{i=0}^n {V_i^2}\f]
     *
     **/
    inline ElementType sumAllElementsSq() const
    {
        ElementType result(0);
        for (int i = 0; i < _size(); i++)
            result += _at(i) * _at(i);
        return result;
    }

    /**
     * Sum of the powers of element of the vector.
     *
     * \f[\sum_{i=0}^n {V_i^k}\f]
     *
     **/
    template<int power>
    inline ElementType sumAllElementsPow() const
    {
        ElementType result(0);
        for (int i = 0; i < _size(); i++)
            result += pow(_at(i), power);
        return result;
    }

    /**
     * Euclidean distance
     *
     * \f[D = \sqrt{\sum_{i=0}^n {V_i^2}} \f]
     *
     **/
    inline ElementType l2Metric() const
    {
        /* TODO ASAP: Correct this to select appropriate sqrt */
        return (ElementType)sqrt(this->sumAllElementsSq());
    }

    /**
     * Ln norm distance
     *
     * \f[D = \sqrt{\sum_{i=0}^n {V_i^2}} \f]
     *
     **/
    template <int power>
    inline ElementType lnMetric() const
    {
        /* TODO ASAP: Correct this to select appropriate sqrt */
        return (ElementType)pow(sumAllElementsPow<power>(), (1.0 / (double)power));
    }

    inline ElementType l2MetricStable() const
    {
        ElementType result(0);
        ElementType w(0);

        for (int i = 0; i < _size(); i++) {
            ElementType el = std::abs(_at(i));
            if (el > w)
                w = el;
        }

        if (w == 0) return 0;

        for (int i = 0; i < _size(); i++) {
            ElementType tmp = _at(i) / w;
            result += tmp * tmp;
        }

        /* TODO ASAP: Correct this to select appropriate sqrt */
        return w * sqrt(result);
    }

    /**
     * Another alias for Euclidean distance
     *
     * \f[D = \sqrt{\sum_{i=0}^n {V_i^2}} \f]
     *
     **/
    inline ElementType operator !() const
    {
        return l2Metric();
    }

    /**
     * The normalized dot product of two vectors. This a sort of correlation
     *
     * \f[cos(\alpha) = { \sum_{i=0}^n {V1_i V2_i} \over \sqrt{\sum_{i=0}^n {V1_i^2}} \sqrt{\sum_{i=0}^n {V2_i^2}}}\f]
     *
     **/
    ElementType cosineTo(const RealType &other) const
    {
        double thisLength  = !(*this);
        double otherLength = !other;

        if (thisLength == ElementType(0) || otherLength == ElementType(0)) {
            return ElementType(0);
        }
        return ((*realThis()) & other) / (thisLength * otherLength);
    }

    /**
     * Normalize a vector length to 1 without changing the direction
     *
     * \f[V := V \over {\|V_i\|} \f]
     *
     **/
    inline void normalise()
    {
        ElementType length = this->l2Metric();
        if (length != 0)
            this->operator /=(length);
    }

    inline void normaliseStable()
    {
        ElementType length = this->l2MetricStable();
        if (length != 0)
            this->operator /=(length);
    }

    /**
     * Return a normalized copy of vector
     *
     * \f[W = V \over {\|V_i\|} \f]
     *
     **/
    inline ReturnType normalised() const
    {
        ElementType length = this->l2Metric();
        if (length != ElementType(0))
            return (*returnThis() / length);
        else {
            return this->cloneVector();
        }
    }

    inline ReturnType normalisedStable() const
    {
        ElementType length = this->l2MetricStable();
        if (length != ElementType(0))
            return (*returnThis() / length);
        else {
            return this->cloneVector();
        }
    }

    /**
     * Return copy of vector
     **/
    inline ReturnType cloneVector() const
    {
        ReturnType result = this->_createVector(this->_size());
        for (int i = 0; i < this->_size(); i++)
            result.at(i) = this->_at(i);
        return result;
    }

    /**
     * Sum of the absolute values of element of the vector.
     *
     * \f[ \|V_i\|_1 = \sum_{i=0}^n {\|V_i\|}\f]
     *
     *  More info: http://en.wikipedia.org/wiki/Lp_space#The_p-norm_in_finite_dimensions
     **/
    inline ElementType l1Metric() const
    {
        ElementType result = 0;
        for (int i = 0; i < _size(); i++)
            result += (_at(i) > ElementType(0)) ? _at(i) : -_at(i);
        return result;
    }

    /**
     * Value of the maximum absolute value
     *
     * \f[ \|V_i\|_{\inf} = max ({\|V_i\|})\f]
     *
     * More info: http://en.wikipedia.org/wiki/Lp_space#The_p-norm_in_finite_dimensions
     **/
    inline ElementType lInfMetric() const
    {
        ElementType result = 0;
        for (int i = 0; i < _size(); i++)
        {
            ElementType abs = (_at(i) > ElementType(0)) ? _at(i) : -_at(i);
            if (abs > result)
                result = abs;
        }
        return result;
    }

    /**
     * Negative operator
     *
     * \f[V := -V\f]
     *
     **/
    ReturnType inline operator -() const
    {
        ReturnType result = _createVector(_size());
        for (int i = 0; i < _size(); i++)
            result.at(i) = -_at(i);
        return result;
    }

    /**
     * Per-elementSqrt
     *
     * \f[W_i = sqrt(V_i) \f]
     *
     **/
    ReturnType inline perElementSqrt() const
    {
        RealType result = _createVector(_size());
        for (int i = 0; i < _size(); i++)
            result.at(i) = (ElementType)sqrt(_at(i));
        return result;
    }


    /**
     * Per-element Abs
     *
     * \f[W_i = sqrt(V_i) \f]
     *
     **/
    ReturnType inline perElementAbs() const
    {
        RealType result = _createVector(_size());
        for (int i = 0; i < _size(); i++)
            result.at(i) = (ElementType)CORE_ABS(_at(i));
        return result;
    }

    ReturnType inline perElementMax(const RealType &other) const
    {
        int size = CORE_MIN(_size(), other._size());
        RealType result = _createVector(size);
        for (int i = 0; i < size; i++)
            result.at(i) = (ElementType)CORE_MAX(_at(i), other._at(i));
        return result;
    }

    ReturnType inline perElementMin(const RealType &other) const
    {
        int size = CORE_MIN(_size(), other._size());
        RealType result = _createVector(size);
        for (int i = 0; i < size; i++)
            result.at(i) = (ElementType)CORE_MIN(_at(i), other._at(i));
        return result;
    }

    /**
     * Per-element cast
     *
     *  Casts each element to the given type, then back.
     *  useful for rounding all elements
     *
     **/
    template <typename Type>
    ReturnType inline perElementCast() const
    {
        RealType result = _createVector(_size());
        for (int i = 0; i < _size(); i++)
            result.at(i) = (ElementType)static_cast<Type>(_at(i));
        return result;
    }

    /**
     *  Classic map function
     **/
    template<typename F>
    ReturnType inline mapF(const F& lambda) const
    {
        RealType result = _createVector(_size());
        for (int i = 0; i < _size(); i++)
            result.at(i) = lambda(_at(i));
        return result;
    }

    /**
     *   Check if current vector is inside the n dimensional cube
     *   \f[
     *      low_{i} \le this_{i} \le high_{i}
     *   \f]
     *
     *   NB - both low and high limits are included.
     *
     **/
    //bool isInHypercube(const VectorOperationsBase &low, const VectorOperationsBase &high) const
    bool isInHypercube(const RealType &low, const RealType &high) const
    {
        for (int i = 0; i < _size(); i++)
        {
            if (_at(i) <  low._at(i)) return false;
            if (_at(i) > high._at(i)) return false;

        }
        return true;
    }


    void mapToHypercube(const RealType &low, const RealType &high)
    {
        for (int i = 0; i < _size(); i++)
        {
            if (_at(i) <  low._at(i)) _at(i) =  low._at(i);
            if (_at(i) > high._at(i)) _at(i) = high._at(i);
        }
    }

    friend std::ostream & operator <<(std::ostream &out, const RealType &vector)
    {
        out << "[";
        for (int i = 0; i < vector._size(); i++)
           out << (i == 0 ? "" : ", ") << vector._at(i);
        out << "]";
        return out;
    }

    friend std::istream & operator >>(std::istream &in, RealType &vector)
    {
        for (int i = 0; i < vector._size(); i++)
            in >> vector._at(i);
        return in;
    }

    void print() const
    {
        std::cout << *realThis();
    }


    void println() const
    {
        print();
        std::cout << "\n";
    }

    inline ElementType minimum() const
    {
        ElementType minimum = numeric_limits<ElementType>::max();
        for (int i = 0; i < _size(); i++)
        {
            if (_at(i) < minimum)
                minimum = _at(i);
        }
        return minimum;
    }

    inline ElementType maximum() const
    {
        ElementType maximum = numeric_limits<ElementType>::lowest();
        for (int i = 0; i < _size(); i++)
        {
            if (_at(i) > maximum)
                maximum = _at(i);
        }
        return maximum;
    }

    inline ElementType minimumId() const
    {
        ElementType minimum = numeric_limits<ElementType>::max();
        int minId = 0;
        for (int i = 0; i < _size(); i++)
        {
            if (_at(i) < minimum) {
                minimum = _at(i);
                minId = i;
            }
        }
        return minId;
    }

    inline ElementType maximumId() const
    {
        ElementType maximum = numeric_limits<ElementType>::lowest();
        int maxId = 0;
        for (int i = 0; i < _size(); i++)
        {
            if (_at(i) > maximum) {
                maximum = _at(i);
                maxId = i;
            }
        }
        return maxId;
    }


    inline ElementType minimumAbsId() const
    {
        ElementType minimum = numeric_limits<ElementType>::max();
        int minId = 0;
        for (int i = 0; i < _size(); i++)
        {
            ElementType absolute = CORE_ABS(_at(i));
            if (absolute < minimum) {
                minimum = absolute;
                minId = i;
            }
        }
        return minId;
    }

    /* Reflection block */
    typedef typename ReflectionHelper<ElementType>::Type ReflectionType;

    /* Rewrite this */
template<class VisitorType>
    void accept1(VisitorType &visitor)
    {
        ElementType defaultValue(0);
        char name[50];
        for (int i = 0; i < _size() ; i++)
        {
            snprintf2buf(name, "a[%d]", i);
            visitor.visit(_at(i), defaultValue, name);
        }
    }


template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        ElementType defaultValue(0);
        char name[50];
        for (int i = 0; i < _size() ; i++)
        {
            snprintf2buf(name, "a[%d]", i);
            const ReflectionType fieldDescriptor(i, defaultValue, name);
            visitor.visit(_at(i), &fieldDescriptor);
        }
    }

    bool hasNans()
    {
        for (int i = 0; i < _size() ; i++)
        {
            if (std::isnan(_at(i)))
                return true;
        }
        return false;
    }

};

} //namespace corecvs

#endif // VECTOROPERATIONS_H

