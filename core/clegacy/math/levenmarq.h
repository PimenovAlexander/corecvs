#pragma once
#include "math/vector/vector2d.h"
#include "math/matrix/matrix.h"
#include <vector>
namespace corecvs {


class LevenMarq
{
public:
    static vector<Vector2dd> doLevMarFit(vector<Vector2dd> const & distortionVector);

private:
    static Matrix * createMatrix(vector<Vector2dd> const & curve, double k, double b);
    static double dist(Matrix * m);
    static vector<Vector2dd> inverseCurve(vector<Vector2dd> const & curve);
};


class LevenMarq
{
public:
    static vector<Vector2dd> doLevMarFit(vector<Vector2dd> const & distortionVector);

private:
    static Matrix * createMatrix(vector<Vector2dd> const & curve, double k, double b);
    static double dist(Matrix * m);
    static vector<Vector2dd> inverseCurve(vector<Vector2dd> const & curve);
};

} //namespace corecvs
