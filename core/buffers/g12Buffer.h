#pragma once
/**
 * \file g12Buffer.h
 * \brief This file holds the class description for 12 bit grayscale buffer
 *
 *
 * \ingroup cppcorefiles
 * \date Feb 22, 2010
 * \author alexander
 */
#include <iostream>

#include "global.h"

#include "abstractBuffer.h"
#include "abstractContiniousBuffer.h"
#include "fixedPointDisplace.h"

namespace corecvs {

typedef AbstractContiniousBuffer<uint16_t, int32_t> G12BufferBase;

/**
 * TODO: Add consts where it should go
 *
 *
 */
class G12Buffer : public G12BufferBase
{
public:
    static const int BUFFER_BITS      = 12;
    static const int BUFFER_MAX_VALUE = (1 << BUFFER_BITS) - 1;  // = 0x0FFF = 4095

    /**
     *
     * \brief This function creates a gray scale 12 bit buffer for the given width and height.
     *
     * In fact in the PC implementation a 16bit buffer is used. The created image is filled with 0 values
     *
     * \param h The height of the image to be created.
     * \param w The width of the image to be created.
     *
     * \param shouldInit if the buffer should be initialized (with 0 values)
     *
     */
    G12Buffer(int32_t h, int32_t w,   bool shouldInit = true) : G12BufferBase (h, w, shouldInit) {}
    G12Buffer(Vector2d<int32_t> size, bool shouldInit = true) : G12BufferBase (size, shouldInit) {}

    G12Buffer(G12Buffer &that) : G12BufferBase (that) {}
    G12Buffer(G12Buffer *that) : G12BufferBase (that) {}

    G12Buffer(G12Buffer *src, int32_t x1, int32_t y1, int32_t x2, int32_t y2) :
        G12BufferBase(src, x1, y1, x2, y2) {}

    G12Buffer(int32_t h, int32_t w, uint16_t *data) : G12BufferBase(h, w, data) {}

    /*TODO: Make this more generic */
    G12Buffer(int32_t _h, int32_t _w, uint16_t *data, int shuffle) : G12BufferBase(_h, _w)
    {
        int w = getW();
        int h = getH();
        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                uint16_t data16;
                if (!shuffle)
                    data16 = data[i * w + j];
                else
                    data16 = data[i * w + (w - j - 1)];

                this->element(i, j) = ((data16 & 0xFF00) >> 8) | ((data16 & 0x00FF) << 8);
            }
        }
    }

    /**
     * This function is used as a hack to interface the C-style g12Buffer
     *
     **/
    G12Buffer() {}

    /** Fills the buffer by random values within the given range */
    void fillWithRands(int valueMax = BUFFER_MAX_VALUE, int valueMin = 0)
    {
        G12BufferBase::fillWithRands(valueMax, valueMin);
    }

    /** Safe assignment of this buffer by the content from the given one */
    G12Buffer& operator =(G12Buffer& other)
    {
        if (!hasSameSize(&other)) {
            ASSERT_FAIL("buffers have different sizes for the assignment!");  // TODO: implement this case when need
        }
        else {
            fillWithRaw(other.data);
        }
        return *this;
    }

    void gainOffset (double gain, int offset);
    void shiftMask (uint32_t mask, int8_t shift);
    void intervalMap (uint32_t min, uint32_t max);

template<typename DeformMapType>
    inline G12Buffer *doReverseTransform(const DeformMapType *map, int32_t newY, int32_t newX)
    {
        G12Buffer *toReturn = doReverseDeformationBl<G12Buffer, DeformMapType>(map, newY, newX);
        return toReturn;
    }

    inline bool isValidCoordBlPrecomp(const BilinearMapPoint &point) const
    {
        return /*(point.x >= 0 ) &&*/ (point.x < this->getW() - 1) &&
               /*(point.y >= 0 ) &&*/ (point.y < this->getH() - 1);
    }

    G12Buffer::InternalElementType elementBlPrecomp(const BilinearMapPoint &point)
    {
        uint32_t a = this->element(point.y    , point.x    );
        uint32_t b = this->element(point.y    , point.x + 1);
        uint32_t c = this->element(point.y + 1, point.x    );
        uint32_t d = this->element(point.y + 1, point.x + 1);
        return (a * point.k1 + b * point.k2 + c * point.k3 + d * point.k4) / 255;
    }


    class ParallelDoReverseDeformationBlPrecomp
    {
        G12Buffer *toReturn;
        const FixedPointDisplace *map;
        G12Buffer *buf;

    public:
        ParallelDoReverseDeformationBlPrecomp(
            G12Buffer *_toReturn,
            const FixedPointDisplace *_map,
            G12Buffer *_buf
            ) :
        toReturn(_toReturn), map(_map), buf(_buf)
        {}

        void operator()( const BlockedRange<G12Buffer::InternalIndexType>& r ) const
        {
            G12Buffer::InternalIndexType j;
            G12Buffer::InternalIndexType newW = toReturn->getW();

            for (G12Buffer::InternalIndexType i = r.begin(); i < r.end(); i++)
            {
                for (j = 0; j < newW - 1; j++)
                {
                    BilinearMapPoint p = map->map(i,j);
                    if (buf->isValidCoordBlPrecomp(p))
                        toReturn->element(i,j) = buf->elementBlPrecomp(p);
                    else
                        toReturn->element(i,j) = 0x0;
                }
            }
        }

    }; // ParallelDoReverseDeformationBlPrecomp

    /**
     * This function template is responsible for applying reverse
     * transformation to the abstract buffer.
     *
     *
     * \param map
     *         The map to apply
     * \param newH
     *         Output Buffer Height
     * \param newW
     *        Output Buffer Width
     **/
    G12Buffer *doReverseDeformationBlPrecomp(const FixedPointDisplace *map, G12Buffer::InternalIndexType newH, G12Buffer::InternalIndexType newW)
    {
        G12Buffer *toReturn = new G12Buffer(newH, newW);
        DOTRACE(("Starting transform to %d %d...\n", newW - 1, newH - 1));
        parallelable_for(0, newH - 1, ParallelDoReverseDeformationBlPrecomp(toReturn, map, this));
        return toReturn;
    }

    /**
     *  \deprecated
     **/
    template<typename ConvElementType, typename ConvIndexType>
    inline G12Buffer *doConvolve1
        (
            AbstractKernel<ConvElementType, ConvIndexType> *kernel
        )
    {
        return doConvolve<G12Buffer, ConvElementType, ConvIndexType>(kernel);
    }

    static G12Buffer *difference (G12Buffer *first, G12Buffer *second);

    G12Buffer *binarize (uint16_t threshold);
    void fillWithYUYV (uint16_t *yuvu);

    void print();
    bool verify();

    friend ostream & operator <<(ostream &out, const G12Buffer &buffer);

    virtual ~G12Buffer();

}; // G12Buffer



} //namespace corecvs
