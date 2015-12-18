#pragma once
/**
 * \file abstractContiniousBuffer.h
 * \brief a header for AbstractContiniousBuffer.c
 *
 *
 * \ingroup cppcorefiles
 * \date Feb 26, 2010
 * \author: alexander
 *
 *
 * TODO: Add nearest neighbor      GEDB
 * TODO: Add colorspace converters GEDB
 */
#include "global.h"
#include "abstractBuffer.h"
#include "matrix.h"

//#include "fixedPointBlMapper.h"
//#include "bilinearMapPoint.h"

namespace corecvs {

/**
 * This template class describes a buffer for which it is possible to
 * interpolate the result between cells.
 *
 * TODO: Check type conversions - get rid of the warnings
 *
 */
template<typename ElementType, typename IndexType = int32_t>
class AbstractContiniousBuffer : public AbstractBuffer<ElementType, IndexType>
{
public:
    typedef AbstractBuffer<ElementType, IndexType> BaseClass;

    AbstractContiniousBuffer(IndexType h, IndexType w, bool shouldInit = true, IndexType stride = BaseClass::STRIDE_AUTO)
        : BaseClass(h, w, shouldInit, stride)
    {}
    AbstractContiniousBuffer(Vector2d<IndexType> size, bool shouldInit = true, IndexType stride = BaseClass::STRIDE_AUTO)
        : BaseClass(size, shouldInit, stride)
    {}

    AbstractContiniousBuffer(BaseClass &that) : BaseClass(that) {}
    AbstractContiniousBuffer(BaseClass *that) : BaseClass(that) {}

    /**
     * Constructor that creates the buffer form the raw input data
     **/
    AbstractContiniousBuffer(IndexType h, IndexType w, ElementType *data) :
        BaseClass(h, w, data) {}
    AbstractContiniousBuffer(Vector2d<IndexType> size, ElementType *data) :
        BaseClass(size, data) {}

    AbstractContiniousBuffer(BaseClass *src, IndexType x1, IndexType y1, IndexType x2, IndexType y2) :
        BaseClass(src, x1, y1, x2, y2) {}

    /**
     *  Constructors that fill the resulted array with data element
     **/
    AbstractContiniousBuffer(IndexType h, IndexType w, const ElementType &data, IndexType stride = BaseClass::STRIDE_AUTO) :
        BaseClass(h, w, data, stride) {}
    AbstractContiniousBuffer(Vector2d<IndexType> size, const ElementType &data, IndexType stride = BaseClass::STRIDE_AUTO) :
        BaseClass(size, data, stride) {}

    AbstractContiniousBuffer() {}

    inline ElementType elementBl(const Vector2dd &p)
    {
        return elementBl(p.y(), p.x());
    }

    inline ElementType elementBlFast(const Vector2dd &p)
    {
        return elementBlFast(p.y(), p.x());
    }

    /**
     * The element getter with spline interpolation
     **/
//    inline ElementType elementS3(double y, double x)
//    {
//        IndexType i = (IndexType) floor(y);
//        IndexType j = (IndexType) floor(x);

//        CORE_ASSERT_TRUE_P(this->isValidCoordBl(y,x),
//            ("Invalid coordinate in AbstractContiniousBuffer::elementBl(double y=%lf, double x=%lf) buffer sizes is [%dx%d]",
//             y, x, this->w, this->h));

//        double a = (double)this->element(i    ,j    );
//        double b = (double)this->element(i    ,j + 1);
//        double c = (double)this->element(i + 1,j    );
//        double d = (double)this->element(i + 1,j + 1);

//        double derAY = c - a;
//        double derCY = c - a;
//        double derBY = d - b;
//        double derDY = d - b;
//        double derAX = b - a;
//        double derBX = b - a;
//        double derCX = d - c;
//        double derDX = d - c;

//        if (i > 0)
//        {
//            derAY = (c - (double)this->element(i - 1, j)) * 0.5;
//            derBY = (d - (double)this->element(i - 1, j + 1)) * 0.5;
//        }
//        if (i + 2 < this->h)
//        {
//            derCY = ((double)this->element(i + 2, j) - a) * 0.5;
//            derDY = ((double)this->element(i + 2, j + 1) - b) * 0.5;
//        }
//        if (j > 0)
//        {
//            derAX = (b - (double)this->element(i, j - 1)) * 0.5;
//            derCX = (d - (double)this->element(i + 1, j - 1)) * 0.5;
//        }
//        if (j + 2 < this->w)
//        {
//            derBX = ((double)this->element(i, j + 2) - a) * 0.5;
//            derDX = ((double)this->element(i + 1, j + 2) - c) * 0.5;
//        }

//        double b0 = 4 * a - 4 * b - 4 * c + 4 * d + 2 * derAX + 2 * derBX
//                - 2 * derCX - 2 * derDX + 2 * derAY - 2 * derBY + 2 * derCY - 2 * derDY;
//        double b1 = - 6 * a + 6 * b + 6 * c - 6 * d - 3 * derAX - 3 * derBX
//                + 3 * derCX + 3 * derDX - 4 * derAY + 4 * derBY - 2 * derCY + 2 * derDY;
//        double b2 = 2 * derAY - 2 * derBY;
//        double b3 = 2 * a - 2 * b + derAX + derBX;
//        double b4 = - 6 * a + 6 * b + 6 * c - 6 * d - 4 * derAX - 2 * derBX
//                + 4 * derCX + 2 * derDX - 3 * derAY + 3 * derBY - 3 * derCY + 3 * derDY;
//        double b5 = 9 * a - 9 * b - 9 * c + 9 * d + 6 * derAX + 3 * derBX
//                - 6 * derCX - 3 * derDX + 6 * derAY - 6 * derBY + 3 * derCY - 3 * derDY;
//        double b6 = - 3 * derAY + 3 * derBY;
//        double b7 = - 3 * a + 3 * b - 2 * derAX - derBX;
//        double b8 = 2 * derAX - 2 * derCX;
//        double b9 = - 3 * derAX + 3 * derCX;
//        double b10 = 0;
//        double b11 = derAX;
//        double b12 = 2 * a - 2 * c + derAY + derCY;
//        double b13 = - 3 * a + 3 * c - 2 * derAY - derCY;
//        double b14 = derAY;
//        double b15 = a;


//        double point = b0 * pow(x - j, 3) * pow(y - i, 3) +
//               b1 * pow(x - j, 3) * pow(y - i, 2) +
//               b2 * pow(x - j, 3) * (y - i) +
//               b3 * pow(x - j, 3) +
//               b4 * pow(x - j, 2) * pow(y - i, 3) +
//               b5 * pow(x - j, 2) * pow(y - i, 2) +
//               b6 * pow(x - j, 2) * (y - i) +
//               b7 * pow(x - j, 2) +
//               b8 * (x - j) * pow(y - i, 3) +
//               b9 * (x - j) * pow(y - i, 2) +
//               b10 * (x - j) * (y - i) +
//               b11 * (x - j) +
//               b12 * pow(y - i, 3) +
//               b13 * pow(y - i, 2) +
//               b14 * (y - i) +
//               b15;
//        return (ElementType)point;
//    }

    /**
     * The element getter with bilinear filtration
     *
     *
     **/
    inline ElementType elementBl(double y, double x)
    {
        /* floor() is needed here because of values (-1..0] which will be
         * rounded to 0 and cause error */
        IndexType i = (IndexType)floor(y);
        IndexType j = (IndexType)floor(x);

        CORE_ASSERT_TRUE_P(this->isValidCoordBl(y, x),
            ("Invalid coordinate in AbstractContiniousBuffer::elementBl(double y=%lf, double x=%lf) buffer sizes is [%dx%d]",
             y, x, this->w, this->h));

        ElementType a = this->element(i    ,j    );
        ElementType b = this->element(i    ,j + 1);
        ElementType c = this->element(i + 1,j    );
        ElementType d = this->element(i + 1,j + 1);

        double k1 = x - j;
        double k2 = y - i;

        ElementType result =
             (a * (1 - k1) + k1 * b) * (1 - k2) +
             (c * (1 - k1) + k1 * d) *      k2;
        return result;
    }

    /**
     * The element getter with bilinear filtration
     * This version is using integer multiplication
     *
     **/
    inline ElementType elementBlInt(double y, double x)
    {
        /* floor() is needed here because of values (-1..0] which will be
         * rounded to 0 and cause error */
        IndexType i = floor(y);
        IndexType j = floor(x);

        CORE_ASSERT_TRUE_P(this->isValidCoordBl(y, x),
            ("Invalid coordinate in AbstractContiniousBuffer::elementBl(double y=%lf, double x=%lf) buffer sizes is [%dx%d]",
             y, x, this->w, this->h));

        ElementType a = this->element(i    ,j    );
        ElementType b = this->element(i    ,j + 1);
        ElementType c = this->element(i + 1,j    );
        ElementType d = this->element(i + 1,j + 1);

        double k1 = (x - j);
        double k2 = (y - i);
        double k3 = 1.0 - k1;
        double k4 = 1.0 - k2;

        int f1 = k3 * k4 * 256.0;
        int f2 = k1 * k4 * 256.0;
        int f3 = k3 * k2 * 256.0;
        int f4 = k1 * k2 * 256.0;

        ElementType result =
             (a * f1 + f2 * b) +
             (c * f3 + f4 * d);
        return result / 256;
    }

    /**
     * Alternative version (faster)
     *
     * http://en.wikipedia.org/wiki/Bilinear_interpolation
     *
     * GEDB
     *
     * Could not be used for uint buffers
     *
     *
     **/
    inline ElementType elementBlFast(double y, double x)
    {
        /* floor() is needed here because of values (-1..0] which will be
         * rounded to 0 and cause error */
        IndexType i = floor(y);
        IndexType j = floor(x);

        CORE_ASSERT_TRUE_P(this->isValidCoordBl(y, x),
            ("Invalid coordinate in AbstractContiniousBuffer::elementBl(double y=%lf, double x=%lf) buffer sizes is [%dx%d]",
             y, x, this->w, this->h));

        ElementType &a = this->element(i    ,j    );
        ElementType &b = this->element(i    ,j + 1);
        ElementType &c = this->element(i + 1,j    );
        ElementType &d = this->element(i + 1,j + 1);

        double k1 = x - j;
        double k2 = y - i;

        ElementType b1 = a;
        ElementType b2 = b - a;
        ElementType b3 = c - a;
        ElementType b4 = d - b2 - c;
        ElementType result = b1 + b2 * k1 + b3 * k2 + b4 * k1 * k2;
        return result;
    }

    /**
     *  Checks if coordinate lies within the buffer area
     *
     *  \param y
     *      The y coordinate of the point to check
     *  \param x
     *      The x coordinate of the point to check
     **/
    inline bool isValidCoordBl(double y, double x) const
    {
        /*return (floor(x) >= 0) && (ceil(x) < this->w) &&
                 (floor(y) >= 0) && (ceil(y) < this->h);*/
        return (floor(x) >= 0) && (floor(x) + 1.0 < this->w) &&
               (floor(y) >= 0) && (floor(y) + 1.0 < this->h);
    }

    /**
     *  Checks if coordinate lies within the buffer area
     **/
    inline bool isValidCoordBl(const Vector2dd &p) const
    {
        return isValidCoordBl(p.y(), p.x());
    }




    /**
     *  This class provides a parallel execution of bilinear pixel mapping.
     *  Basically it moves pixels to new position based on the DeformMapType buffer that should implement
     *  the map(i,j) function
     *
     **/
    template<class ReturnType, class DeformMapType>
    class ParallelDoReverseDeformationBl
    {
        ReturnType *toReturn;
        const DeformMapType *map;
        AbstractContiniousBuffer<ElementType, IndexType> *buf;

    public:
        ParallelDoReverseDeformationBl(
                ReturnType *_toReturn,
                const DeformMapType *_map,
                AbstractContiniousBuffer<ElementType, IndexType> *_buf) :
        toReturn(_toReturn), map(_map), buf(_buf)
        {}


        void operator()( const BlockedRange<IndexType>& r ) const
        {
            IndexType j;
            IndexType newW = toReturn->w;

            for (IndexType i = r.begin(); i < r.end(); i++)
            {
                for (j = 0; j < newW - 1; j++)
                {
                    Vector2dd p = map->map(i,j);
                    if (buf->isValidCoordBl(p)) {
                        toReturn->element(i,j) = buf->elementBl(p);
                    } else {
                        toReturn->element(i,j) = ElementType(0x0);
                    }
                }
            }
        }
    };

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
     *         Output Buffer Width
     **/
    template<class ReturnType, class DeformMapType>
    ReturnType *doReverseDeformationBl(const DeformMapType *map, IndexType newH = -1, IndexType newW = -1)
    {
        if (newH == -1) {
            newH = this->h;
        }
        if (newW == -1) {
            newW = this->w;
        }

        ReturnType *toReturn = new ReturnType(newH, newW);
        DOTRACE(("Starting transform to %d %d...\n", newW - 1, newH - 1));
        parallelable_for((IndexType)0, (IndexType)(newH-1), ParallelDoReverseDeformationBl<ReturnType, DeformMapType>(toReturn, map, this));
        return toReturn;
    }


    /**
     *  This class provides a parallel execution of pixel mapping.
     *  Basically it moves pixels to new position based on the DeformMapType buffer that should implement
     *  the map(i,j) function
     *
     **/

    template<class ReturnType, class DeformMapType>
    class ParallelDoReverseDeformation
    {
        ReturnType *toReturn;
        DeformMapType &map;
        AbstractContiniousBuffer<ElementType, IndexType> *buf;
    public:

        ParallelDoReverseDeformation(
            ReturnType *_toReturn,
            DeformMapType &_map,
            AbstractContiniousBuffer<ElementType, IndexType> *_buf) :
        toReturn(_toReturn), map(_map), buf(_buf)
        {}

        void operator()( const BlockedRange<IndexType>& r ) const
        {
            IndexType j;
            IndexType newW = toReturn->w;

            for (IndexType i = r.begin(); i < r.end(); i++)
            {
                for (j = 0; j < newW - 1; j++)
                {
                    Vector2dd p = map.map(i,j);
                    if (buf->isValidCoord(p.y(), p.x()))
                        toReturn->element(i,j) = buf->element(p.y(), p.x());
                    else
                        toReturn->element(i,j) = typename ReturnType::InternalElementType(0x0);
                }
            }
        }
    };

    template<class ReturnType, class DeformMapType>
    ReturnType *doReverseDeformation(DeformMapType &map, IndexType newH = -1, IndexType newW = -1)
    {
        if (newH == -1) {
            newH = this->h;
        }
        if (newW == -1) {
            newW = this->w;
        }

        ReturnType *toReturn = new ReturnType(newH, newW);
        DOTRACE(("Starting integer transform to %d %d...\n", newW - 1, newH - 1));

        parallelable_for((IndexType)0, (IndexType)(newH-1), ParallelDoReverseDeformation<ReturnType, DeformMapType>(toReturn, map, this));

        return toReturn;
    }

};

} //namespace corecvs
