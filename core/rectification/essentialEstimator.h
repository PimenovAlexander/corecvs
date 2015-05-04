/**
 * \file essentialEstimator.h
 * \brief Add Comment Here
 *
 * \date Oct 19, 2011
 * \author alexander
 */

#ifndef ESSENTIALESTIMATOR_H_
#define ESSENTIALESTIMATOR_H_
#include <vector>
#include "correspondanceList.h"
#include "essentialMatrix.h"
#include "function.h"
#include "quaternion.h"
namespace corecvs {

class EssentialEstimator
{
public:

    enum OptimisationMethod {
        METHOD_SVD_LSE,
        METHOD_GRAD_DESC,
        METHOD_MARQ_LEV,
        METHOD_CLASSIC_KALMAN,
        METHOD_ITERATIVE_KALMAN,
        METHOD_GRAD_DESC_RM,
        METHOD_LAST,
        METHOD_DEFAULT = METHOD_MARQ_LEV
    };


    /**
     * This is a class that describe the translation form state in form of
     * the array of doubles into an Essential matrix.
     *
     **/
    class CostFunctionBase : public FunctionArgs
    {
    public:
        enum InputVector2Model {
            ROTATION_Q_X = 0,
            ROTATION_Q_Y,
            ROTATION_Q_Z,
            ROTATION_Q_T,
            TRANSLATION_X,
            TRANSLATION_Y,
            TRANSLATION_Z,
            VECTOR_SIZE
        };



        CostFunctionBase(int outputs) :
            FunctionArgs(VECTOR_SIZE, outputs)
        {}

        static Quaternion      getRotationQ    (const double in[]);
        static Matrix33        getRotationM    (const double in[]);
        static Vector3dd       getTranslation  (const double in[]);
        static EssentialMatrix getEssential    (const double in[]);
        static double         getRotationAngle(const double in[]);
        static Vector3dd       getRotationAxis (const double in[]);

        static void packState(double out[], const Quaternion &q, const Vector3dd &t);
    };

    /**
     * Normalized cost function try to trick the optimization algorithms
     * by re-normalising the direction and the rotation quaternion every time
     * it is used.
     **/
    class CostFunction7to1 : public CostFunctionBase {
    public:
        const vector<Correspondance *> *samples;

        CostFunction7to1(const vector<Correspondance *> *_samples) :
               CostFunctionBase(1),
               samples(_samples)
        {};
        double getCost(const EssentialMatrix &matrix) const;
        virtual void operator()(const double in[], double out[]);
        virtual Matrix getJacobian(const double in[], double delta = 1e-7);
    };


    /**
     *  Function that normalizes the current state.
     *  It makes quaternion have module of 1 and also makes 1 a length of the
     *  direction vector
     *
     **/
    class NormalizeFunction : public FunctionArgs
    {
    public:
        const vector<Correspondance *> *samples;
        NormalizeFunction() :
            FunctionArgs(CostFunctionBase::VECTOR_SIZE,1) {};

        virtual void operator()(const double in[], double out[]);
        using FunctionArgs::operator();
    };

    /**
     *   Function that returned
     **/
    class CostFunction7toN : public CostFunctionBase {
    public:
        const vector<Correspondance *> *samples;

        CostFunction7toN(const vector<Correspondance *> *_samples) :
            CostFunctionBase((int)_samples->size()),
            samples(_samples)
        {};

        virtual void operator()(const double in[], double out[]);
    };


    /* Main methods for essential matrix extraction */

    EssentialMatrix getEssential             (
            const vector<Correspondance *> &samples,
            OptimisationMethod method = METHOD_DEFAULT);

    EssentialMatrix getEssentialLSE          (const vector<Correspondance *> &samples);
    EssentialMatrix getEssentialLM           (const vector<Correspondance *> &samples);
    EssentialMatrix getEssentialGrad         (const vector<Correspondance *> &samples);
    EssentialMatrix getEssentialGradToRm     (const vector<Correspondance *> &samples);
    EssentialMatrix getEssentialSimpleKalman (const vector<Correspondance *> &samples);
#if 0
    EssentialMatrix getEssentialKalman       (const vector<Correspondance *> &samples);
    EssentialMatrix getEssentialMultiKalman  (const vector<Correspondance *> &samples);
#endif


    EssentialEstimator();
    virtual ~EssentialEstimator();
};


} //namespace corecvs
#endif /* ESSENTIALESTIMATOR_H_ */

