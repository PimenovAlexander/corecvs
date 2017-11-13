#ifndef PACKED_DERIVATIVE_H
#define PACKED_DERIVATIVE_H

#include <iostream>
#include <cmath>

namespace corecvs {

using std::sqrt;

template<int dimension = 1>
class PackedDerivative
{
public:
    double value = 0.0;
    double derivative[dimension];

    explicit PackedDerivative(double value) :
        value(value)
    {
        for (int i = 0; i < dimension; i++)
            derivative[i] = 0.0;
    }

    explicit PackedDerivative(int value) :
        value(value)
    {
        for (int i = 0; i < dimension; i++)
            derivative[i] = 0.0;
    }

    explicit PackedDerivative(double value, const double _derivative[dimension]) :
        value(value)
    {
        for (int i = 0; i < dimension; i++)
            derivative[i] = _derivative[i];
    }

    static PackedDerivative ID(double value, int num) {
        PackedDerivative toReturn(value);
        if (num < dimension) {
            toReturn.derivative[num] = 1.0;
        }
        return toReturn;
    }

    static PackedDerivative X(double value) {
        return ID(value, 0);
    }

    static PackedDerivative Y(double value) {
        return ID(value, 1);
    }

    static PackedDerivative Z(double value) {
        return ID(value, 2);
    }

    PackedDerivative() {}

    friend std::ostream& operator << (std::ostream &out, PackedDerivative &toSave)
    {
        out << "(" << toSave.value << "; ";
        for (int i = 0; i < dimension; i++) {
             out << (i == 0 ? "" : ", ") << toSave.derivative[i] ;
        }
        out << ")" ;
        return out;
    }

    friend inline PackedDerivative operator *(const PackedDerivative &left, const PackedDerivative &right)
    {
        PackedDerivative result(left.value * right.value);

        for (int i = 0; i < dimension; i++)
            result.derivative[i] = left.value * right.derivative[i] + left.derivative[i] * right.value;

        return result;
    }

    friend inline PackedDerivative operator +(const PackedDerivative &left, const PackedDerivative &right)
    {
        PackedDerivative result(left.value + right.value);

        for (int i = 0; i < dimension; i++)
            result.derivative[i] = left.derivative[i] + right.derivative[i];

        return result;
    }

    friend inline PackedDerivative operator +=(PackedDerivative &left, const PackedDerivative &right)
    {
        left.value += right.value;

        for (int i = 0; i < dimension; i++)
            left.derivative[i] += right.derivative[i];

        return left;
    }

    friend inline PackedDerivative operator -(const PackedDerivative &left, const PackedDerivative &right)
    {
        PackedDerivative result(left.value - right.value);

        for (int i = 0; i < dimension; i++)
            result.derivative[i] = left.derivative[i] - right.derivative[i];

        return result;
    }

    friend inline PackedDerivative operator -(const PackedDerivative &right)
    {
        PackedDerivative result(-right.value);

        for (int i = 0; i < dimension; i++)
            result.derivative[i] = -right.derivative[i];

        return result;
    }

    friend inline PackedDerivative operator /(const PackedDerivative &left, const PackedDerivative &right)
    {
        PackedDerivative result(left.value / right.value);

        double vsq = right.value * right.value;
        for (int i = 0; i < dimension; i++)
            result.derivative[i] = (left.derivative[i] * right.value - left.value * right.derivative[i]) / vsq;

        return result;
    }

    friend inline PackedDerivative sqrt(const PackedDerivative &left)
    {
        PackedDerivative result(sqrt(left.value));
        for (int i = 0; i < dimension; i++)
        {
            result.derivative[i] = 0.5 / result.value * left.derivative[i];
        }

        return result;
    }

    /* King Midas style operators. This could be optimized. */

    PackedDerivative operator =(const double &right)
    {
        *this = PackedDerivative(right);
        return *this;
    }

    PackedDerivative operator =(const int &right)
    {
        *this = PackedDerivative(right);
        return *this;
    }


    friend inline PackedDerivative operator *(const PackedDerivative &left, const double &right)
    {
        return  left * PackedDerivative(right);
    }

    friend inline PackedDerivative operator *(const double &left, const PackedDerivative &right)
    {
        return PackedDerivative(left) * right;
    }

    friend inline PackedDerivative operator +(const PackedDerivative &left, const double &right)
    {
        return left + PackedDerivative(right);
    }

    friend inline PackedDerivative operator +(const double &left, const PackedDerivative &right)
    {
        return PackedDerivative(left) + right;
    }

    friend inline PackedDerivative operator -(const PackedDerivative &left, const double &right)
    {
        return left - PackedDerivative(right);
    }

    friend inline PackedDerivative operator -(const double &left, const PackedDerivative &right)
    {
        return PackedDerivative(left) - right;
    }


    friend inline PackedDerivative operator /(const double &left, const PackedDerivative &right)
    {
        return PackedDerivative(left) / right;
    }


    friend inline PackedDerivative operator / (const PackedDerivative &left, const double &right)
    {
        return left / PackedDerivative(right);
    }

};

} //namespace corecvs

#endif // PACKED_DERIVATIVE_H
