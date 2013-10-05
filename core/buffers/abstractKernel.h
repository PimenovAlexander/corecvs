/**
 * \file abstractKernel.h
 * \brief This class holds a convolution kernel
 *
 *
 * \ingroup cppcorefiles
 * \date Feb 26, 2010
 * \author alexander
 */

#ifndef ABSTRACTKERNEL_H_
#define ABSTRACTKERNEL_H_

#include "global.h"

#include "abstractBuffer.h"

namespace corecvs {

/**
 *  \brief Template class that holds the convolution kernel. Most probably depreciated
 *  functionality
 **/
template<typename ElementType, typename IndexType>
class AbstractKernel : public AbstractBuffer<ElementType, IndexType>
{
public:
    ElementType invFactor;
    ElementType bias;
    IndexType x;
    IndexType y;

    AbstractKernel();
    AbstractKernel(IndexType h, IndexType w, ElementType *data,
            ElementType _invFactor,
            ElementType _bias,
            IndexType _x,
            IndexType _y
    ) : AbstractBuffer<ElementType, IndexType>(h, w, data)
    {
        invFactor = _invFactor;
        bias = _bias;
        x = _x;
        y = _y;
    }

    virtual ~AbstractKernel(){}

    template<typename BufferElementType, typename BufferIndexType>
        ElementType multiplyAtPoint (
                AbstractBuffer<BufferElementType, BufferIndexType> *buffer,
                BufferIndexType bcx,
                BufferIndexType bcy
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
                if (buffer->isValidCoord(bx, by))
                {
                    sum = sum + this->element(dy, dx) * buffer->element(bx, by);
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


} //namespace corecvs
#endif /* ABSTRACTKERNEL_H_ */
