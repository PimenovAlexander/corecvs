/**
 * \file radialCorrection.cpp
 * \brief This file contains the radial lens distortion correction primitives
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */

#include "core/utils/global.h"
#include "core/alignment/radialCorrection.h"
#include "core/math/levenmarq.h"
#include "core/geometry/ellipticalApproximation.h"

namespace corecvs {


RadialCorrection::RadialCorrection(const LensDistortionModelParameters &params) :
        FunctionArgs(2,2),
        mParams(params),
        addShiftX(0.0),
        addShiftY(0.0)
{
}

RadialCorrection::~RadialCorrection()
{
}

G12Buffer *RadialCorrection::doCorrectionTransform(G12Buffer *inputBuffer)
{
    return inputBuffer->doReverseDeformationBl<G12Buffer, RadialCorrection>
            (this, inputBuffer->h, inputBuffer->w);
}

RGB24Buffer *RadialCorrection::doCorrectionTransform(RGB24Buffer *inputBuffer)
{
    return inputBuffer->doReverseDeformationBlTyped<RadialCorrection>
            (this, inputBuffer->h, inputBuffer->w);
}

class RadialCorrectionInversionCostFunction : public FunctionArgs
{
public:
    enum {
        TANGENT_X = 0,
        TANGENT_Y,
        RADIAL_FIRST
    };

    static const unsigned MODEL_POWER = 6;
    static const unsigned MODEL_SIZE = RADIAL_FIRST + MODEL_POWER;

    RadialCorrection &mInput;
    RadialCorrection &mGuess;
    int mSteps;
    int mH;
    int mW;

    RadialCorrectionInversionCostFunction(
            RadialCorrection &input,
            RadialCorrection &guess,
            int steps,
            int h, int w
    ) : FunctionArgs(MODEL_SIZE, getOutputs(steps)),
        mInput(input),
        mGuess(guess),
        mSteps(steps),
        mH(h),
        mW(w)
    {}

    virtual void operator()(const double in[], double out[])
    {
        double dh = (double)mH / (mSteps - 1);
        double dw = (double)mW / (mSteps - 1);
        RadialCorrection guess = updateWithModel(mGuess, in);

        for (int i = 0; i < mSteps; i++)
        {
            for (int j = 0; j < mSteps; j++)
            {
                Vector2dd point(dw * j, dh * i);
                Vector2dd deformed = mInput.map(point); /* this could be cached */
                Vector2dd backproject = guess.map(deformed);
                Vector2dd diff = backproject - point;

                out[2 * (i * mSteps + j)    ] = diff.x();
                out[2 * (i * mSteps + j) + 1] = diff.y();
            }
        }
    }

    EllipticalApproximation1d aggregatedCost(const double in[])
    {
        vector<double> error(outputs);
        operator ()(in, &error[0]);

        EllipticalApproximation1d result;
        for (unsigned i = 0; i < error.size(); i++)
        {
            result.addPoint(error[i]);
        }   
        return result;
    }

    static void fillWithRadial(const RadialCorrection &input, double out[])
    {
        out[TANGENT_X] = input.mParams.tangentialX();
        out[TANGENT_Y] = input.mParams.tangentialY();
        for (unsigned i = 0; i < MODEL_POWER; i++)
        {
            out[i + RADIAL_FIRST] = input.mParams.mKoeff[i];
        }
    }

    static RadialCorrection updateWithModel(const RadialCorrection &input, const double in[])
    {
        LensDistortionModelParameters params = input.mParams;
        params.setTangentialX(in[TANGENT_X]);
        params.setTangentialY(in[TANGENT_Y]);
        for (unsigned i = 0; i < MODEL_POWER; i++)
        {
            params.mKoeff[i] = in[i + RADIAL_FIRST];
        }

        return RadialCorrection(params);
    }

private:
    static int getOutputs(int steps)
    {
        return 2 * steps * steps;
    }


};


RadialCorrection RadialCorrection::invertCorrection(int h, int w, int step)
{
    LensDistortionModelParameters input = this->mParams;
    LensDistortionModelParameters result;

    /* make initial guess */

    result.setPrincipalX(input.principalX());
    result.setPrincipalY(input.principalY());
    result.setNormalizingFocal(input.normalizingFocal());

    result.setTangentialX(-input.tangentialX());
    result.setTangentialY(-input.tangentialY());

    result.setScale(1.0 / input.scale());

    result.setAspect(1.0 / input.scale()); /*< bad guess I believe */

    result.mKoeff.resize(RadialCorrectionInversionCostFunction::MODEL_POWER);
    for (unsigned i = 0; i < RadialCorrectionInversionCostFunction::MODEL_POWER; i++)
    {
        if (i < input.mKoeff.size()) {
            result.mKoeff[i] = -input.mKoeff[i];
        } else {
            result.mKoeff[i] = 0.0;
        }
    }

    /* Pack the guess and launch optimization */
    RadialCorrection guess(result);
    RadialCorrectionInversionCostFunction cost(*this, guess, step, h, w);

    LevenbergMarquardt lmFit;
    lmFit.maxIterations = 101;
    lmFit.maxLambda = 10e8;
    lmFit.lambdaFactor = 8;
    lmFit.f = &cost;
    lmFit.traceCrucial  = true;
    lmFit.traceProgress = true;
    lmFit.traceMatrix   = true;

    vector<double> initialGuess(cost.inputs);
    RadialCorrectionInversionCostFunction::fillWithRadial(guess, &(initialGuess[0]));
    cout << guess.mParams << endl;

    EllipticalApproximation1d stats;
    stats = cost.aggregatedCost(&(initialGuess[0]));
    SYNC_PRINT(("Start Mean Error: %f px\n", stats.getRadiusAround0()));
    SYNC_PRINT(("Start Max  Error: %f px\n", stats.getMax()));

    vector<double> target(cost.outputs, 0.0);
    vector<double> optimal = lmFit.fit(initialGuess, target);

    guess = RadialCorrectionInversionCostFunction::updateWithModel(guess, &(optimal[0]));

    /* Cost */

    cout << guess.mParams << endl;
    stats = cost.aggregatedCost(&(optimal[0]));
    SYNC_PRINT(("Final Mean Error: %f px\n", stats.getRadiusAround0()));
    SYNC_PRINT(("Final Max  Error: %f px\n", stats.getMax()));


    return guess;
}

/* use sample points. We can improve maximium numerically, but it's ok so far */
EllipticalApproximation1d RadialCorrection::compareWith(const RadialCorrection &other, int h, int w, int steps)
{

    EllipticalApproximation1d result;

    double dh = (double)h / (steps - 1);
    double dw = (double)w / (steps - 1);

    for (int i = 0; i < steps; i++)
    {
        for (int j = 0; j < steps; j++)
        {
            Vector2dd point(dw * j, dh * i);

            Vector2dd deformedThis  =       map(point); /* this could be cached */
            Vector2dd deformedOther = other.map(point);
            result.addPoint((deformedOther - deformedThis).l2Metric());
        }
    }

    return result;
}





} //namespace corecvs
