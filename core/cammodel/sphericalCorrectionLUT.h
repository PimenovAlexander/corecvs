#ifndef SPHERICALCORRECTIONLUT_H_
#define SPHERICALCORRECTIONLUT_H_
/**
 * \file sphericalCorrectionLUT.h
 * \brief Add Comment Here
 *
 * \date Feb 22, 2011
 * \author alexander
 */


#include <vector>

#include "core/buffers/abstractBuffer.h"
#include "core/math/vector/vector2d.h"
#include "core/buffers/deformMap.h"

namespace corecvs {

using std::vector;

class GenericRadiusDeformer
{
public:
    double transformRadiusSquare(double /*rSquare*/) const
    {
        return 0.0;
    };
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
        double r2new = radiusDeformer->transformRadiusSquare(r2);
        return center + r2new * delta;
    }

};

class RadiusCorrectionLUT : public GenericRadiusDeformer
{
public:
    const vector<Vector2dd> *LUT;
    RadiusCorrectionLUT( const vector<Vector2dd> *_LUT) : LUT(_LUT) {};


    inline double transformRadiusSquare(double rSquare) const
    {
        unsigned n = 1;
        while ((n < LUT->size() - 1) && (LUT->at(n + 1).x() < rSquare))
        {
           n++;
        }

        double x0 = LUT->at(n - 1).x();
        double x1 = LUT->at(n    ).x();
        double y0 = LUT->at(n - 1).y();
        double y1 = LUT->at(n    ).y();

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
         return (1.0 / (x1 - x0)) * ((y1 - y0) * rSquare + y0 * x1 - y1 * x0);
         //return ((y1 - y0) / (x1 - x0)) * (rSquare - x0) + y0;
    };
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

