#ifndef STEREO_ALIGNER_H_
#define STEREO_ALIGNER_H_

/**
 * \file stereoAligner.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jul 29, 2010
 * \author alexander
 */


#include "core/math/projectiveTransform.h"
#include "core/buffers/correspondenceList.h"
#include "core/math/matrix/matrix33.h"
namespace corecvs {

class StereoTransformation
{
public:
    StereoTransformation(const ProjectiveTransform &_leftTransform,
                         const ProjectiveTransform &_rightTransform) :
                             leftTransform(_leftTransform),
                             rightTransform(_rightTransform){}
    ProjectiveTransform leftTransform;
    ProjectiveTransform rightTransform;

};

/*
class AlignmentCalculationParams
{

};
*/

class StereoAligner
{
public:

    StereoAligner() {}

    static void getAlignmentTransformation(
        const Matrix33 &F,
        Matrix33 *firstTransform,
        Matrix33 *secondTransform,
        Vector3dd z = Vector3dd(0.0,1.0,0.0));

    static void getLateralCorrection(
        Matrix33 *leftTransform,
        Matrix33 *rightTransform,
        CorrespondenceList *list,
        unsigned  threshold = 0 );

    static void getLateralCorrectionFixPoint(
        Matrix33 *leftTransform,
        Matrix33 *rightb,
        Vector2dd fixedPoint );

    static Vector3dd getBestZ(
        const Matrix33 &F,
        const Vector2dd &rect,
        unsigned granularity = 512,
        double *leftDistortions = NULL,
        double *rightDistortions = NULL);

    static double getDistortion(Vector3dd projective, const Vector2dd &rect);

};


} //namespace corecvs
#endif /* STEREO_ALIGNER_H_ */

