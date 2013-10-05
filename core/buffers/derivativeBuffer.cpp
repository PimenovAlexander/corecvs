/**
 * \file derivativeBuffer.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Sep 13, 2010
 * \author alexander
 */

#include "derivativeBuffer.h"
namespace corecvs {

DerivativeBuffer::DerivativeBuffer(G12Buffer *input) : DerivativeBufferBase(input->h, input->w)
{
    for (int i = 1; i < h - 1; i++)
    {
        for (int j = 1; j < w - 1; j++)
        {
            int16_t resultX = (
                    input->element(i - 1, j + 1) +
                2 * input->element(i    , j + 1) +
                    input->element(i + 1, j + 1)
                ) - (
                    input->element(i - 1, j - 1) +
                2 * input->element(i    , j - 1) +
                    input->element(i + 1, j - 1)
                );

            int16_t resultY =  (
                    input->element(i + 1, j - 1) +
                2 * input->element(i + 1, j    ) +
                    input->element(i + 1, j + 1)
                ) - (
                    input->element(i - 1, j - 1) +
                2 * input->element(i - 1, j    ) +
                    input->element(i - 1, j + 1)
                );
            element(i,j) = Vector2d<int16_t>(resultX, resultY);
        }
    }
}

G12Buffer *DerivativeBuffer::nonMaximalSuppression(/*uint16_t low, uint16_t hight*/)
{
    G12Buffer *toReturn = new G12Buffer(h,w);

    for (int i = 1; i < h - 1; i++)
    {
        for (int j = 1; j < w - 1; j++)
        {
            int dx, dy;
            double dIdx = element(i,j).x();
            double dIdy = element(i,j).y();

            do {
                if (dIdx == 0 && dIdy == 0) {
                    dx = 0; dy = 0;  break;
                }

                if (dIdx < 0) {
                    dIdx = -dIdx;
                    dIdy = -dIdy;
                }

                if (dIdy < -dIdx * sqrt(3.0)) {
                    dx =  0; dy = -1; break;
                }
                if (dIdy < -dIdx / sqrt(3.0)) {
                    dx =  1; dy = -1; break;
                }
                if (dIdy < dIdx / sqrt(3.0)) {
                    dx =  1; dy =  0; break;
                }
                if (dIdy < dIdx * sqrt(3.0)) {
                    dx =  1; dy =  1; break;
                }
                    dx =  0; dy =  1;

            } while (false);


            int16_t first   =  element(i + dy, j + dx).x();
            int16_t second  =  element(i - dy, j - dx).x();
            int16_t current =  element(i     , j     ).x();

            first   = (first   > 0) ?   first :    -first;
            second  = (second  > 0) ?  second :  - second;
            current = (current > 0) ? current : - current;


            if (current > second && current > first)
                toReturn->element(i, j) = (uint16_t)current;
            else
                toReturn->element(i, j) = 0x0;
        }

    }
    return toReturn;
}


G12Buffer *DerivativeBuffer::gradientMagnitudeBuffer()
{
    G12Buffer *toReturn = new G12Buffer(h,w);

    for (int i = 1; i < h - 1; i++)
    {
        for (int j = 1; j < w - 1; j++)
        {
            toReturn->element(i,j) = (this->element(i,j).l1Metric() / 2);
        }
    }
    return toReturn;
}


} //namespace corecvs

