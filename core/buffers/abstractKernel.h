#ifndef ABSTRACTKERNEL_H_
#define ABSTRACTKERNEL_H_

/**
 * \file abstractKernel.h
 * \brief This class holds a convolution kernel
 *
 *
 * \ingroup cppcorefiles
 * \date Feb 26, 2010
 * \author alexander
 */


#include "utils/global.h"
#include "buffers/abstractBuffer.h"

namespace corecvs {

/**
 *  \brief Template class that holds the convolution kernel. Most probably depreciated
 *  functionality
 **/
template<typename ElementType, typename IndexType = int32_t>
class AbstractKernel : public AbstractBuffer<ElementType, IndexType>
{
public:    
    ElementType invFactor;
    ElementType bias;
    IndexType x;
    IndexType y;

    AbstractKernel(IndexType h, IndexType w, ElementType *data,
            ElementType invFactor,
            ElementType bias,
            IndexType x,
            IndexType y
    ) :
      AbstractBuffer<ElementType, IndexType>(h, w, data),
      invFactor(invFactor),
      bias(bias),
      x(x),y(y)
    {}

    AbstractKernel(
        IndexType h,
        IndexType w,
        IndexType x,
        IndexType y
    ) :
      AbstractBuffer<ElementType, IndexType>(h, w),
      invFactor(1.0),
      bias(0.0),
      x(x),y(y)
    {}

    AbstractKernel(IndexType h = 0, IndexType w = 0, ElementType *data = nullptr)
        : AbstractBuffer<ElementType, IndexType>(h, w, data),
          invFactor(1.0), bias(0.0), x(w / 2), y(h / 2)
    {
    }

    virtual ~AbstractKernel(){}

    template<typename BufferElementType, typename BufferIndexType, bool allValid = false>
        ElementType multiplyAtPoint (
                AbstractBuffer<BufferElementType, BufferIndexType> *buffer,
                BufferIndexType bcy,
                BufferIndexType bcx
        )
    {
        IndexType dx,dy;
        ElementType sum = 0;
        unsigned count = 0;

        for (dy = 0; dy < this->h; dy++)
        {
            for (dx = 0; dx < this->w; dx++)
            {
                // TODO: Make this recurrent
                BufferIndexType bx = bcx - this->x + dx;
                BufferIndexType by = bcy - this->y + dy;

                // Let's pray for inlining and branch prediction
                if (allValid || buffer->isValidCoord(by, bx))
                {
                    sum = sum + this->element(dy, dx) * buffer->element(by, bx);
                    count++;
                }
            }
        }

        /// \todo TODO: Redo this! This a terrible approach. We should copy margin elements instead
        if (count != ((unsigned)this->h * (unsigned)this->w))
        {
            sum = sum * (this->h * this->w) / count;
        }
        return sum / invFactor + bias;
    }

};

typedef AbstractKernel<double> DpKernel;
typedef AbstractKernel<float> FpKernel;


} //namespace corecvs
#endif /* ABSTRACTKERNEL_H_ */
