#ifndef RADIALCORRECTION_H_
#define RADIALCORRECTION_H_

/**
 * \file radialCorrection.h
 * \brief Header file for the radial lens distortion correction primitives
 *
 * \ingroup cppcorefiles
 *
 * \date Jun 22, 2010
 * \author alexander
 */

#include "geometry/ellipticalApproximation.h"
#include "alignment/lensDistortionModelParameters.h"
#include "utils/global.h"
#include "math/vector/vector2d.h"
#include "buffers/g12Buffer.h"
#include "buffers/rgb24/rgb24Buffer.h"
#include "function/function.h"
#include "math/levenmarq.h"

namespace corecvs {

class DisplacementBuffer;

class RadialCorrection : public DeformMap<int32_t, double>, public FunctionArgs
{
public:
    explicit RadialCorrection(const LensDistortionModelParameters &params = LensDistortionModelParameters());

    virtual ~RadialCorrection();

    inline Vector2dd map(const Vector2dd &v, bool fromUndistorted = false) const
    {
        return fromUndistorted ? mapFromUndistorted(v) : mapToUndistorted(v);
    }
    inline Vector2dd map(int y, int x, bool fromUndistorted = false) const
    {
        return map(Vector2dd(x, y), fromUndistorted);
    }
    inline Vector2dd map(double y, double x, bool fromUndistorted = false) const
    {
        return map(Vector2dd(x, y), fromUndistorted);
    }
    inline Vector2dd mapToUndistorted(Vector2dd const & v) const
    {
        return mParams.mapBackward(v);
    }
    inline Vector2dd mapFromUndistorted(Vector2dd const & v) const
    {
        return mParams.mapForward(v);
    }


    inline Vector2dd mapFromUndistorted(int y, int x) const
    {
        return mapFromUndistorted((double)y, (double)x);
    }
    inline Vector2dd mapToUndistorted(int y, int x) const
    {
        return mapToUndistorted((double)y, (double)x);
    }

    /**
     * \brief Apply the lens distortion correction.
     *
     * For details see LensDistortionModelParameters
     *
     * \param y  y position of the point to be mapped
     * \param x  x position of the point to be mapped
     * \return A corrected buffer
     *
     **/
    inline Vector2dd mapToUndistorted(double y, double x) const
    {
        return mapToUndistorted(Vector2dd(x, y));
    }

    Vector2dd mapFromUndistorted(double y, double x) const
    {
        return mapFromUndistorted(Vector2dd(x, y));
    }

    /**/
    virtual void operator()(const double in[], double out[])
    {
        Vector2dd result = mapToUndistorted(in[1], in[0]);
        out[1] = result.y();
        out[0] = result.x();
    }


    //G12Buffer *correctLens(G12Buffer *input);
    G12Buffer   *doCorrectionTransform(G12Buffer *inputBuffer);
    RGB24Buffer *doCorrectionTransform(RGB24Buffer *inputBuffer);
    DisplacementBuffer getUndistortionTransformation(const Vector2dd &undistortedSize, const Vector2dd &distortedSize, const double step = 0.25, bool useLM = false);

    //Vector2dd getCorrectionForPoint(Vector2dd input);
    LensDistortionModelParameters mParams;


    Vector2dd center() const
    {
        return Vector2dd(mParams.principalX(), mParams.principalY());
    }


    RadialCorrection invertCorrection   (int h, int w, int step);

#if 0
    RadialCorrection invertCorrectionLSE(int h, int w, int step);
#endif

    EllipticalApproximation1d compareWith(const RadialCorrection &other, int h, int w, int steps);

    /* */
    void getCircumscribedImageRect(const int32_t &x1, const int32_t &y1, const int32_t &x2, const int32_t &y2,
                                   Vector2dd &min, Vector2dd &max)
    {
        mParams.getCircumscribedImageRect(Vector2dd(x1, y1), Vector2dd(x2, y2), min, max);
    }

    void getInscribedImageRect(const int32_t &x1, const int32_t &y1, const int32_t &x2, const int32_t &y2,
                               Vector2dd &min, Vector2dd &max)
    {
        mParams.getInscribedImageRect(Vector2dd(x1, y1), Vector2dd(x2, y2), min, max);
    }
};


} //namespace corecvs
#endif /* RADIALCORRECTION_H_ */

