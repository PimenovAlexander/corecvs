#ifndef ALIGN_NONLINEAR_H_
#define ALIGN_NONLINEAR_H_
/**
 * \file align_nonlinear.h
 * \brief A header for align_nonlinear.c
 *
 *
 * \ingroup corefiles
 * \date May 20, 2009
 * \author Alexander Pimenov
 */

#ifdef __cplusplus
    extern "C" {
#endif

#include "g12buffer.h"
#include "clegacy/math/geometry.h"

#ifdef DEPRICATED
#include "displacement.h"
#endif


/**
 * \brief This structure holds the parameters to correct the image.
 *
 * The intrinsic correction parameters form
 * http://www.vision.caltech.edu/bouguetj/calib_doc/papers/heikkila97.pdf
 *
 * Here are the correction formulas
 *  \f{eqnarray*}
 *  \pmatrix{dx \cr dy} &=& \pmatrix{x \cr y} - \pmatrix{x_c \cr y_c} \\
 *                    r &=& \sqrt{dx^2 + dy^2} \\
 *                 r_{corr} &=& k_1 r^2 + k_2 r^4 \\
 *
 *   \pmatrix{x_{coor}^t \cr y_{coor}^t} &=&
 *   \pmatrix{p_1 dx dy + p_2 (r^2 + 2 dx^2)\cr p_1 (r^2 + 2 dy^2) + p_2 dx dy} \\
 *
 *
 *   \pmatrix{x \cr y} &=&
 *   \pmatrix{x_0 \cr y_0} +
 *   \pmatrix{dx \cr dy} * r_{corr} +
 *   \pmatrix{x_{coor}^t \cr y_{coor}^t}
 *
 *   \f}
 *
 *   for more details please read the code of getCorrectionForPoint() or read the Heikkila paper
 *
 */
typedef struct LensDistortionModelParameters_TAG {
    double k1;       /**< Second order radial correction coefficient - \f$k_1\f$*/
    double k2;       /**< Fourth order radial correction coefficient - \f$k_2\f$*/

    double p1;       /**< First tangent correction coefficient - \f$p_1\f$*/
    double p2;       /**< Second tangent correction coefficient - \f$p_2\f$*/

    vector2Du16 center; /**< The center of the distortion \f$(x_c,y_c)\f$*/
} LensDistortionModelParameters;



G12Buffer *correctLens (G12Buffer *input, LensDistortionModelParameters* params);


#define PARAMETER_NUM 16
void CameraModel(double input[], double X, double Y, double Z, double *x, double *y);

#ifdef DEPRICATED
DisplacementBuffer *displacementBufferFromInverseCamera(LensDistortionModelParameters *params, int h, int w);
#endif


#ifdef __cplusplus
    } //     extern "C"
#endif
#endif /* ALIGN_NONLINEAR_H_ */

