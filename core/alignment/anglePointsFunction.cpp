#include "core/alignment/anglePointsFunction.h"

namespace corecvs {

AnglePointsFunction::AnglePointsFunction(
        const vector<vector<Vector2dd> > &sample,
        const ModelToRadialCorrection &modelFactory) :
    FunctionArgs(modelFactory.getModelSize(), getOutputs(sample)),
    mSimpleJacobian(true),
    mSample(sample),
    mModelFactory(modelFactory)
{
}

/**
 * This function computes the cost function.
 *
 *    Lines of length smaller then 3 points are of no good. They fit any model, so they are discarded
 *
 *    For each line the transformation dictated by input model is applied
 *
 *    Cost function is constructed by computing the sine values of the angles between
 *    consecutive segments of the path
 *
 **/
void AnglePointsFunction::operator ()(const double in[], double out[])
{
    int count = 0;
    RadialCorrection model = mModelFactory.getRadial(in);

    for (unsigned i = 0; i < mSample.size(); i++)
    {
        const vector<Vector2dd> &straight = mSample.at(i);
        if (straight.size() < 3) {
            continue;
        }

        /* Vector deformed by model */
        vector<Vector2dd> newStraight(straight.size());
        for (unsigned j = 0; j < straight.size(); j ++)
        {
            newStraight[j] = model.map(straight[j]);
        }

        Vector2dd p1 = newStraight[0];
        Vector2dd p2 = newStraight[1];
        for (unsigned j = 2; j < straight.size(); j++)
        {
            Vector2dd p3 = newStraight[j];
            Vector2dd a = (p1 - p2);
            Vector2dd b = (p3 - p2);
            out[count] = a.sineTo(b);
            p1 = p2;
            p2 = p3;
            count++;
        }
    }

    /* Enforcing corners */
#if ENFORCE_CORNERS
    Vector2dd corner1 = mCenter * 2.0;
    Vector2dd pos1 = model.map(corner1);
    pos1 -= corner1;
    out[count++] = pos1.x();
    out[count++] = pos1.y();
    Vector2dd corner2 = Vector2dd(0.0, 0.0);
    Vector2dd pos2 = model.map(corner2);
    pos2 -= corner2;
    out[count++] = pos2.x();
    out[count++] = pos2.y();
#endif

}


/**
 *  Jacobian of the cost function
 **/
Matrix AnglePointsFunction::getJacobian(const double in[], double delta)
{
    CORE_UNUSED(delta);
    if (mSimpleJacobian) {
        Matrix res1 = FunctionArgs::getJacobian(in, 1e-7);
        return res1;
    }

    Matrix result(outputs, inputs);
    int count = 0;
    for (unsigned i = 0; i < mSample.size(); i ++)
    {
        vector<Vector2dd> straight = mSample[i];


        RadialFunc f(straight,
                    mModelFactory.mLockedDimentions.center(),
               (int)mModelFactory.mLockedDimentions.mParams.koeff().size());

        vector<Vector2dd> newStraight(straight.size());
        f.setScaleFactor(mModelFactory.mLockedDimentions.mParams.aspect());
        Matrix dPointsDKoeff = f.getJacobian(in);
        f(in, &(newStraight[0].x()));

        Vector2dd p1 = newStraight[0];
        Vector2dd p2 = newStraight[1];
        for (unsigned j = 1; j < straight.size() - 1; j++)
        {
            Vector2dd p3 = newStraight[j + 1];
            Vector2dd a = (p1 - p2);
            Vector2dd b = (p3 - p2);

            double derP1X = (b.y() * a.l2Metric() - (a.x() * b.y() - a.y() * b.x()) * a.x() / a.l2Metric()) / (b.l2Metric() * pow (a.l2Metric(), 2)); //der p1.x()
            double derP1Y = (- b.x() * a.l2Metric() - (a.x() * b.y() - a.y() * b.x()) * a.y() / a.l2Metric()) / (b.l2Metric() * pow (a.l2Metric(), 2)); // der p1.y()
            double derP2X = ((- b.y() + a.y()) * a.l2Metric() * b.l2Metric() - (a.x() * b.y() - a.y() * b.x()) * (a.x() * b.l2Metric() / a.l2Metric() + b.x() * a.l2Metric() / b.l2Metric()))
                    / pow (a.l2Metric() * b.l2Metric(), 2); // der p2.x()
            double derP2Y = ((- a.x() + b.x()) * a.l2Metric() * b.l2Metric() - (a.x() * b.y() - a.y() * b.x()) * (a.y() * b.l2Metric() / a.l2Metric() + b.y() * a.l2Metric() / b.l2Metric()))
                    / pow (a.l2Metric() * b.l2Metric(), 2); //der p2.y()
            double derP3X = (-a.y() * b.l2Metric() - (a.x() * b.y() - a.y() * b.x()) * b.x() / b.l2Metric()) / (a.l2Metric() * pow(b.l2Metric(), 2)); // der p3.x()
            double derP3Y = (a.x() * b.l2Metric() - (a.x() * b.y() - a.y() * b.x()) * b.y() / b.l2Metric()) / (a.l2Metric() * pow(b.l2Metric(), 2)); // der p3.y()

            for (int k = 0; k < inputs; k ++)
            {
                result.a(count, k) = derP1X * dPointsDKoeff.a((j - 1) * 2, k) + derP1Y * dPointsDKoeff.a(2 * j - 1, k) + derP2X * dPointsDKoeff.a(2 * j, k) +
                        derP2Y * dPointsDKoeff.a(j * 2 + 1, k) + derP3X * dPointsDKoeff.a((j + 1) * 2, k) + derP3Y * dPointsDKoeff.a(j * 2 + 3, k);
            }
            p1 = p2;
            p2 = p3;
            count ++;
        }
    }
    return result;
}

/**
 *  Count number of outputs - the size of cost vector
 **/
int AnglePointsFunction::getOutputs(const vector<vector<Vector2dd> > &sample)
{
    int pointsNum = 0;
    for (unsigned i = 0; i < sample.size(); i ++)
    {
        if (sample[i].size() < 3) {
            continue;
        }
        pointsNum += (int)sample[i].size() - 2;
    }
#if ENFORCE_CORNERS
    pointsNum += 4;
#endif

    return pointsNum;

}

} // namespace corecvs

