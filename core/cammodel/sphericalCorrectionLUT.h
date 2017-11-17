#ifndef SPHERICALCORRECTIONLUT_H_
#define SPHERICALCORRECTIONLUT_H_
/**
 * \file sphericalCorrectionLUT.h
 * \brief Add Comment Here
 *
 * \date Feb 22, 2011
 * \author alexander
 */

#include <type_traits>
#include <vector>

#include "core/buffers/abstractBuffer.h"
#include "core/math/vector/vector2d.h"
#include "core/buffers/deformMap.h"

namespace corecvs {

using std::vector;


template<class RadiusDeformer>
class SphericalCorrectionSq : public DeformMap<int32_t, double>
{
public:
    RadiusDeformer *radiusDeformer;
    Vector2dd center;

    SphericalCorrectionSq(const Vector2dd &_center, RadiusDeformer *_radiusDeformer) :
        radiusDeformer(_radiusDeformer),
        center(_center)
    {}

    Vector2d<double> map(const int32_t &y, const int32_t &x) const
    {
        return map(Vector2dd(x,y));
    }

    Vector2d<double> map(const Vector2d<double> &point) const
    {
        Vector2dd delta = point - center;
        double r2 = delta.sumAllElementsSq();
        double r2new = radiusDeformer->transformRadiusSquare(r2);
        return center + r2new * delta;
    }


};

template<class RadiusDeformer>
class SphericalCorrection : public DeformMap<int32_t, double>
{
public:
    RadiusDeformer *radiusDeformer;
    Vector2dd center;

    SphericalCorrection(const Vector2dd &_center, RadiusDeformer *_radiusDeformer) :
        radiusDeformer(_radiusDeformer),
        center(_center)
    {}

    Vector2d<double> map(const int32_t &y, const int32_t &x) const
    {
        return map(Vector2dd(x,y));
    }

    Vector2d<double> map(const Vector2d<double> &point) const
    {
        Vector2dd delta = point - center;
        double r2 = delta.sumAllElementsSq();
        double r2new = radiusDeformer->transformRadius(sqrt(r2));
        return center + r2new * delta;
    }

};

class RadiusCorrectionLUTSq
{
public:
    const vector<Vector2dd> *LUT;
    RadiusCorrectionLUTSq( const vector<Vector2dd> *_LUT) : LUT(_LUT) {}


    inline double transformRadiusSquare(double rSquare) const
    {
        unsigned n = 1;

        for (; n + 2 < LUT->size(); n++)
        {
            if (LUT->operator[](n + 1).x() > rSquare)
                break;
        }

        double x0 = LUT->operator[](n    ).x();
        double y0 = LUT->operator[](n    ).y();
        double x1 = LUT->operator[](n + 1).x();
        double y1 = LUT->operator[](n + 1).y();

         /**
          * Liner interpolation
          *
          *     1
          *  ------- * ((y2 - y1) * r^2 + y1 * x2 - y2 * x1) =
          *  x2 - x1
          *
          *
          *  y2 - y1
          *  ------- * (r^2 - x1) + y1
          *  x2 - x1
          **/
         //return (1.0 / (x1 - x0)) * ((y1 - y0) * rSquare + y0 * x1 - y1 * x0);
         return lerp(y0, y1, rSquare, x0, x1);
         //return ((y1 - y0) / (x1 - x0)) * (rSquare - x0) + y0;
    }
};


class RadiusCorrectionLUT
{
public:
    vector<double> LUT;
    static const size_t LUT_SIZE_LIMIT = 100000;

    RadiusCorrectionLUT( /*const vector<Vector2dd> &_LUT*/)
    {
/*       double last = sqrt(_LUT.back().x());
       LUT.reserve(last + 1);
       for (int i = 0; i < last; i++)
       {
           double val = i;
           double rSquare = val * val;
           size_t n = 1;
           for (; n + 2 < _LUT.size(); n++)
           {
               if (_LUT[n + 1].x() > rSquare)
                   break;
           }
           double koef = lerp(_LUT[n].y(), _LUT[n + 1].y(), rSquare, _LUT[n].x(), _LUT[n + 1].x());
           LUT.push_back(koef);
       }*/
    }

    static RadiusCorrectionLUT FromSquareToSquare(const vector<Vector2dd> &_LUT)
    {
        RadiusCorrectionLUT result;
        size_t last = sqrt(_LUT.back().x());
        last = std::min(LUT_SIZE_LIMIT, last);

        result.LUT.reserve(last + 1);
        for (size_t i = 0; i < last; i++)
        {
            double val = i;
            double rSquare = val * val;
            size_t n = 1;
            for (; n + 2 < _LUT.size(); n++)
            {
                if (_LUT[n + 1].x() > rSquare)
                    break;
            }
            double koef = lerp(_LUT[n].y(), _LUT[n + 1].y(), rSquare, _LUT[n].x(), _LUT[n + 1].x());
            result.LUT.push_back(koef);
        }
        return result;
    }

    static RadiusCorrectionLUT FromAngleAndProjection(const vector<Vector2dd> &_AngleLUT, double mmToPixel, double focalInPx)
    {
        RadiusCorrectionLUT result;
        vector<Vector2dd> lut;
        lut.reserve(_AngleLUT.size());

        /* We create a temporary lut that maps pixels to pixels */
        for (size_t i = 0; i < _AngleLUT.size(); i++)
        {
            double original = tan(degToRad(_AngleLUT[i].x())) * focalInPx;
            double result   = _AngleLUT[i].y() * mmToPixel;

            double koef = result / original;
            // cout << "RadiusCorrectionLUT FromAngleAndProjection(): Shift " <<  original << "  -> " << result  << "(" << koef << ")" << endl;
            lut.push_back(Vector2dd(original, koef));
        }

        /* Now we will create a dense lut that works for every integer distance */
        /* This makes lookup fast */
        size_t last = 0;
        for (size_t i = 0; i < lut.size(); i++)
        {
            if (lut[i].x() > last)  last = lut[i].x();
        }
        last = std::min(LUT_SIZE_LIMIT, last);

        result.LUT.reserve(last + 1);

        SYNC_PRINT(("RadiusCorrectionLUT FromAngleAndProjection(): %d table size\n", (int)last));
        for (size_t i = 0; i < last; i++)
        {
            double val = i;
            size_t n = 1;
            for (; n + 2 < lut.size(); n++)
            {
                if (lut[n + 1].x() > val)
                    break;
            }
            double koef = lerp(lut[n].y(), lut[n + 1].y(), val, lut[n].x(), lut[n + 1].x());
            result.LUT.push_back(koef);
        }
        return result;
    }

    inline double transformRadius(double radius) const
    {
        if (radius < LUT.size() - 1 && radius > 0)
        {
            int n = radius;
            double y0 = LUT[n    ];
            double y1 = LUT[n + 1];
            return lerp(y0, y1, radius, n, n+1);
        }

        return 0;
    }
};

class SphericalCorrectionLUTSq : public SphericalCorrectionSq<RadiusCorrectionLUTSq>
{
public:
    SphericalCorrectionLUTSq(const Vector2dd &_center, RadiusCorrectionLUTSq *_radiusDeformer) :
        SphericalCorrectionSq<RadiusCorrectionLUTSq>(_center, _radiusDeformer)
     {

     }
};

class SphericalCorrectionLUT : public SphericalCorrection<RadiusCorrectionLUT>
{
public:
    SphericalCorrectionLUT(const Vector2dd &_center, RadiusCorrectionLUT *_radiusDeformer) :
        SphericalCorrection<RadiusCorrectionLUT>(_center, _radiusDeformer)
     {

     }
};


} //namespace corecvs
#endif /* SPHERICALCORRECTIONLUT_H_ */

