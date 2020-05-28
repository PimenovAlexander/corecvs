/**
 * \file spatialGradient.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Sep 22, 2010
 * \author alexander
 */

#include "utils/global.h"

#include "buffers/kernels/spatialGradient.h"
#include "buffers/kernels/fastkernel/fastKernel.h"
#include "buffers/kernels/fastkernel/scalarAlgebra.h"
#include "buffers/kernels/gaussian.h"
#include "buffers/kernels/fastkernel/vectorTraits.h"
namespace corecvs {


template<typename ContainerType>
AbstractSpatialGradient<ContainerType>::AbstractSpatialGradient(G12Buffer *input) : AbstractBuffer<ContainerType, int32_t>(input->h, input->w)
{
    for (int32_t i = 1; i < input->h - 1; i++)
    {
        for (int32_t j = 1; j < input->w - 1; j++)
        {
            InternalDoubleType Ix = (InternalDoubleType)(input->element(i, j + 1) - input->element(i, j - 1))/2;
            InternalDoubleType Iy = (InternalDoubleType)(input->element(i + 1, j) - input->element(i - 1, j))/2;
            this->element(i,j) = ContainerType(Ix*Ix, Ix*Iy, Iy*Iy);
        }
    }
}





/**
 *  R=detM-(SCALLER*(traceM)^2)
 * points in which R is smaller then scaler are forced to zero color
 **/

template<typename ContainerType>
G12Buffer* AbstractSpatialGradient<ContainerType>::findCornerPoints(double scaler, int apperture)
{
    int v = apperture;
    printf("SpatialGradient::findCornerPoints(%lf, %d):called\n", scaler, apperture);

    AbstractSpatialGradient *start    = this;

    AbstractSpatialGradient *blurVert = new AbstractSpatialGradient(this->getSize());
    AbstractSpatialGradient *blur     = new AbstractSpatialGradient(this->getSize());

    /* TODO: Redo this to simplify for buffers */
    if (v == 5)
    {


        BufferProcessor<AbstractSpatialGradient, AbstractSpatialGradient, Blur5Horisontal, SpacialGradientScalarAlgebra> blurerH;
        BufferProcessor<AbstractSpatialGradient, AbstractSpatialGradient, Blur5Vertical  , SpacialGradientScalarAlgebra> blurerV;

        blurerV.process(&start   , &blurVert);
        blurerH.process(&blurVert, &blur);
    }
    else
    {
        printf("SpatialGradient::findCornerPoints() %d\n", v);

        for (int32_t i = 0 ; i < start->h; i++)
        {
            for (int32_t j = 0; j < start->w; j++)
            {
                ContainerType sum = ContainerType::Zero();

                for (int dy = i - v / 2 ; dy <= (int)i + v / 2; dy++)
                {
                    for (int dx = j - v / 2 ; dx <= (int)j + v / 2; dx++)
                    {
                        if (!start->isValidCoord(dy,dx))
                            continue;
                        sum += start->element(dy,dx);
                    }
                }

                blur->element(i,j) = sum / (v*v);
            }
        }
    }


    G12Buffer *toReturn = new G12Buffer(this->getSize());

    for (int i = 2; i < (int)this->h - 2; i++)
    {
        for (int j = 2; j < (int)this->w - 2; j++)
        {
            ContainerType grad = blur->element(i,j);
            // Counting eigenvalues
            double a = 1.0;
            double b = - (grad.x() + grad.z());
            double c = grad.x() * grad.z() - grad.y() * grad.y();

            /* TODO: solve quadric to other class*/
            double D = b * b - 4 * a * c;
            if (D < 0)
            {
                toReturn->element(i,j) = 0;
            }
            else
            {
                double lambda1 = (-b + sqrt(D)) / (2.0 * a);
                double lambda2 = (-b - sqrt(D)) / (2.0 * a);

                if (lambda1 > scaler && lambda2 > scaler)
                {
                    double result = sqrt(lambda1 * lambda2) / scaler;
                    toReturn->element(i,j) = G12Buffer::InternalElementType(result > G12Buffer::BUFFER_MAX_VALUE ? G12Buffer::BUFFER_MAX_VALUE : result);
                }
            }
        }
    }

    delete blur;
    delete blurVert;
    return toReturn;
}

template class AbstractSpatialGradient<Vector3dd>;
template class AbstractSpatialGradient<Vector3df>;



} //namespace corecvs

