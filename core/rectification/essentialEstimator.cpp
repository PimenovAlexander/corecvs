/**
 * \file essentialEstimator.cpp
 * \brief Add Comment Here
 *
 * \date Oct 19, 2011
 * \author alexander
 */

#include "essentialEstimator.h"
#include "quaternion.h"
#include "levenmarq.h"
#include "gradientDescent.h"
#include "classicKalman.h"

#ifdef NONFREE
#include "polynomialSolver.h"
#endif
//#include "kalman.h"

namespace corecvs {

using std::flush;

#define DEEP_TRACE

EssentialEstimator::EssentialEstimator()
{
}


/**
 *  Both essential and fundamental matrices can be searched with 8 point algorithm
 *
 * \code
 *             [ a_11   a_12  a_13 ]  [ x']    [ 0 ]
 * [x, y ,1]   [ a_21   a_22  a_23 ]  [ y'] =  [ 0 ]
 *             [ a_31   a_32  a_33 ]  [ 1 ]    [ 0 ]
 * \endcode
 *
 * \f[
 *      \pmatrix{ x & y & 1 }
 *      \pmatrix{
 *        F_{1,1} & F_{1,2} & F_{1,3}\cr
 *        F_{2,1} & F_{2,2} & F_{2,3}\cr
 *        F_{3,1} & F_{3,2} & F_{3,3}}
 *      \pmatrix{ x' \cr y' \cr 1} = \pmatrix{ 0 \cr 0 \cr 0}
 * \f]
 *
 *
 *  We reconstruct the model by LSE estimation.
 *
 * \f[
 *      F_{1,1} x x' + F_{1,2} x y' + F_{1,3} x +
 *      F_{2,1} y x' + F_{2,2} y y' + F_{2,3} y +
 *      F_{3,1} x' + F_{3,2} y' + F_{3,3} = 0
 * \f]
 *
 *
 * \f[
 *     \left( \begin{array}{ccccccccc}
 *       x_1 x_1' & x_1 y_1' & x_1 & y_1 x_1' & y_1 y_1' & y_1 & x_1' & y_1' & 1 \\
 *       \multicolumn{9}{c}{\cdots} \\
 *       x_n x_n' & x_n y_n' & x_n & y_n x_n' & y_n y_n' & y_n & x_n' & y_n' & 1
 *    \end{array} \right )
 *
 *    \pmatrix{
 *      F_{1,1} \cr F_{1,2} \cr F_{1,3} \cr
 *      F_{2,1} \cr F_{2,2} \cr F_{2,3} \cr
 *      F_{3,1} \cr F_{3,2} \cr F_{3,3}
 *    }
 *    = \vec 0
 * \f]
 **/
EssentialMatrix EssentialEstimator::getEssentialLSE(const vector<Correspondence*> & samples)
{
    Matrix X(std::max(9, (int)samples.size()), 9, 0.0);
    Matrix W(1,9);
    Matrix V(9,9);
    Matrix33 F(0.0);

#if 1
    corecvs::Vector2dd meanL(0.0, 0.0), meanR(0.0, 0.0);
    corecvs::Vector2dd stdevL(0.0, 0.0), stdevR(0.0, 0.0);
    for (size_t i = 0; i < samples.size(); ++i)
    {
        auto L = samples[i]->start;
        auto R = samples[i]->end;
        meanL += L;
        meanR += R;

        stdevL += L * L;
        stdevR += R * R;
    }
    double N = 1.0 / samples.size();
    meanL *= N;
    meanR *= N;
    stdevL = stdevL * N - meanL * meanL;
    stdevR = stdevR * N - meanR * meanR;
    for (size_t i = 0; i < 2; ++i)
    {
        stdevL[i] = std::sqrt(stdevL[i]);       // TODO: this must be present in statistics/approx blocks...
        stdevR[i] = std::sqrt(stdevR[i]);
    }
#if 1
    corecvs::Matrix33 TL = corecvs::Matrix33(
            stdevL[0],       0.0, meanL[0],
                  0.0, stdevL[1], meanL[1],
                  0.0,       0.0,      1.0).inv();
    corecvs::Matrix33 TR = corecvs::Matrix33(
            stdevR[0],       0.0, meanR[0],
                  0.0, stdevR[1], meanR[1],
                  0.0,       0.0,      1.0).inv();
#else
    corecvs::Matrix33 TL(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
    corecvs::Matrix33 TR(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
#endif
#endif

    corecvs::Vector2dd ml(0.0, 0.0), mr(0.0, 0.0);
    for (unsigned i = 0; i < samples.size(); i++)
    {
        Vector2dd first  = samples[i]->start;
        Vector2dd second = samples[i]->end;
#if 1
        auto firstP = TL * corecvs::Vector3dd(first.x(), first.y(), 1.0);
        auto secondP = TR * corecvs::Vector3dd(second.x(), second.y(), 1.0);
        firstP /= firstP[2];
        secondP /= secondP[2];
        first = corecvs::Vector2dd(firstP[0], firstP[1]);
        second = corecvs::Vector2dd(secondP[0], secondP[1]);
        ml += first;
        mr += second;
#endif
        X.fillLineWithArgs(i,
        first.x() * second.x(), first.x() * second.y(), first.x(),
        first.y() * second.x(), first.y() * second.y(), first.y(),
        second.x(), second.y(),  1.0);
    }
//    X.a(samples.size(), 0) = 1.0;
//    std::cout << ml << std::endl << mr << std::endl;

#ifdef DEEP_TRACE
//        X.print();
//        cout << endl;
//        fflush(stdout);
#endif

    /** \todo TODO Here the transform should be done to increase the precision.
     * The choice should be centered to 0 and scaled to average of sqrt(2) */

    Matrix::svd(&X, &W, &V);

#ifdef DEEP_TRACE
//    printf("The singular values:\n");
//    W.print();
//    printf("\n");
//    X.print();
//    printf("\n");
//    V.print();
//    printf("\n");
#endif


    int min = -1;
    double minval = std::numeric_limits<double>::max();
    for (unsigned i = 0; i < 9; i ++)
    {
        if (W.a(0, i) <= minval)
        {
            min = i;
            minval = W.a(0, i);
        }
    }

    F = Matrix33(
        V.a(0,min), V.a(1,min), V.a(2,min),
        V.a(3,min), V.a(4,min), V.a(5,min),
        V.a(6,min), V.a(7,min), V.a(8,min)
    );

    F = TL.transposed() * F * TR;
    if (F.a(2,1) < 0) {
        F = -F;
    }

    return EssentialMatrix(F);
}


EssentialMatrix EssentialEstimator::getEssentialLM(const vector<Correspondence*> & samples)
{
    CostFunction7toN costFunction(&samples);
    NormalizeFunction normalise;
    LevenbergMarquardt LMfit;

    LMfit.f = &costFunction;
    LMfit.normalisation = &normalise;
    LMfit.maxIterations = 100000;
    LMfit.startLambda = 10;
    LMfit.lambdaFactor = 20.0;

    vector<double> input(CostFunctionBase::VECTOR_SIZE);
    /* Left camera is in the negative direction of right camera */
    Quaternion rotation    = Quaternion::RotationIdentity();
    Vector3dd  translation = Vector3dd(-1.0, 0.0, 0.0);
    CostFunctionBase::packState(&input[0], rotation, translation);

    vector<double> output(samples.size());
    for (unsigned i = 0; i < samples.size(); i++)
        output[i] = 0.0;

    vector<double> optInput = LMfit.fit(input, output);

    PrinterVisitor visitor;

   /* cout << "Axis" << endl;
    CostFunctionBase::getRotationAxis(&optInput[0]).accept(visitor);
    cout << "Angle:" << endl;
    cout << radToDeg(CostFunctionBase::getRotationAngle(&optInput[0]))  << "deg" << endl;
    cout << "Baseline:" << endl;
    CostFunctionBase::getTranslation(&optInput[0]).accept(visitor);
*/

    return CostFunctionBase::getEssential(&optInput[0]);
}

EssentialMatrix EssentialEstimator::getEssentialGrad(const vector<Correspondence*> & samples)
{
    CostFunction7to1 costFunction(&samples);
    NormalizeFunction normalise;
    GradientDescent GDfit;

    GDfit.f = &costFunction;
    GDfit.normalisation = &normalise;
    GDfit.maxIterations = 10000000;
    GDfit.lambda = 0.003;

    vector<double> input(CostFunctionBase::VECTOR_SIZE);
    Quaternion rotation    = Quaternion::RotationIdentity();
    Vector3dd  translation = Vector3dd(-1.0, 0.0, 0.0);
    CostFunctionBase::packState(&input[0], rotation, translation);

    vector<double> optInput = GDfit.minimise(input);
    return CostFunctionBase::getEssential(&optInput[0]);
}

EssentialMatrix EssentialEstimator::getEssentialGradToRm(const vector<Correspondence*> & samples)
{
    CostFunction7toN costFunction(&samples);
    NormalizeFunction normalise;
    GradientDescentRnToRm GDfit;
    GDfit.f = &costFunction;
    GDfit.normalisation = &normalise;
    GDfit.maxIterations = 1000000;
    GDfit.lambda = 0.003;

    vector<double> input(CostFunctionBase::VECTOR_SIZE);
    Quaternion rotation    = Quaternion::RotationIdentity();
    Vector3dd  translation = Vector3dd(-1.0, 0.0, 0.0);
    CostFunctionBase::packState(&input[0], rotation, translation);

    vector<double> output(samples.size());
    /* This is an overkill*/
    for (unsigned i = 0; i < output.size(); i++)
        output[i] = 0.0;

    vector<double> optInput = GDfit.fit(input, output);
    return CostFunctionBase::getEssential(&optInput[0]);
}

EssentialMatrix EssentialEstimator::getEssentialSimpleKalman (const vector<Correspondence *> &samples)
{
    IdentityFunction F(CostFunctionBase::VECTOR_SIZE);
    CostFunction7to1 H(&samples);
    NormalizeFunction N;

    // We know almost nothing about the essential matrix
    Matrix P(CostFunctionBase::VECTOR_SIZE, CostFunctionBase::VECTOR_SIZE, 500.0);
    // Our model evolution is precise
    Matrix Q(CostFunctionBase::VECTOR_SIZE, CostFunctionBase::VECTOR_SIZE, 0.0);
    // Our measurement is precise up to 2px.
    Matrix R(1,1, 2.0);

    Vector input(CostFunctionBase::VECTOR_SIZE);
    Quaternion rotation    = Quaternion::RotationIdentity();
    Vector3dd  translation = Vector3dd(-1.0, 0.0, 0.0);
    CostFunctionBase::packState(&input[0], rotation, translation);

    ClassicKalman kalmanFilter(
        &F,
        &H,
        Q,
        R,
        P,
        input);


    Vector normalised(CostFunctionBase::VECTOR_SIZE);

    static int maxIterations = 2500000;

    cout << "================== Starting Simple Kalman optimisation ================== " << endl;
    cout << "[" << flush;

    for (int step = 0; step < maxIterations; step++)
    {
        if ((step % ((maxIterations / 100) + 1) == 0))
        {
            cout << "#" << flush;
        }

        double measurement = 0.0;
        kalmanFilter.predict();
        N(kalmanFilter.xPredicted, normalised);
        kalmanFilter.xPredicted = normalised;
        kalmanFilter.z[0] = measurement;
        kalmanFilter.update();
    }
    cout << "]" << endl;

    return CostFunctionBase::getEssential(&kalmanFilter.x[0]);
}

#if 0
EssentialMatrix EssentialEstimator::getEssentialKalman(const vector<Correspondence *> &samples)
{
    /* Setting up P, Q, R */
    CostFunction7to1 F(&samples);
    NormalizeFunction N;
    KalmanFilter kalmanFilter(CostFunctionBase::VECTOR_SIZE, 0.0);
    *(kalmanFilter.P->diagonal) *= 500.0;

    DyVector misfit(1);
    misfit.dy[0] = 0.0;
    misfit.index[0] = 0;
    misfit.number = 1;

    static int maxIterations = 2500000;

    cout << "================ Starting Innovating Kalman optimisation ================ " << endl;
    cout << "[" << flush;

    Vector input(CostFunctionBase::VECTOR_SIZE);
    Quaternion rotation    = Quaternion::RotationIdentity();
    Vector3dd  translation = Vector3dd(-1.0, 0.0, 0.0);
    CostFunctionBase::packState(&input[0], rotation, translation);

    Vector normalised(CostFunctionBase::VECTOR_SIZE);

    for (int step = 0; step < maxIterations; step++)
    {
        if ((step % ((maxIterations / 100) + 1) == 0))
        {
            cout << "#" << flush;
        }

        for (int i = 0; i < CostFunctionBase::VECTOR_SIZE; i++)
            kalmanFilter.currentState->a(0, i) = input[i];

        Matrix R(1,1,2.0);
        misfit.dy[0] = F.getCost(CostFunctionBase::getEssential(&input[0]));

        Matrix H(1, CostFunctionBase::VECTOR_SIZE);
        H = F.getJacobian(&input[0]);
        H.neg();

        kalmanFilter.step(&R, &misfit,  &H);

        for (int i = 0; i < CostFunctionBase::VECTOR_SIZE; i++)
            input[i] = kalmanFilter.currentState->a(0, i);

        N(input, normalised);
        input = normalised;
    }
    cout << "]" << endl;

    return CostFunctionBase::getEssential(&input[0]);
}


EssentialMatrix EssentialEstimator::getEssentialMultiKalman(const vector<Correspondence *> &samples)
{
    /* Setting up P, Q, R */
    CostFunction7toN F(&samples);
    NormalizeFunction N;
    KalmanFilter kalmanFilter(CostFunctionBase::VECTOR_SIZE, 0.0);
    *(kalmanFilter.P->diagonal) *= 1000.0;

    DyVector misfit((int)samples.size());
    misfit.number = (int)samples.size();
    for (unsigned i = 0; i < samples.size(); i++)
        misfit.index[i] = i;

    static int maxIterations = 250000;

    cout << "================ Starting Innovating Kalman Multi Optimisation ================ " << endl;
    cout << "[" << flush;

    Vector input(CostFunctionBase::VECTOR_SIZE);
    Quaternion rotation    = Quaternion::RotationIdentity();
    Vector3dd  translation = Vector3dd(-1.0, 0.0, 0.0);
    CostFunctionBase::packState(&input[0], rotation, translation);

    Vector normalised(CostFunctionBase::VECTOR_SIZE);

    for (int step = 0; step < maxIterations; step++)
    {
        if ((step % ((maxIterations / 100) + 1) == 0))
        {
            cout << "#" << flush;
        }

        for (int i = 0; i < CostFunctionBase::VECTOR_SIZE; i++)
            kalmanFilter.currentState->a(0, i) = input[i];

        Matrix R(misfit.number, misfit.number,10.0);
        F(&input[0], misfit.dy);

        Matrix H = F.getJacobian(&input[0]);
        H.neg();

        kalmanFilter.step(&R, &misfit,  &H);

        for (int i = 0; i < CostFunctionBase::VECTOR_SIZE; i++)
            input[i] = kalmanFilter.currentState->a(0, i);

        N(input, normalised);
        input = normalised;
    }
    cout << "]" << endl;

    return CostFunctionBase::getEssential(&input[0]);
}
#endif

EssentialMatrix EssentialEstimator::getEssential             (
        const vector<Correspondence *> &samples,
        OptimisationMethod method)
{
    switch (method)
    {
        case METHOD_SVD_LSE:
            return getEssentialLSE(samples);
        case METHOD_GRAD_DESC:
            return getEssentialGrad(samples);
        case METHOD_MARQ_LEV:
            return getEssentialLM(samples);
        case METHOD_CLASSIC_KALMAN:
            return getEssentialSimpleKalman(samples);
        case METHOD_ITERATIVE_KALMAN:
//            return getEssentialKalman(samples);
        case METHOD_GRAD_DESC_RM:
//            return getEssentialGradToRm(samples);

        default:
            return getEssentialLM(samples);
    }

    return EssentialMatrix();
}



Quaternion EssentialEstimator::CostFunctionBase::getRotationQ(const double in[])
{
    return Quaternion(in[ROTATION_Q_X], in[ROTATION_Q_Y], in[ROTATION_Q_Z], in[ROTATION_Q_T]).normalised();
}

Vector3dd EssentialEstimator::CostFunctionBase::getTranslation(const double in[])
{
    return Vector3dd(in[TRANSLATION_X], in[TRANSLATION_Y], in[TRANSLATION_Z]).normalised();
}

Matrix33 EssentialEstimator::CostFunctionBase::getRotationM(const double in[])
{
    return getRotationQ(in).toMatrix();
}

double EssentialEstimator::CostFunctionBase::getRotationAngle(const double in[])
{
    return getRotationQ(in).getAngle();
}

Vector3dd EssentialEstimator::CostFunctionBase::getRotationAxis(const double in[])
{
    return getRotationQ(in).getAxis();
}

EssentialMatrix EssentialEstimator::CostFunctionBase::getEssential(const double in[])
{
    return EssentialMatrix::compose(getRotationM(in), getTranslation(in));
}

void EssentialEstimator::CostFunctionBase::packState(double out[], const Quaternion &q, const Vector3dd &t)
{
    out[CostFunctionBase::ROTATION_Q_X]  = q.x();
    out[CostFunctionBase::ROTATION_Q_Y]  = q.y();
    out[CostFunctionBase::ROTATION_Q_Z]  = q.z();
    out[CostFunctionBase::ROTATION_Q_T]  = q.t();
    out[CostFunctionBase::TRANSLATION_X] = t.x();
    out[CostFunctionBase::TRANSLATION_Y] = t.y();
    out[CostFunctionBase::TRANSLATION_Z] = t.z();
}

double EssentialEstimator::CostFunction7to1::getCost(const EssentialMatrix &matrix) const
{
    double cost = 0.0;
    for (unsigned i = 0; i < samples->size(); i++)
    {
        double fitness = matrix.epipolarDistance(*(samples->at(i)));
        cost += fitness * fitness;
    }
    return cost;
}

void EssentialEstimator::CostFunction7to1::operator()(const double in[], double out[])
{
    EssentialMatrix m = getEssential(in);
    out[0] = getCost(m);
}

/**
 **/
Matrix EssentialEstimator::CostFunction7to1::getJacobian(const double in[], double delta)
{
    Matrix result(outputs, inputs);
    // is it needed here?
//    EssentialMatrix m = getEssential(in);

    vector<double> xc(inputs);
    EssentialMatrix m_minus(Matrix33(1.0));
    EssentialMatrix m_plus (Matrix33(1.0));

    for (int i = 0; i < inputs; i++)
    {
       xc[i] = in[i];
    }

    for (int i = 0; i < inputs; i++)
    {
       xc[i] = in[i] - delta;
       m_minus = getEssential(&xc[0]);
       xc[i] = in[i] + delta;
       m_plus  = getEssential(&xc[0]);
       xc[i] = in[i];

       double sum_plus  = 0.0;
       double sum_minus = 0.0;

       for (unsigned j = 0; j < samples->size(); j++)
       {
           Correspondence *corr = samples->at(j);
           double value_plus  = m_plus .epipolarDistance(*corr);
           double value_minus = m_minus.epipolarDistance(*corr);
           sum_plus  += value_plus  * value_plus ;
           sum_minus += value_minus * value_minus;
       }
       result.element(0,i) = (sum_plus - sum_minus) / (2.0 * delta);

/*
       double d = 0.0;

       for (unsigned j = 0; j < samples->size(); j++)
       {
           Correspondence *corr = samples->at(j);
           double value_plus  = m_plus .epipolarDistance(*corr);
           double value_minus = m_minus.epipolarDistance(*corr);
           double value       = m.      epipolarDistance(*corr);

           double dvalue = value * (value_plus - value_minus) / delta;
           d += dvalue;
       }
       result.element(0,i) = d;
*/
/*
       double d = 0.0;

       for (unsigned j = 0; j < samples->size(); j++)
       {
           Correspondence *corr = samples->at(j);
           double value_plus  = m_plus .epipolarDistance(*corr);
           double value_minus = m_minus.epipolarDistance(*corr);
           double value       = m.      epipolarDistance(*corr);

           d += value * (value_plus - value_minus);
       }
       result.element(0,i) = d / delta;
*/
/*       double d = 0.0;
       double sum_plus  = 0.0;
       double sum_minus = 0.0;

       for (unsigned j = 0; j < samples->size(); j++)
       {
           Correspondence *corr = samples->at(j);
           double value_plus  = m_plus .epipolarDistance(*corr);
           double value_minus = m_minus.epipolarDistance(*corr);
           double value       = m.      epipolarDistance(*corr);

           sum_plus  += value * value_plus ;
           sum_minus += value * value_minus;
       }
       result.element(0,i) = (sum_plus - sum_minus) / delta;*/


    }

//    Matrix result1 = FunctionArgs::getJacobian(in);
//    cout << result  << endl;
//    cout << result1 << endl;

    return result;
}

void EssentialEstimator::CostFunction7toN::operator()(const double in[], double out[])
{
    EssentialMatrix matrix = getEssential(in);
    for (unsigned i = 0; i < samples->size(); i++)
    {
        out[i] = matrix.epipolarDistance(*(samples->at(i)));
    }
}

/*Matrix EssentialEstimator::MultioutCostFunction::getJacobian(const double in[], double delta = 1e-7)
{


}*/

void EssentialEstimator::NormalizeFunction::operator()(const double in[], double out[])
{

    Quaternion rotation    = CostFunctionBase::getRotationQ(in);
    Vector3dd  translation = CostFunctionBase::getTranslation(in);
    rotation.normalise();
    translation.normalise();
    CostFunctionBase::packState(out, rotation, translation);
}


EssentialEstimator::~EssentialEstimator()
{
}


} //namespace corecvs

