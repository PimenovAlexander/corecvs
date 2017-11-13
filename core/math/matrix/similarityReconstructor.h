#ifndef SIMILARITYRECONSTRUCTOR_H
#define SIMILARITYRECONSTRUCTOR_H

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
#include "core/rectification/correspondenceList.h"
#include "core/math/matrix/matrix.h"
#include "core/math/quaternion.h"
#include "core/function/function.h"

namespace corecvs {

using std::vector;

/**
 *    Similarity is a way to transform one object into another in the following steps
 *    One set is shifted, scaled and then rotated afterwards scaled back and translated to place
 *
 **/
class Similarity {
public:
    Vector3dd shiftL;
    Vector3dd shiftR;

    double scaleL;
    double scaleR;

    Quaternion rotation;

    Similarity() :
         shiftL(0.0)
       , shiftR(0.0)
       , scaleL(1.0)
       , scaleR(1.0)
       , rotation(Quaternion::RotationIdentity())
    {}


    /**
     *    To matrix transformation
     **/
    Matrix44 toMatrix() const;

    friend ostream & operator << (ostream &out, const Similarity &reconstructor);

    enum {
        SHIFT_X,
        SHIFT_Y,
        SHIFT_Z,
        SCALE,
        ROTATION_X,
        ROTATION_Y,
        ROTATION_Z,
        ROTATION_T,
        PARAM_NUMBER
    };

    void fillFunctionInput(double in[]);

    Similarity(const double in[]) :
        shiftL(in[SHIFT_X], in[SHIFT_Y], in[SHIFT_Z]),
        shiftR(0.0),
        scaleL(in[SCALE]),
        scaleR(1.0),
        rotation(in[ROTATION_X], in[ROTATION_Y], in[ROTATION_Z], in[ROTATION_T])
    {}

    Similarity getInterpolated(double t);


    /* Get simple params*/
    double getScale();
    double getInvScale();

    Vector3dd getShift();
    Matrix33 getRotation();
};

typedef PrimitiveCorrespondence<Vector3dd, Vector3dd> Correspondence3D;


class SimilarityReconstructor
{
public:
    vector<Correspondence3D> p2p;
    bool trace;

    SimilarityReconstructor();
    void addPoint2PointConstraint(const Vector3dd &from, const Vector3dd &to);
    void reset(void);

    /**
     * Usues classical approach to construct similarity from scratch
     * It estimates  statistics of the point clouds and then computes the rotation
     **/
    Similarity getBestSimilarity();

    /**
     * Makes the guess more precise with Marquardt-Levenberg iterations
     **/
    Similarity getBestSimilarityLM (Similarity &firstGuess);

    /**
     * Makes the guess more precise with Marquardt-Levenberg iterations using
     * alternative cost function
     **/
    Similarity getBestSimilarityLMN(Similarity &firstGuess);


    double getCostFunction(const Similarity &input);

    virtual ~SimilarityReconstructor();


    class CostFunction : public FunctionArgs {
    public:
        SimilarityReconstructor *reconstructor;
        CostFunction(SimilarityReconstructor *_reconstructor) :
            FunctionArgs(Similarity::PARAM_NUMBER,1)
          , reconstructor(_reconstructor)
        {}

        virtual void operator()(const double in[], double out[]);
    };

    class CostFunctionToN : public FunctionArgs {
    public:
        SimilarityReconstructor *reconstructor;
        CostFunctionToN(SimilarityReconstructor * _reconstructor) :
            FunctionArgs(Similarity::PARAM_NUMBER, (int)_reconstructor->p2p.size())
          , reconstructor(_reconstructor)
        {}

        virtual void operator()(const double in[], double out[]);
    };

    class NormalizeFunction : public FunctionArgs
    {
    public:
        NormalizeFunction() :
            FunctionArgs(Similarity::PARAM_NUMBER,Similarity::PARAM_NUMBER) {}

        virtual void operator()(const double in[], double out[]);
        //using FunctionArgs::operator();
    };

};


} //namespace corecvs
#endif // SIMILARITYRECONSTRUCTOR_H
