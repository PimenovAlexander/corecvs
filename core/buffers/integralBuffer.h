#ifndef INTERGALBUFFER_H_
#define INTERGALBUFFER_H_
/**
 * \file integralBuffer.h
 * \brief А header for IntergalBuffer.c
 *
 * This file holds methods and data structures to support IntegralBuffer optimisation
 * for Viola-Jones optimizations
 *
 *
 *
 * \ingroup cppcorefiles
 * \date Feb 23, 2010
 * \author: alexander
 */
#include <stdio.h>
#ifdef WITH_SSE
#include <emmintrin.h>
#endif
#include "math/sse/sseWrapper.h"
#include "math/neon/neonWrapper.h"

#include "utils/global.h"

#include "buffers/abstractBuffer.h"
#include "buffers/g12Buffer.h"
#include "buffers/g8Buffer.h"

#include "tbbwrapper/tbbWrapper.h"

#include "geometry/rectangle.h"

namespace corecvs {


 /**
  *
  * So each element of the integral buffer will hold the sum of the elements in a rectangle
  *
  *
  * \f[ Integ(x,y) = \sum_{i=0, j=0}^{j<x,i<y} I(j,i) \f]
  *
  **/
template<typename ElementType, typename BaseElementType, typename IndexType>
class IntegralBuffer : public AbstractBuffer<ElementType, IndexType> {

public:
    typedef BaseElementType InternalBaseElementType;
    typedef AbstractBuffer<BaseElementType, IndexType> SourceBufferType;


    IntegralBuffer(SourceBufferType *input) :
        AbstractBuffer<ElementType, IndexType>(input->h + 1, input->w + 1, false)
    {
        IndexType i,j;
        // Scan the first line and column
        for (j = 0; j < this->getW(); j++)
            this->element(0, j) = ElementType(0);

        for (i = 0; i < this->getH(); i++)
            this->element(i, 0) = ElementType(0);

        // Scan all the picture
        for (i = 1; i < this->getH(); i++)
        {
            ElementType sum(0);
            BaseElementType * baseOffset      = &(input->element(i - 1, 0));
            ElementType     * currentOffset   = &(this-> element(i    , 1));
            ElementType     * prevOffset      = &(this-> element(i - 1, 1));

            for (j = 1; j < this->getW(); j ++)
            {
#ifdef ASSERTS
//                ElementType test_sum = sum + input->element(i, j);
//                CORE_ASSERT_FALSE((test_sum < sum || test_sum < input->element(i, j)), "Overflow while creating the integral buffer");

#endif
                sum += *baseOffset;
                *currentOffset = sum + *prevOffset;
                baseOffset++;
                currentOffset++;
                prevOffset++;
            }
        }

    }

    /**
     * This function returns the size of the buffer from which the intergal buffer was produced.
     *
     * */
    IndexType getEffectiveH() const
    {
        return this->h - 1;
    }

    /**
     * This function returns the size of the buffer from which the intergal buffer was produced.
     *
     * */
    IndexType getEffectiveW() const
    {
        return this->w - 1;
    }

  /**
  *  This function calculates the sum inside the rectangle
     With the structure of the intergal buffer it can be done in 4 random accesses

     \f[ \sum_{x=x_0}^{x <= x_1}\sum_{y=y_0}^{y <= y_1} I(x,y) = Integ(x_1 + 1, y_1 + 1) + Integ(x_0, y_0) - Integ(x_1 + 1, y_0) - Integ(x_0, y_1 + 1) \f]

  **/
    inline ElementType rectangle (IndexType x1, IndexType y1, IndexType x2, IndexType y2)
    {
        return    this->element(y1, x1    ) + this->element(y2 + 1, x2 + 1)
                - this->element(y1, x2 + 1) - this->element(y2 + 1, x1);
    }

    inline ElementType rectangle (const Rectangle<IndexType> &rect)
    {
        return rectangle(rect.left(), rect.top(), rect.right(), rect.bottom());
    }

    template<typename ReturnType>
    class ParallelRectangularBlur
    {
        ReturnType *toReturn;
        IndexType hSizeHalf;
        IndexType wSizeHalf;
        IntegralBuffer<ElementType, BaseElementType, IndexType> *buf;

    public:
        void operator()( const BlockedRange<IndexType>& r ) const
        {
            IndexType hSize = hSizeHalf * 2;
            IndexType wSize = wSizeHalf * 2;

            IndexType area = (wSize + 1) * (hSize + 1);

            for( IndexType i = r.begin(); i < r.end(); i++ )
            {
                /* Holding the pointers to 4 corners of the blur zone */
                ElementType *aCorner = &(buf->element(i,0));
                ElementType *cCorner = &(buf->element(i + hSize + 1,0));

                ElementType *bCorner = aCorner + (wSize + 1);
                ElementType *dCorner = cCorner + (wSize + 1);

                typename ReturnType::InternalElementType *dest = &(toReturn->element(i,0));

                for (IndexType j = 0; j < toReturn->w; j ++)
                {
                   ElementType value = *aCorner + *dCorner - *bCorner - *cCorner;
                   *dest = value / area;
                   aCorner++;
                   bCorner++;
                   cCorner++;
                   dCorner++;
                   dest++;
                }
            }
        }
        ParallelRectangularBlur(ReturnType *_toReturn, IndexType _hSizeHalf,
                                IndexType _wSizeHalf,
                                IntegralBuffer<ElementType, BaseElementType, IndexType> *_buf) :
        toReturn(_toReturn), hSizeHalf(_hSizeHalf), wSizeHalf(_wSizeHalf), buf(_buf)
        {}
    };

    /**
     * This function calculates the buffer that holds the mean values over the
     * squares of the size  (2 * wSizeHalf + 1) by (2 * hSizeHalf + 1)
     *
     **/
    template<typename ReturnType>
    inline ReturnType *rectangularBlur(
            IndexType hSizeHalf,
            IndexType wSizeHalf,
            bool reduceSize = true)
    {

        ReturnType *toReturn;
        int newh = this->getEffectiveH() - 2 * hSizeHalf;
        int neww = this->getEffectiveW() - 2 * wSizeHalf;

        if (reduceSize) {
            toReturn = new ReturnType( newh, neww, false);
            parallelable_for(0, toReturn->h, ParallelRectangularBlur<ReturnType>(toReturn, hSizeHalf, wSizeHalf, this));
            return toReturn;
        } else {
            // TODO: There is an overkill, it is enough to just zero the margins
            toReturn = new ReturnType(this->getEffectiveH(), this->getEffectiveW(), true);
            ReturnType view = toReturn->template createView<ReturnType>(hSizeHalf, wSizeHalf, newh, neww);
            parallelable_for(0, view.h, ParallelRectangularBlur<ReturnType>(&view, hSizeHalf, wSizeHalf, this));
            return toReturn;
        }
    }

/*
    ElementType rectangleBl (double x1, double y1, double x2, double y2)
    {
        int x1i = ceil(x1);
        int y1i = ceil(y1);

        int x2i = floor(x2);
        int y2i = floor(y2);

        ElementType sum(0);

        if (x2i > x1i && y2i > y1i)
          sum =   this->element(y1i, x1i    ) + this->element(y2i + 1, x2i + 1)
                - this->element(y1i, x2i + 1) - this->element(y2i + 1, x1i);
      // Now we should add the float margins



      return sum;
    }
*/
};


class G12IntegralBuffer : public IntegralBuffer<uint32_t, G12Buffer::InternalElementType, int32_t> {
public:
    G12IntegralBuffer(G12Buffer *input) :
        IntegralBuffer<uint32_t, G12Buffer::InternalElementType, int32_t>(input) {}

#ifdef WITH_SSE
    /**
     *  This functions could calculate 4 sum rectangle values with SSE2
     *
     *       32bit            32bit                32bit              32bit
     *  [x1,y1,x2,y2][x1 + 1,y1,x2 + 1,y2][x1 + 2,y1,x2 + 2,y2][x1 + 3,y1,x2 + 3,y2]
     *
     **/


    ALIGN_STACK_SSE inline __m128i rectangle_sse(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
    {
            __m128i a = _mm_loadu_si128((__m128i*)&this->element(y1,     x1    ));
            __m128i b = _mm_loadu_si128((__m128i*)&this->element(y1,     x2 + 1));
            __m128i c = _mm_loadu_si128((__m128i*)&this->element(y2 + 1, x1    ));
            __m128i d = _mm_loadu_si128((__m128i*)&this->element(y2 + 1, x2 + 1));

            return _mm_sub_epi32(
                       _mm_add_epi32(a,d),
                       _mm_add_epi32(b,c)
                   );
  //         Int32x4 a(&this->element(y1,     x1    ));
  //          return a + c - b - d;
    }
#endif

#if defined(WITH_SSE) || defined(WITH_NEON)

    ALIGN_STACK_SSE inline Int32x4 rectangle_sse_new(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
    {
        Int32x4 a(&this->element(y1,     x1    ));
        Int32x4 b(&this->element(y1,     x2 + 1));
        Int32x4 c(&this->element(y2 + 1, x1    ));
        Int32x4 d(&this->element(y2 + 1, x2 + 1));
        return a + d - b - c;
    }

    class ParallelRectangularBlurSSE
    {
        G12Buffer *toReturn;
        int hSizeHalf;
        int wSizeHalf;
        G12IntegralBuffer *buf;

    public:
        ALIGN_STACK_SSE void operator()( const BlockedRange<int>& r ) const
        {
            int hSize = hSizeHalf * 2;
            int wSize = wSizeHalf * 2;

            int area = (wSize + 1) * (hSize + 1);

            float div = 1.0f / (area);
            Float32x4 divisor(div);

            for( int i = r.begin(); i < r.end(); i++ )
            {
                /* Holding the pointers to 4 corners of the blur zone */
                uint32_t *aCorner = &(buf->element(i,0));
                uint32_t *cCorner = &(buf->element(i + hSize + 1,0));

                uint32_t *bCorner = aCorner + (wSize + 1);
                uint32_t *dCorner = cCorner + (wSize + 1);

                uint16_t *dest = &(toReturn->element(i,0));

                int j;
                for ( j = 0; j + 7 < toReturn->w; j += 8)
                {

                    Int32x4 a(aCorner);
                    Int32x4 b(bCorner);
                    Int32x4 c(cCorner);
                    Int32x4 d(dCorner);

                    Int32x4 sum = a + d - b - c;

                    Float32x4 float_sum(sum);
                    Float32x4 float_result = float_sum * divisor;
                    Int32x4 result_hi = float_result.trunc();

                    a = Int32x4(aCorner + 4);
                    b = Int32x4(bCorner + 4);
                    c = Int32x4(cCorner + 4);
                    d = Int32x4(dCorner + 4);

                    sum = a + d - b - c;

                    float_sum = Float32x4(sum);
                    float_result = float_sum * divisor;
                    Int32x4 result_low = float_result.trunc();


                    Int16x8 result = Int16x8::pack(result_hi, result_low);
                    result.save(dest);

                    aCorner+=8;
                    bCorner+=8;
                    cCorner+=8;
                    dCorner+=8;
                    dest+=8;
                }

                for (    ; j  < toReturn->w; j ++)
                {
                   uint32_t value = *aCorner + *dCorner - *bCorner - *cCorner;
                   *dest = value / area;
                   aCorner++;
                   bCorner++;
                   cCorner++;
                   dCorner++;
                   dest++;
                }
            }
        }
        ParallelRectangularBlurSSE(G12Buffer *_toReturn, int _hSizeHalf,
                                int _wSizeHalf,
                                G12IntegralBuffer *_buf) :
        toReturn(_toReturn), hSizeHalf(_hSizeHalf), wSizeHalf(_wSizeHalf), buf(_buf)
        {}
    };

    ALIGN_STACK_SSE inline G12Buffer *rectangularBlurSSE(
            int hSizeHalf, int wSizeHalf)
    {
        G12Buffer *toReturn = new G12Buffer(
                this->getEffectiveH() - 2 * hSizeHalf,
                this->getEffectiveW() - 2 * wSizeHalf,
                false);

        parallelable_for(0, toReturn->h, ParallelRectangularBlurSSE(toReturn, hSizeHalf, wSizeHalf, this));
        return toReturn;
    }


#endif
};


    class G8IntegralBuffer : public IntegralBuffer<uint32_t, G8Buffer::InternalElementType, int32_t> {
public:
    G8IntegralBuffer(G8Buffer *input) :
        IntegralBuffer<uint32_t, G8Buffer::InternalElementType, int32_t>(input) {}

#ifdef WITH_SSE
    /**
     *  This functions could calculate 4 sum rectangle values with SSE2
     *
     *       32bit            32bit                32bit              32bit
     *  [x1,y1,x2,y2][x1 + 1,y1,x2 + 1,y2][x1 + 2,y1,x2 + 2,y2][x1 + 3,y1,x2 + 3,y2]
     *
     **/
    ALIGN_STACK_SSE __m128i rectangle_sse(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
    {
        return _mm_sub_epi32(_mm_add_epi32(_mm_loadu_si128((__m128i*)&this->element(y1,x1    )),_mm_loadu_si128((__m128i*)&this->element(y2 + 1,x2 + 1))),
                             _mm_add_epi32(_mm_loadu_si128((__m128i*)&this->element(y1,x2 + 1)),_mm_loadu_si128((__m128i*)&this->element(y2 + 1,x1    ))));
    }
#endif

#if defined(WITH_SSE) || defined(WITH_NEON)
    ALIGN_STACK_SSE Int32x4 rectangle_sse_new(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
    {
        Int32x4 a(this->element(y1,    x1    ));
        Int32x4 b(this->element(y2 + 1,x2 + 1));
        Int32x4 c(this->element(y1,x2 + 1));
        Int32x4 d(this->element(y2 + 1,x1    ));
        return a + b - c - d;
    }

#endif
};


} //namespace corecvs
#endif  //INTERGALBUFFER_H_

