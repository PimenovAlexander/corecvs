/**
 * \file stereoAligner.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jul 29, 2010
 * \author alexander
 */

#undef TRACE
#undef DEEP_TRACE

#include <limits>

#include "core/utils/global.h"

#include "core/math/mathUtils.h"
#include "core/math/vector/vector2d.h"
#include "core/math/matrix/matrix.h"
#include "core/rectification/stereoAligner.h"
#include "core/rectification/ransac.h"
#include "core/geometry/line.h"
#include "core/rectification/essentialMatrix.h"
namespace corecvs {

#if 0
/**
 * \brief The calculation of the fundamental matrix of two stereo images.
 * This algorithm uses RANSAC - Ransac.
 *
 * \code
 *             [ a_11   a_12  a_13 ]  [ x']    [ 0 ]
 * [x, y ,1]   [ a_21   a_22  a_23 ]  [ y'] =  [ 0 ]
 *             [ a_31   a_32  a_33 ]  [ 1 ]    [ 0 ]
 * \endcode
 *
 * \f$
 *      \pmatrix{ x & y & 1}
 *      \pmatrix{
 *        F_{1,1} & F_{1,2} & F_{1,3}\cr
 *        F_{2,1} & F_{2,2} & F_{2,3}\cr
 *        F_{3,1} & F_{3,2} & F_{3,3}}
 *      \pmatrix{ x' \cr y' \cr 1} = \pmatrix{ 0 \cr 0 \cr 0}
 * \f$
 *
 *
 * The inliers calculation is as follows:
 *  1. We first get the epipolar line for a left input point
 *
 *  \f[
 *      \pmatrix{ x & y & 1 }
 *      \pmatrix{
 *        F_{1,1} & F_{1,2} & F_{1,3}\cr
 *        F_{2,1} & F_{2,2} & F_{2,3}\cr
 *        F_{3,1} & F_{3,2} & F_{3,3}} = \pmatrix{ a \cr b \cr c }
 *  \f]
 *
 *  Here is the equation of the line - \f$ax' + by' + c = 0\f$. And now we want to know how far is the real right point from the predicted by the current model -
 *
 *   \f[ D = \frac{\left | a x' + b y' + c \right |}{\sqrt{a^2+b^2}}. \f]
 *
 *  When we get the distance we can say if the point is to far from the model and is not the inlier or close enough.
 *
 *
 *
 * \param[in, out] list      the list of input pairs
 * \param[in] trySize        the size of the minimal set for RANSAC. Should not be less then 8. Typically should be around 9-11.
 * \param[in] maxIterations  maximum number of iterations for RANSAC
 * \param[in] threshold      a threshold to consider the pair satisfies the model
 *
 * \return the fundamental matrix.
 *
 */
#endif

/**
 *
 * This function implements the simple rectification.
 * It is not optimal in any sense. This rectification is universal for both essential and
 * fundamental matrix. In essential case it could be done simpler.
 *
 * \f[ {p'}^T F p = 0 \f]
 *
 * We actually want to transform \f$p\f$ and \f$p'\f$ to \f$q\f$ and \f$q'\f$ to satisfy
 * the following condition:
 *
 * \f[ {q'}^T I_{\times} q = 0,~~where~~I_{\times} =
 *  \pmatrix {
 *    0 & 0 &  0 \cr
 *    0 & 0 & -1 \cr
 *    0 & 1 &  0 }
 *
 * \f]
 *
 * This will be done, by presenting
 *
 * \f[ F = H_{1}^T I_{\times} H_{2} \f]
 * \f[ {p'}^T H_{1}^T I_{\times} H_{2} p = 0 \f]
 * \f[ {\underbrace{(H_{1} p')}_{q'}}^T I_{\times}  \underbrace{(H_{2} p)}_{q} = 0 \f]
 *
 *
 *
 * The main idea is selecting two connected directions in both cameras
 * This function does the following decomposition
 * \f[ H_{1} = R' P' ~~ and ~~ H_{2} = R P \f]
 *
 * \f[ F = (R' P')^T I_{\times} R P \f]
 *
 * Let \f$E_1\f$ be the left nullspace of \f$F\f$, and \f$E_2\f$ the second one.
 * Left nullspace vector is the direction to the second optical center in the first
 * coordinate system.
 *
 * Quite obviously the common rectified plane should be parallel to this line.
 * So we need one more constraint to define this plane and the ability to calculate this
 * direction in both First and Second coordinate systems.
 *
 *
 * Let's choose arbitary vector z (most probably it would be close to Y direction)
 * And have a look at following vectors
 *
 *
 * \f[ w^{3D}_1 = z \times {E_1} \f]
 * \f[ w^{3D}_2 = F z \f]
 *
 * \f$w^{3D}_1\f$ and \f$w^{3D}_2\f$ are the normal vectors to the same plane but in first and
 * second coordinate system respectively. And this plane will be paralle to \f$E_1\f$ in first
 * coordinate system and to \f$E_2\f$ in the second.
 * Let's prove this step by step.
 *
 * 1. \f$w^{3D}_1\f$ is ortogonal both to \f$z\f$ and \f$E_1\f$ - obvious
 * 2. Consider the point in space ortogonal to \f$w^{3D}_1\f$. It can be decomposed by \f$z\f$ and \f$E_1\f$
 * \f$a=\alpha z + \beta {E_1} \f$
 *
 *
 * \f[ F =
 * {\left(
 *
 *  \pmatrix {
 *      {w_2}_y f_{22} - f_{12} & f_{02} - {w_2}_x f_{22} &  0 \cr
 *      {w_2}_x f_{22} - f_{02} & {w_2}_y f_{22} - f_{12} &  0 \cr
 *                 0              &               0           &  1 }
 *
 *  \pmatrix {
 *      1     &   0     &  0 \cr
 *      0     &   1     &  0 \cr
 *    {w_1}_x & {w_1}_y &  1 }
 *
 *  \right)}^T
 *
 *  \pmatrix {
 *    0 & 0 &  0 \cr
 *    0 & 0 & -1 \cr
 *    0 & 1 &  0 }
 *
 *  \pmatrix {
 *      f_{21} - {w_1}_y f_{22} & {w_1}_x f_{22} - f_{20} &  0      \cr
 *      f_{20} - {w_1}_x f_{22} & f_{21} - {w_1}_y f_{22} &  f_{22} \cr
 *                 0              &             0             &  1 }
 *
 *  \pmatrix {
 *       1   &    0     &  0 \cr
 *       0   &    1     &  0 \cr
 *   {w_2}_x & {w_2}_y &  1 }
 * \f]
 *
 * The rotational part can be reduced to :
 *
 * \f[
 *      {R'}^T I_{\times} R =
 *      \pmatrix{
 *                   0         &             0           & f_{02} - f_{22} {w_2}_x \cr
 *                   0         &             0           & f_{12} - f_{22} {w_2}_y \cr
 *       f_{20}-f_{22} {w_1}_x & f_{21} - f_{22} {w_1}_y & f_{22}}
 * \f]
 *
 *
 * If one wants to apply the transformations to input images. F is a transform computed for first pair
 * of images. The following formulas could be used:
 *
 *
 * \f{eqnarray*}
 *  {p'}^T F p &=& 0 \\
 *     q' &=& A p' \\
 *     q  &=& B p  \\
 *  {q'}^T G q &=& (A p')^T G B p = {p'}^T A^T G B p\\
 *  G &=& {A^T}^{-1} F B^{-1}
 * \f}
 *
 * This could be presented as:
 *
 * \f[ G = {A^T}^{-1} F B^{-1} = {A^T}^{-1} H_{1}^T I_x H_{2} B^{-1} \f]
 *
 * \f[ G = \underbrace{{A^T}^{-1} H_{1}^T}_{H_{G1}^T} I_x \underbrace{H_{2} B^{-1}}_{H_{G2}} \f]
 *
 * Finally align transformations and there inverses are
 *
 * \f{eqnarray*}
 *   H_{G1}      &=& ({A^T}^{-1} H_{1}^T)^T = H_{1} A^{-1} \\
 *   H_{G2}      &=& H_{2} B^{-1} \\
 *   H_{G1}^{-1} &=& A H_{1}^{-1} \\
 *   H_{G2}^{-1} &=& B H_{2}^{-1}
 *
 * \f}
 *
 * \param[in]  F               input fundamental or essential matrix
 * \param[out] firstTransform   output left transform \f$H_{1}\f$
 * \param[out] secondTransform  output right transform \f$H_{2}\f$
 * \param[in]  z               free vector \f$z\f$
 *
 */

#define TRACE

void StereoAligner::getAlignmentTransformation(
        const Matrix33 &F,
        Matrix33 *firstTransform,
        Matrix33 *secondTransform,
        Vector3dd z)
{
    Vector3dd E1;
    Vector3dd E2;
    EssentialMatrix(F).nullspaces(&E1, &E2);


#ifdef TRACE
    printf("Fundamental Matrix:\n");
    F.print();
    printf("\n");
    printf("Epipoles:\n");
    E1.print();
    printf("\n");
    E2.print();
    printf("\n");
    printf("Z Direction:\n");
    z.println();
#endif

    /// \todo TODO In fact this vector should be guessed
    ///
    Vector3dd w1 = z ^ E1;
    Vector3dd w2 = z * F;

#ifdef TRACE
    printf("Projective correctives:\n");
    w1.print();
    printf("\n");
    w2.print();
    printf("\n");
#endif

    Vector2dd w1_2d = w1.project();
    Vector2dd w2_2d = w2.project();

#ifdef TRACE
    printf("Normalized correctives:\n");
    w1_2d.println();
    w2_2d.println();
    printf("Checks:\n");
    printf("Dot product for 1st corrector: %lg (should be zero)\n",
            E1 & Vector3dd(w1_2d.x(), w1_2d.y(), 1.0));
    printf("Dot product for 2nd corrector: %lg\n",
            E2 & Vector3dd(w2_2d.x(), w2_2d.y(), 1.0));
#endif

    Matrix33 projectivePartFirst  = Matrix33::Projective(w1_2d);
    Matrix33 projectivePartSecond = Matrix33::Projective(w2_2d);

    double wc = 0;
    double a, b;

    a = w2_2d.y() * F.a(2,2) - F.a(1,2);
    b = F.a(0,2) - w2_2d.x() * F.a(2,2);
    Matrix33 rotationalPartFirst(
         a,  b,  0,
        -b,  a,  wc,
         0,  0,  1 );

    a = F.a(2,1) - w1_2d.y() * F.a(2,2);
    b = w1_2d.x() * F.a(2,2) - F.a(2,0);
    Matrix33 rotationalPartSecond(
         a,  b,  0,
        -b,  a,  F.a(2,2) + wc,
         0,  0,  1 );

    if (a < 0)
    {   rotationalPartFirst = -rotationalPartFirst;
        rotationalPartFirst.a(2,2) = 1.0;
        rotationalPartSecond = -rotationalPartSecond;
        rotationalPartSecond.a(2,2) = 1.0;
        printf("Rotative negation done\n");
    }

#ifdef TRACE
    printf("Rotator1:\n");
    rotationalPartFirst.print();
    printf("Rotator2:\n");
    rotationalPartSecond.print();

    printf("Projector1:\n");
    projectivePartSecond.print();
    printf("Projector2:\n");
    projectivePartFirst.print();
#endif


    Matrix33 first  =  rotationalPartFirst  * projectivePartFirst;
    Matrix33 second =  rotationalPartSecond * projectivePartSecond;


#ifdef ASSERTS
    Matrix33 Ix(
             0,  0,  0,
             0,  0, -1,
             0,  1,  0 );

    Matrix33 Fprim = first.transposed() * Ix * second;

    printf("Recollect\n");
    Fprim.print();
    printf("\n");
    printf("Old Matrix was\n");
    F.print();
    printf("\n");

    if (Fprim.notTooFar(F, 1e-5) || Fprim.notTooFar(-F, 1e-5)) {
        SYNC_PRINT(("Matrix reconstruction ok\n"));
    } else {
        SYNC_PRINT(("Matrix reconstruction failed\n"));
    }

#endif // ASSERTS

    double det1 = rotationalPartFirst .det();
    double det2 = rotationalPartSecond.det();

    printf("rotationalPartLeft .det() %lg\n", det1);
    printf("rotationalPartRight.det() %lg\n", det2);

    double scaleFactor = det1 > det2 ? det1 : det2;
    scaleFactor = sqrt(scaleFactor);


    printf("Scale factor %lg\n", scaleFactor);
    Matrix33 scale =  Matrix33::Scale2(1.0 / scaleFactor, 1.0 / scaleFactor);

    *firstTransform  = scale * first;
    *secondTransform = scale * second;

#if 0
    printf("Final L %lg:\n", detL);
    leftTransform->print();
    printf("Final R %lg:\n", detR);
    rightTransform->print();
#endif

}

Vector3dd StereoAligner::getBestZ(const Matrix33 &F, const Vector2dd &rect, unsigned granularity, double *leftDistortions, double *rightDistortions)
{
    Vector3dd E1;
    Vector3dd E2;
    EssentialMatrix(F).nullspaces(&E2, &E1);

    SYNC_PRINT(("StereoAligner::getBestZ(): called\n"));

    double minsum = numeric_limits<double>::max();
    double minalpha  = 0;
    Vector3dd bestZ = Vector3dd::OrtY();

    for (unsigned i = 0; i < granularity; i++)
    {
        double alpha = i / (double)granularity * (M_PI * 2.0);
        Vector3dd z = Vector3dd(0.0, cos(alpha), sin(alpha));

        Vector3dd w1 = z ^ E1;
        Vector3dd w2 = F * z;

        w1 = w1.normalisedProjective();
        w2 = w2.normalisedProjective();

        double distLeft  = getDistortion(w1, rect);
        double distRight = getDistortion(w2, rect);
        if (leftDistortions != NULL)
            leftDistortions[i] = distLeft;
        if (rightDistortions != NULL)
            rightDistortions[i] = distRight;

        // CORE_ASSERT_TRUE(distLeft > 0 && distRight > 0, "Internal error");

        if (distLeft + distRight <  minsum)
        {
            minsum = distLeft + distRight;
            minalpha = alpha;
            bestZ = z;
        }
        // cout << "Alpha: " << radToDeg(alpha) << "deg weight " << (distLeft + distRight) << " dl:" << distLeft << " dr:" << distRight << std::endl;
    }

    cout << "StereoAligner::getBestZ():Best Alpha: " << radToDeg(minalpha) << endl;
    return bestZ;
}

/*
void Rectificator::normaliseScale(
        Matrix33 *leftTransform,
        Matrix33 *rightTransform)
{
    double detL = rotationalPartLeft .det();
    double detR = rotationalPartRight.det();
    double scaleFactor = detL > detR ? detL : detR;
    scaleFactor = sqrt(scaleFactor);

#ifdef TRACE
    printf("Rotator1 %lg:\n", detL);
    rotationalPartLeft.print();
    printf("Rotator2 %lg:\n", detR);
    rotationalPartRight.print();
#endif



    printf("Scale factor %lg\n", scaleFactor);
    Matrix33 scale =  Matrix33::Scale2(1.0 / scaleFactor,1.0 / scaleFactor);

    *leftTransform  = scale * left;
    *rightTransform = scale * right;

#ifdef TRACE
    printf("Final L %lg:\n", detL);
    leftTransform->print();
    printf("Final R %lg:\n", detR);
    rightTransform->print();
#endif
}
*/
void StereoAligner::getLateralCorrection(
        Matrix33 *leftTransform,
        Matrix33 *rightTransform,
        CorrespondenceList *list,
        unsigned threshold
        )
{
    CORE_ASSERT_TRUE(list->size() != 0, "Input list should not ne empty");
    double min = numeric_limits<double>::max();
    double max = numeric_limits<double>::min();
    vector<double> sortedLaterals;
    sortedLaterals.reserve(list->size());

//    double totalMisrectify = 0;
    for (unsigned i = 0; i < list->size(); i++)
    {
        Vector2dd left  = (*leftTransform) * list->at(i).start;
        Vector2dd right = (*rightTransform) * list->at(i).end;
//        totalMisrectify += fabs(left.y() - right.y());
        double diff = left.x() - right.x();
        if (diff < min) {
            diff = min;
        }

        if (diff > max) {
            diff = max;
        }

        sortedLaterals.push_back(diff);
    }

    sort(sortedLaterals.begin(), sortedLaterals.end());
    double shift;
    if (threshold > sortedLaterals.size())
        shift = max;
    else
        shift = sortedLaterals[sortedLaterals.size() - 1 - threshold];
    (*leftTransform)  = Matrix33::ShiftProj( shift / 2.0, 0.0) * (*leftTransform);
    (*rightTransform) = Matrix33::ShiftProj(-shift / 2.0, 0.0) * (*rightTransform);



 //   DOTRACE(("Total misrectify is %lf", totalMisrectify));

}

void StereoAligner::getLateralCorrectionFixPoint(
        Matrix33 *leftTransform,
        Matrix33 *rightTransform,
        Vector2dd fixedPoint)

{
    Vector2dd fixedPointShift = (*leftTransform) * fixedPoint;
    Matrix33 backShift = Matrix33::ShiftProj(fixedPoint - fixedPointShift);

    *leftTransform = backShift * (*leftTransform);
    *rightTransform = backShift * (*rightTransform);
}

/**
 *
 *
 *
 **/
double StereoAligner::getDistortion(Vector3dd projective, const Vector2dd &rect)
{
    double h = rect.y();
    double w = rect.x();

    Matrix33 PPt = w * h * (1 / 12.0) *
        Matrix33(
            w * w - 1,      0.0,  0.0,
                  0.0, h * h -1,  0.0,
                  0.0,      0.0,  0.0
        );

    Vector3dd center = Vector3dd( w * 0.5, h * 0.5, 1.0);

    Matrix33 ppc = Matrix33::VectorByVector(center, center);

    double distortion =
            ((projective * PPt) & projective) /
            ((projective * ppc) & projective);

    return distortion;
}


} //namespace corecvs

