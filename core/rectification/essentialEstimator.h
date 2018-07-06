#ifndef ESSENTIALESTIMATOR_H_
#define ESSENTIALESTIMATOR_H_
/**
 * \file essentialEstimator.h
 * \brief Add Comment Here
 *
 * \date Oct 19, 2011
 * \author alexander
 */

#include <vector>
#include "core/rectification/correspondenceList.h"
#include "core/rectification/essentialMatrix.h"
#include "core/function/function.h"
#include "core/math/quaternion.h"

#include "core/meta/astNode.h"
#include "core/math/matrix/matrixOperations.h"

namespace corecvs {

class ASTNodeInt;

class EssentialEstimator
{
public:

    enum OptimisationMethod {
        METHOD_SVD_LSE,
        METHOD_GRAD_DESC,
        METHOD_MARQ_LEV,
        METHOD_CLASSIC_KALMAN,
        METHOD_GRAD_DESC_RM,

        METHOD_5_POINT,
        METHOD_7_POINT,

        METHOD_LAST,
        METHOD_DEFAULT = METHOD_MARQ_LEV
    };

    static constexpr int defaultSamples(int method)
    {
        return (method == METHOD_5_POINT) ? 5 :
               (method == METHOD_7_POINT) ? 7 : 8;
    }

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
        static double          getRotationAngle(const double in[]);
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
        const vector<Correspondence *> *samples;

        CostFunction7to1(const vector<Correspondence *> *_samples) :
               CostFunctionBase(1),
               samples(_samples)
        {}
        double getCost(const EssentialMatrix &matrix) const;
        virtual void operator()(const double in[], double out[]) override;
        virtual Matrix getJacobian(const double in[], double delta = 1e-7) override;
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
        const vector<Correspondence *> *samples;
        NormalizeFunction() :
            FunctionArgs(CostFunctionBase::VECTOR_SIZE,1) {}

        virtual void operator()(const double in[], double out[]);
        using FunctionArgs::operator();
    };

    /**
     *   Function that returned
     **/
    class CostFunction7toN : public CostFunctionBase {
    public:
        const vector<Correspondence *> *samples;

        CostFunction7toN(const vector<Correspondence *> *_samples) :
            CostFunctionBase((int)_samples->size()),
            samples(_samples)
        {}

        virtual void operator()(const double in[], double out[]) override;
    };

    class CostFunction7toNPacked : public CostFunction7toN {
    public:
        CostFunction7toNPacked(const vector<Correspondence *> *_samples) :
               CostFunction7toN(_samples)
        {}

        typedef FixMatrixFixed<ASTNode, 3, 3> Matrix33Diff;
        typedef GenericQuaternion< ASTNode >   QuaternionDiff;
        typedef Vector2d< ASTNode >   Vector2dDiff;
        typedef Vector3d< ASTNode >   Vector3dDiff;

        static Matrix33Diff essentialAST();
        static ASTNodeInt* derivative(const Matrix33Diff &input);
        virtual Matrix getJacobian(const double in[], double delta = 1e-7)  override;
    };


    class CostFunction7toNGenerated : public CostFunction7toN {
    public:
        CostFunction7toNGenerated(const vector<Correspondence *> *_samples) :
               CostFunction7toN(_samples)
        {}

        virtual Matrix getJacobian(const double in[], double delta = 1e-7)  override;
    };

    class CostFunction7toNGenerated1 : public CostFunction7toN {
    public:
        CostFunction7toNGenerated1(const vector<Correspondence *> *_samples) :
               CostFunction7toN(_samples)
        {}

        virtual Matrix getJacobian(const double in[], double delta = 1e-7)  override;
    };


    /* Main methods for essential matrix extraction */

    EssentialMatrix getEssential               (
            const vector<Correspondence *> &samples,
            OptimisationMethod method = METHOD_DEFAULT);

    std::vector<EssentialMatrix> getEssentials (
            const vector<Correspondence *> &samples,
            OptimisationMethod method = METHOD_DEFAULT);

    EssentialMatrix getEssentialLSE          (const vector<Correspondence *> &samples);
    EssentialMatrix getEssentialLM           (const vector<Correspondence *> &samples, const corecvs::Quaternion &rotation = corecvs::Quaternion::RotationIdentity(), const corecvs::Vector3dd &translation = corecvs::Vector3dd(-1., 0., 0.));
    EssentialMatrix getEssentialGrad         (const vector<Correspondence *> &samples);
    EssentialMatrix getEssentialGradToRm     (const vector<Correspondence *> &samples);
    EssentialMatrix getEssentialSimpleKalman (const vector<Correspondence *> &samples);
    std::vector<EssentialMatrix> getEssential7point(const vector<Correspondence*> &samples);
    std::vector<EssentialMatrix> getEssential5point(const vector<Correspondence*> &samples);


    EssentialEstimator();
    virtual ~EssentialEstimator();
};



} //namespace corecvs
#endif /* ESSENTIALESTIMATOR_H_ */

