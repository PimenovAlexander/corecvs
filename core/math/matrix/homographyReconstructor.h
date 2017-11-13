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

#include "core/math/vector/vector2d.h"
#include "core/math/matrix/matrix33.h"
#include "core/geometry/polygons.h"
#include "core/rectification/correspondenceList.h"
#include "core/math/matrix/matrix.h"
#include "core/function/function.h"
#include "core/geometry/line.h"
namespace corecvs {

using std::vector;

typedef PrimitiveCorrespondence<Vector2dd, Line2d> CorrespondencePointLine;
typedef PrimitiveCorrespondence<Vector2dd, Segment2d> CorrespondencePointSegment;


class HomographyReconstructor
{
public:
    vector<Correspondence> p2p;
    vector<CorrespondencePointLine> p2l;
    vector<CorrespondencePointSegment> p2s;


    HomographyReconstructor();

    void addPoint2PointConstraint(const Vector2dd &from, const Vector2dd &to);
    void addPoint2LineConstraint(const Vector2dd &from, const Line2d &line);
    void addPoint2SegmentConstraint(const Vector2dd &from, const Segment2d &line);

    void reset(void);

    bool hasEnoughtConstraints();

    double getCostFunction(Matrix33 &input, double out[] = 0);
    int  getConstraintNumber();

    friend ostream & operator << (ostream &out, const HomographyReconstructor &reconstructor);

    Matrix33 getBestHomography(int method);

    /**
     * Function was introduced for testing. Usage is discouraged. Use getBestHomographyLSE1
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
    Matrix33 getBestHomographyClassicKalman(void);
    Matrix33 getBestHomographyFastKalman(void);
    Matrix33 getBestHomographyLM(Matrix33 guess = Matrix33(1.0));


    void normalisePoints(Matrix33 &transformLeft, Matrix33 &transformRight);

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

    class CostFunction : public FunctionArgs {
    public:
        HomographyReconstructor *reconstructor;
        CostFunction(HomographyReconstructor *_reconstructor) : FunctionArgs(8, _reconstructor->getConstraintNumber()), reconstructor(_reconstructor) {}

        virtual void operator()(const double in[], double out[]);
    };

    class CostFunctionWize : public FunctionArgs {
    public:
        HomographyReconstructor *reconstructor;
        CostFunctionWize(HomographyReconstructor *_reconstructor) : FunctionArgs(8,_reconstructor->getConstraintNumber()), reconstructor(_reconstructor) {}

        virtual void operator()(const double in[], double out[]);
        Matrix33 matrixFromState(const double in[]);
    };


};


} //namespace corecvs
#endif /* HOMOGRAPHYRECONSTRUCTOR_H_ */

