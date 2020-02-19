#ifndef HOMOGRAPHYRECONSTRUCTOR_H_
#define HOMOGRAPHYRECONSTRUCTOR_H_
/**
 * \file homographyReconstructor.h
 * \brief Add Comment Here
 *
 * \date Jun 26, 2011
 * \author alexander
 */
#include <iostream>
#include <vector>

#include "math/vector/vector2d.h"
#include "math/matrix/matrix33.h"
#include "math/matrix/matrix.h"
#include "math/matrix/matrixOperations.h"
#include "function/function.h"
#include "geometry/line.h"
#include "geometry/polygons.h"
#include "buffers/correspondenceList.h"
#include "filters/newstyle/newStyleBlock.h"
#include "cameracalibration/calibrationLocation.h"
#include "xml/generated/homographyAlgorithm.h"
#include "xml/generated/homorgaphyReconstructorBlockBase.h"

namespace corecvs {

using std::vector;
using std::sqrt; /* < ugly but otherwise we would intrude into std:: namespace*/

typedef PrimitiveCorrespondence<Vector2dd, Line2d> CorrespondencePointLine;
typedef PrimitiveCorrespondence<Vector2dd, Segment2d> CorrespondencePointSegment;


class HomographyReconstructor
{
public:
    vector<Correspondence> p2p;
    vector<CorrespondencePointLine> p2l;
    vector<CorrespondencePointSegment> p2s;

public:
    bool trace = false;

    HomographyReconstructor();

    void addPoint2PointConstraint  (const Vector2dd &from, const Vector2dd &to);
    void addPoint2PointConstraint  (const Correspondence &correspondence);

    void addPoint2LineConstraint   (const Vector2dd &from, const Line2d &line);
    void addPoint2SegmentConstraint(const Vector2dd &from, const Segment2d &line);

    void reset(void);

    bool hasEnoughtConstraints();

    double getCostFunction(Matrix33 &input, double out[] = NULL);
    int  getConstraintNumber();

    friend ostream & operator << (ostream &out, const HomographyReconstructor &reconstructor);

    /**
     *  This is a common entry point
     **/
    Matrix33 getBestHomography(const HomographyAlgorithm::HomographyAlgorithm &method = HomographyAlgorithm::ML_AFTER_LSE);

    /**
     *  Function was introduced for testing. Usage is discouraged. Use getBestHomographyLSE1
     **/
    Matrix33 getBestHomographyLSE(void);

    /**
     *  This method uses SVD of (A^T A) matrix to solve
     **/
    Matrix33 getBestHomographyLSE1(void);
    /**
     *  This method uses inverse SVD to solve
     **/
    Matrix33 getBestHomographyLSE2(void);


    Matrix33 getBestHomographyLM(Matrix33 guess = Matrix33(1.0));
    Matrix33 getBestHomographyLSEandLM();


    void normalisePoints(Matrix33 &transformLeft, Matrix33 &transformRight);

    Matrix getJacobian(const Matrix33 &H) const;

    virtual ~HomographyReconstructor();

private:
    void addPoint2PointConstraintLSE(
            Matrix &A,
            Matrix &B,
            int num,
            const Vector2dd &from, const Vector2dd &to);

    void addPoint2LineConstraintLSE(
            Matrix &A,
            Matrix &B,
            int num,
            const Vector2dd &from, const Line2d &line);

    void addPoint2LineConstraintLSEUnif(
            Matrix &A,
            int num,
            const Vector2dd &from, const Line2d &line);

    void addPoint2PointConstraintLSEUnif(
            Matrix &A,
            int num,
            const Vector2dd &from, const Vector2dd &to);

    /* Cost function related stuff */

    /**
     * This class reprents the function that stores
     * the model as a 3 by 3 matrix with lower left element fixed to zero
     *
     **/
    class CostFunction : public FunctionArgs {
    public:
        HomographyReconstructor *reconstructor;
        CostFunction(HomographyReconstructor *_reconstructor) : FunctionArgs(8, _reconstructor->getConstraintNumber()), reconstructor(_reconstructor) {}

        virtual void operator()(const double in[], double out[]);
        Matrix getJacobian(const double in[], double dlta=1e-7);
    };

    /**
     * This class reprents the function that stores
     * the model as a meaningful decomposition of homography matrix
     *
     *  <ol>
     *    <li>First rotaton around current zero</li>
     *    <li>Then shift</li>
     *    <li>Uniform scale</li>
     *    <li>Projective transform</li>
     *  </ol>
     *
     *  \f[ H =
     *       \pmatrix{
     *             1  &  0   & 0\cr
     *             0  &  1   & 0\cr
     *            v_5 &  v_6 & 1
     *        }
     *       \pmatrix{
     *            v_4 &  0  & 0\cr
     *             0  & v_4 & 0\cr
     *             0  &  0  & 1
     *        }
     *       \pmatrix{
     *            1 & 0 & v_2\cr
     *            0 & 1 & v_3\cr
     *            0 & 0 & 1
     *        }
     *      \pmatrix{
     *         cos(v_1) & sin(v_1) & 0\cr
     *        -sin(v_1) & cos(v_1) & 0\cr
     *            0     &   0      & 1
     *        }
     *  \f]
     *
     **/
    class CostFunctionWize : public FunctionArgs {
    public:
        HomographyReconstructor *reconstructor;
        CostFunctionWize(HomographyReconstructor *_reconstructor) : FunctionArgs(6,_reconstructor->getConstraintNumber()), reconstructor(_reconstructor) {}

        virtual void operator()(const double in[], double out[]);
        Matrix33 matrixFromState(const double in[]);
    };


public:
    static CameraLocationData getCameraFromHomography(const Matrix33 &K, const Matrix33 &H);
    static Affine3DQ          getAffineFromHomography(const Matrix33 &K, const Matrix33 &H);



public:
    /**
     * Generic cost function
     *
     * We should find a better place for this
     **/
    template<typename DoubleType>
    void genericCostFunction(const DoubleType in[], DoubleType out[])
    {
        FixMatrixFixed<DoubleType, 3, 3> H;
        H.fillWithArgs(
                in[0], in[1], in[2],
                in[3], in[4], in[5],
                in[6], in[7], DoubleType(1.0));


        int argout = 0;
        for (unsigned i = 0; i < p2p.size(); i++)
        {
            Vector3d<DoubleType> reproj = Vector3d<DoubleType>(H * Vector3dd::FromProjective(p2p[i].start));

            Vector2d<DoubleType> point  = reproj.xy() / reproj.z();
            Vector2d<DoubleType> target = Vector2d<DoubleType>(DoubleType(p2p[i].end.x()), DoubleType(p2p[i].end.y()));

            Vector2d<DoubleType> diff = point - target;
            if (out)
            {
                out[argout++] = diff.x();
                out[argout++] = diff.y();
            }
        }

        for (unsigned i = 0; i < p2l.size(); i++)
        {
            Vector3d<DoubleType> reproj = Vector3d<DoubleType>(H * Vector3dd::FromProjective(p2l[i].start));
            Vector2d<DoubleType> point = reproj.xy() / reproj.z();

            Vector2d<DoubleType> lineNormal = Vector2d<DoubleType>(p2l[i].end.normal());
            DoubleType d = (lineNormal & point) + p2l[i].end.last();
            d = d * d;
            DoubleType lsq = lineNormal.sumAllElementsSq();
            DoubleType distanceSq = (d / lsq);

            if (out)
                out[argout++] = sqrt(distanceSq);
        }
    }

};



/* This part is an attempt to support an interface for a new block structure*/
class HomographyReconstructorBlock : public HomorgaphyReconstructorBlockBase, public NewStyleBlock
{
public:
    HomographyReconstructor wrappee;

    virtual int operator ()()
    {
        if (in0() == NULL)
        {
            SYNC_PRINT(("Fail. No input"));
            return 1;
        }

        wrappee.reset();
        for (Correspondence &c: *in0())
        {
            wrappee.addPoint2PointConstraint(c);
        }

        Matrix33 result = wrappee.getBestHomography(algorithm());
        setOut0(new Matrix33(result));

        return 0;
    }

    ~HomographyReconstructorBlock() {
        delete_safe(mOut0);
    }

};


} //namespace corecvs
#endif /* HOMOGRAPHYRECONSTRUCTOR_H_ */

