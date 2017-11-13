#ifndef INTERPOLATOR_H_
#define INTERPOLATOR_H_
/**
 * \file interpolator.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jul 6, 2010
 * \author alexander
 */
#include "core/utils/global.h"

#include "core/buffers/g12Buffer.h"

namespace corecvs {


/**
 *  This class provides a parallel execution of pixel mapping based on the bilinear transform.
 *  Basically it moves pixels to new position based on the map function
 *  the map(i,j) function
 *
 **/
template<class InputType, class OutputType, class InternalIndexType, class InternalElementType>
class BlMapper {

public:

    template<class Mapper>
    class ParallelDoReverseDeformationBl
    {
        OutputType *toReturn;
        const Mapper *map;
        InputType *buf;

    public:
        ParallelDoReverseDeformationBl(
            OutputType *_toReturn,
            const Mapper *_map,
            InputType *_buf
            ) :
        toReturn(_toReturn), map(_map), buf(_buf)
        {}

        void operator()( const BlockedRange<InternalIndexType>& r ) const
        {
            InternalIndexType j;
            InternalIndexType newW = toReturn->getW();

            for (InternalIndexType i = r.begin(); i < r.end(); i++)
            {
                for (j = 0; j < newW - 1; j++)
                {
                    Vector2dd p = map->map(i,j);
                    if (buf->isValidCoordBl(p))
                        toReturn->element(i,j) = buf->elementBl(p);
                    else
                        toReturn->element(i,j) = InternalElementType(0x0);
                }
            }
        }

    }; // ParallelDoReverseDeformationBl

   /* inline bool isValidCoordBl(const Vector2dd &point) const
    {
        return (floor(point.x()) >= 0) && (floor(point.x()) + 1.0 < this->getW()) &&
               (floor(point.y()) >= 0) && (floor(point.y()) + 1.0 < this->getH());
    }*/

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
    template<class Mapper>
    OutputType *doReverseDeformationBlTyped(const Mapper *map, InternalIndexType newH = -1, InternalIndexType newW = -1)
    {
        InputType *realThis =  static_cast<InputType *>(this);

        if (newH == -1) newH = realThis->getH();
        if (newW == -1) newW = realThis->getW();

        OutputType *toReturn = new OutputType(newH, newW);
        DOTRACE(("Starting transform to %d %d...\n", newW - 1, newH - 1));
        parallelable_for(0, newH - 1, ParallelDoReverseDeformationBl<Mapper>(toReturn, map, realThis));
        return toReturn;
    }

};





class Splain3Interpolator
{
public:

    static double interpolate(double y, double x, G12Buffer * buffer)
    {
        int32_t i = floor(y);
        int32_t j = floor(x);

        CORE_ASSERT_TRUE_P(buffer->isValidCoordBl(y, x),
                ("Invalid coordinate in AbstractContiniousBuffer::elementBl(double y=%lf, double x=%lf) buffer sizes is [%dx%d]",
                   y, x, buffer->w, buffer->h));

        double a = (double)buffer->element(i, j);
        double b = (double)buffer->element(i, CORE_MIN(buffer->w - 1, j + 1));
        double c = (double)buffer->element(CORE_MIN(buffer->h - 1, i + 1), j);
        double d = (double)buffer->element(CORE_MIN(i + 1, buffer->h - 1), CORE_MIN(j + 1, buffer->w - 1));

        double derAY = c - a;
        double derCY = c - a;
        double derBY = d - b;
        double derDY = d - b;
        double derAX = b - a;
        double derBX = b - a;
        double derCX = d - c;
        double derDX = d - c;

        if (i > 0)
        {
            derAY = (c - (double)buffer->element(i - 1, j)) * 0.5;
            derBY = (d - (double)buffer->element(i - 1, CORE_MIN(j + 1, buffer->w - 1))) * 0.5;
        }
        if (i + 2 < buffer->h)
        {
            derCY = ((double)buffer->element(i + 2, j) - a) * 0.5;
            derDY = ((double)buffer->element(i + 2, CORE_MIN(j + 1, buffer->w - 1)) - b) * 0.5;
        }
        if (j > 0)
        {
            derAX = (b - (double)buffer->element(i, j - 1)) * 0.5;
            derCX = (d - (double)buffer->element(CORE_MIN(i + 1, buffer->h - 1), j - 1)) * 0.5;
        }
        if (j + 2 < buffer->w)
        {
            derBX = ((double)buffer->element(i, j + 2) - a) * 0.5;
            derDX = ((double)buffer->element(CORE_MIN(i + 1, buffer->h - 1), j + 2) - c) * 0.5;
        }

        double b0 = 4 * a - 4 * b - 4 * c + 4 * d + 2 * derAX + 2 * derBX
                - 2 * derCX - 2 * derDX + 2 * derAY - 2 * derBY + 2 * derCY - 2 * derDY;
        double b1 = - 6 * a + 6 * b + 6 * c - 6 * d - 3 * derAX - 3 * derBX
                + 3 * derCX + 3 * derDX - 4 * derAY + 4 * derBY - 2 * derCY + 2 * derDY;
        double b2 = 2 * derAY - 2 * derBY;
        double b3 = 2 * a - 2 * b + derAX + derBX;
        double b4 = - 6 * a + 6 * b + 6 * c - 6 * d - 4 * derAX - 2 * derBX
                + 4 * derCX + 2 * derDX - 3 * derAY + 3 * derBY - 3 * derCY + 3 * derDY;
        double b5 = 9 * a - 9 * b - 9 * c + 9 * d + 6 * derAX + 3 * derBX
                - 6 * derCX - 3 * derDX + 6 * derAY - 6 * derBY + 3 * derCY - 3 * derDY;
        double b6 = - 3 * derAY + 3 * derBY;
        double b7 = - 3 * a + 3 * b - 2 * derAX - derBX;
        double b8 = 2 * derAX - 2 * derCX;
        double b9 = - 3 * derAX + 3 * derCX;
        double b10 = 0;
        double b11 = derAX;
        double b12 = 2 * a - 2 * c + derAY + derCY;
        double b13 = - 3 * a + 3 * c - 2 * derAY - derCY;
        double b14 = derAY;
        double b15 = a;

        x -= j;
        y -= i;

        double point = b0 * pow(x, 3) * pow(y, 3) +
               b1 * pow(x, 3) * pow(y, 2) +
               b2 * pow(x, 3) * y +
               b3 * pow(x, 3) +
               b4 * pow(x, 2) * pow(y, 3) +
               b5 * pow(x, 2) * pow(y, 2) +
               b6 * pow(x, 2) * y +
               b7 * pow(x, 2) +
               b8 * x * pow(y, 3) +
               b9 * x * pow(y, 2) +
               b10 * x * y +
               b11 * x +
               b12 * pow(y, 3) +
               b13 * pow(y, 2) +
               b14 * y +
               b15;

        return point;
    }
};

class BilinearInterpolator
{
public:

    static double interpolate(double y, double x, G12Buffer * buffer)
    {
        int32_t i = floor(y);
        int32_t j = floor(x);

        CORE_ASSERT_TRUE_P(buffer->isValidCoordBl(y, x),
            ("Invalid coordinate in AbstractContiniousBuffer::elementBl(double y=%lf, double x=%lf) buffer sizes is [%dx%d]",
               y, x, buffer->w, buffer->h));

        double a = (double)buffer->element(i    ,j    );
        double b = (double)buffer->element(i    ,j + 1);
        double c = (double)buffer->element(i + 1,j    );
        double d = (double)buffer->element(i + 1,j + 1);

        double k1 = x - j;
        double k2 = y - i;

        double result =
         (a * (1 - k1) + k1 * b) * (1 - k2) +
         (c * (1 - k1) + k1 * d) *      k2;
        return result;
    }
};

class PolynomInterpolator
{
public:
    static double interpolate(double y, double x, G12Buffer * buffer)
    {
        int32_t i = floor(y);
        int32_t j = floor(x);

        int w = buffer->getW();
        int h = buffer->getH();

        CORE_ASSERT_TRUE_P(buffer->isValidCoordBl(y, x),
                      ("Invalid coordinate in AbstractContiniousBuffer::elementBl(double y=%lf, double x=%lf) buffer sizes is [%dx%d]",
                       y, x, w, h));

        double a0 = (double)buffer->element(CORE_MAX(0, i - 1)    , CORE_MAX(0, j - 1));
        double a1 = (double)buffer->element(                 i    , CORE_MAX(0, j - 1));
        double a2 = (double)buffer->element(CORE_MIN(i + 1, h - 1), CORE_MAX(0, j - 1));
        double a3 = (double)buffer->element(CORE_MAX(0, i - 1), j);
        double a4 = (double)buffer->element(i, j);
        double a5 = (double)buffer->element(CORE_MIN(i + 1, h - 1), j);
        double a6 = (double)buffer->element(CORE_MAX(0, i - 1),     CORE_MIN(j + 1, w - 1));
        double a7 = (double)buffer->element(i,                      CORE_MIN(j + 1, w - 1));
        double a8 = (double)buffer->element(CORE_MIN(i + 1, h - 1), CORE_MIN(j + 1, w - 1));

        x -= j;
        y -= i;
        double res = a4 +
            (-0.5 * a3 + 0.5 * a5) * x +
            (0.5 * a3 - a4 + 0.5 * a5) * x * x +
            (-0.5 * a1 + 0.5 * a7) * y +
            (0.25 * a0 - 0.25 * a2 - 0.25 * a6 + 0.25 * a8) * y * x +
            (-0.25 * a0 + 0.5 * a1 - 0.25 * a2 + 0.25 * a6 - 0.5 * a7 + 0.25 * a8) * y * x * x +
            (0.5 * a1 - a4 + 0.5 * a7) * y * y +
            (-0.25 * a0 + 0.25 * a2 + 0.5 * a3 - 0.5 * a5 - 0.25 * a6 + 0.25 * a8) * y * y * x +
            (0.25 * a0 - 0.5 * a1 + 0.25 * a2 - 0.5 * a3 + a4 - 0.5 * a5 + 0.25 * a6 -0.5 * a7 + 0.25 * a8) * y * y * x * x;
        return res;
    }
};

} //namespace corecvs

#endif //#ifndef INTERPOLATOR_H_
