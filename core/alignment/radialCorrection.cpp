/**
 * \file radialCorrection.cpp
 * \brief This file contains the radial lens distortion correction primitives
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */

#include "global.h"
#include "radialCorrection.h"

namespace corecvs {


RadialCorrection::RadialCorrection(const LensCorrectionParametres &params) :
        FunctionArgs(2,2),
        mParams(params)
{
}

RadialCorrection::~RadialCorrection()
{
}

#if 0
Vector2dd RadialCorrection::getCorrectionForPoint(Vector2dd input)
{
    double dx = input.x() - mParams->center.x();
    double dy = input.y() - mParams->center.y();
    double dxsq = dx * dx;
    double dysq = dy * dy;
    double dxdy = dx * dy;

    double rsq = dxsq + dysq;
    double radialCorrection = 0;

    double r = sqrt(rsq);
    for (unsigned i = 0; i < mParams->koeff.size(); i ++)
    {
        radialCorrection += mParams->koeff.at(i) * pow(r, i + 1);
    }


    double radialX = (double)dx * radialCorrection;
    double radialY = (double)dy * radialCorrection;

    double tangentX = 2 * mParams->p1 * dxdy + mParams->p2 * ( rsq + 2 * dxsq );
    double tangentY = mParams->p1 * (rsq + 2 * dysq) + 2 * mParams->p2 * dxdy;

    return Vector2dd(radialX + tangentX, radialY + tangentY);
}
#endif

G12Buffer *RadialCorrection::doCorrectionTransform(G12Buffer *inputBuffer)
{
    return inputBuffer->doReverseDeformationBl<G12Buffer, RadialCorrection>
                         (this, inputBuffer->h, inputBuffer->w);
}
} //namespace corecvs
