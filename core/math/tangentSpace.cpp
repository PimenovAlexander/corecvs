#include "tangentSpace.h"

namespace corecvs {

TangentSpaceSO3::TangentSpaceSO3()
{}

Matrix33 corecvs::TangentSpaceSO3::expM()
{
    double    t = theta();
    Matrix33  w = toMatrix();

    if (t != 0) {
        return  Matrix33::Identity() +
                (sin(t) / t) * w +
                (1 -  cos(t)) / (t * t) * w * w;
    }

    return Matrix33::Identity();
}

Quaternion TangentSpaceSO3::exp()
{
    std::cout << "TangentSpaceSO3::exp() not implemented!" << std::endl;

    return Quaternion();
}

} // namespace corecvs
