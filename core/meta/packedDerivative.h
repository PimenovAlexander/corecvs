#ifndef PACKED_DERIVATIVE_H
#define PACKED_DERIVATIVE_H

#include <iostream>
#include <cmath>

namespace corecvs {

using std::sqrt;

class PackedDerivative
{
public:
    double value = 0.0;
    double derivative = 0.0;

    explicit PackedDerivative(double value, double derivative = 0.0) :
        value(value),
        derivative(derivative)
    {}

    static PackedDerivative X(double value) {
        return PackedDerivative(value, 1.0);
    }

    PackedDerivative();

    friend std::ostream& operator << (std::ostream &out, PackedDerivative &toSave)
    {
        out << "(" << toSave.value << "; "<< toSave.derivative << ")" << "\n";
        return out;
    }

};



inline PackedDerivative operator *(const PackedDerivative &left, const PackedDerivative &right)
{
    return PackedDerivative(left.value * right.value, left.value * right.derivative + left.derivative * right.value  );
}

inline PackedDerivative operator +(const PackedDerivative &left, const PackedDerivative &right)
{
    return PackedDerivative(left.value + right.value, left.derivative + right.derivative );
}

inline PackedDerivative operator +=(PackedDerivative &left, const PackedDerivative &right)
{
    left.value      += right.value;
    left.derivative += right.derivative;

    return left;
}

inline PackedDerivative operator -(const PackedDerivative &left, const PackedDerivative &right)
{
    return PackedDerivative(left.value - right.value, left.derivative - right.derivative );
}

inline PackedDerivative operator /(const PackedDerivative &left, const PackedDerivative &right)
{
    return PackedDerivative(left.value / right.value, (left.derivative * right.value - left.value * right.derivative) / (right.value * right.value));
}

inline PackedDerivative sqrt(const PackedDerivative &left)
{
    return PackedDerivative(sqrt(left.value), 0.5 / sqrt(left.value) * left.derivative );
}

/* King Midas style operators. This could be optimized. */

inline PackedDerivative operator *(const PackedDerivative &left, const double &right)
{
    return  left * PackedDerivative(right);
}

inline PackedDerivative operator *(const double &left, const PackedDerivative &right)
{
    return PackedDerivative(left) * right;
}

inline PackedDerivative operator +(const PackedDerivative &left, const double &right)
{
    return left + PackedDerivative(right);
}

inline PackedDerivative operator +(const double &left, const PackedDerivative &right)
{
    return PackedDerivative(left) + right;
}

inline PackedDerivative operator -(const PackedDerivative &left, const double &right)
{
    return left - PackedDerivative(right);
}

inline PackedDerivative operator -(const double &left, const PackedDerivative &right)
{
    return PackedDerivative(left) - right;
}


inline PackedDerivative operator /(const double &left, const PackedDerivative &right)
{
    return PackedDerivative(left) / right;
}


inline PackedDerivative operator / (const PackedDerivative &left, const double &right)
{
    return left / PackedDerivative(right);
}


} //namespace corecvs

#endif // PACKED_DERIVATIVE_H
