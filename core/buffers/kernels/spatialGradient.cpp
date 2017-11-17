/**
 * \file spatialGradient.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Sep 22, 2010
 * \author alexander
 */

#include "core/utils/global.h"

#include "core/buffers/kernels/spatialGradient.h"
#include "core/buffers/kernels/fastkernel/fastKernel.h"
#include "core/buffers/kernels/fastkernel/scalarAlgebra.h"
#include "core/buffers/kernels/gaussian.h"
#include "core/buffers/kernels/fastkernel/vectorTraits.h"
namespace corecvs {



SpatialGradient::SpatialGradient(G12Buffer *input) : AbstractBuffer<Vector3dd, uint32_t>(input->h, input->w)
{
    for (int32_t i = 1; i < input->h - 1; i++)
    {
        for (int32_t j = 1; j < input->w - 1; j++)
        {
            double Ix = (double)(input->element(i, j + 1) - input->element(i, j - 1))/2;
            double Iy = (double)(input->element(i + 1, j) - input->element(i - 1, j))/2;
            element(i,j) = Vector3dd(Ix*Ix, Ix*Iy, Iy*Iy);
        }
    }
}


template<int inputNumber, int outputNumber>
class ScalarAlgebraVector33
{
public:
    typedef TraitGeneric<Vector3dd> TraitVector33;
    typedef ScalarAlgebraMulti<TraitVector33, TraitVector33, inputNumber, outputNumber> Type;
};

/**
 *  R=detM-(SCALLER*(traceM)^2)
 * points in which R is smaller then scaler are forced to zero color
 **/
G12Buffer* SpatialGradient::findCornerPoints(double scaler, int apperture)
{
    int v = apperture;
    printf("SpatialGradient::findCornerPoints(%lf, %d):called\n", scaler, apperture);

    SpatialGradient *start    = this;
    SpatialGradient *blurVert = new SpatialGradient(h, w);
    SpatialGradient *blur     = new SpatialGradient(h, w);

    /* TODO: Redo this to simplify for buffers */
    if (v == 5)
    {
        BufferProcessor<SpatialGradient, SpatialGradient, Blur5Horisontal, ScalarAlgebraVector33> blurerH;
        BufferProcessor<SpatialGradient, SpatialGradient, Blur5Vertical, ScalarAlgebraVector33>   blurerV;

        blurerV.process(&start   , &blurVert);
        blurerH.process(&blurVert, &blur);
    }
    else
    {
        printf("SpatialGradient::findCornerPoints() %d\n", v);

        for (uint32_t i = 0 ; i < start->h; i++)
        {
            for (uint32_t j = 0; j < start->w; j++)
            {
                Vector3dd sum = Vector3dd::Zero();

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


    G12Buffer *toReturn = new G12Buffer(h, w);

    for (int i = 2; i < (int)h - 2; i++)
    {
        for (int j = 2; j < (int)w - 2; j++)
        {
            Vector3dd grad = blur->element(i,j);
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

} //namespace corecvs

