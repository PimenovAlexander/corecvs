#pragma once
#include <vector>
#include "vector2d.h"

using namespace corecvs;

class DistortionParameters
{
public:
    DistortionParameters() {
        mRad = 300;
        mScale = 500;
        mNeedCalculateRad = true;
    }
    DistortionParameters(double rad, double scale)
    {
        mRad = rad;
        mScale = scale;
        mNeedCalculateRad = false;
    }

    Vector2dd getCentre()
    {
        return mCentre;
    }
    vector<Vector2dd> getPoints()
    {
        return mStraightPoints;
    }
    void setCentre(Vector2dd const &centre)
    {
        mCentre = centre;
    }
    void addPoint(Vector2dd const &point)
    {
        mStraightPoints.insert(mStraightPoints.begin(), point);
    }
    void clear()
    {
        mStraightPoints.clear();
    }

    bool needCalculateParams()
    {
        return mNeedCalculateRad;
    }
    double getRadius()
    {
        return mRad;
    }
    double getScale()
    {
        return mScale;
    }

private:
    Vector2dd mCentre;
    vector<Vector2dd> mStraightPoints;
    double mRad;
    double mScale;
    bool mNeedCalculateRad;
};
