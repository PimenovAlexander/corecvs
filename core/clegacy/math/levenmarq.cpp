#include <stdio.h>
#include <string.h>
#include <math.h>

#include "core/utils/global.h"

#include "core/math/levenmarq.h"
#include "stdlib.h"
namespace corecvs {

const int iterations = 100;
const double lambda_factor = 1.5;
const int dimention = 2;
const double minXDif = 5;

//TODO:: ������� ���-�� ����� ���������������� � ������������ �������� �������
/**
 *   Levenberg–Marquardt fillting:
 *
 *   http://en.wikipedia.org/wiki/Levenberg%E2%80%93Marquardt_algorithm
 *
 **/
vector<Vector2dd> LevenMarq::doLevMarFit(vector<Vector2dd> const & distVector)
{
    vector<Vector2dd> distortionVector = distVector;
    int num = distortionVector.size();
    Matrix *D = matrixCreate(num, 1);
    Matrix * J = matrixCreate(num, dimention);
    Matrix *JT = matrixCreate(dimention, num);

    Matrix *A = matrixCreate(dimention, dimention);
    Matrix *Asolve = matrixCreate(dimention, dimention);
    Matrix *B = matrixCreate(dimention, 1);
    Matrix *Bsolve = matrixCreate(dimention, 1);

    vector<Vector2dd> correctVector;

    double lambda = 0.01;
    bool wasInverse = false;
    if (fabs(distortionVector.at(0).x() - distortionVector.back().x()) < minXDif)
    {
        distortionVector = inverseCurve(distVector);
        wasInverse = true;
    }
    double k = (distortionVector.at(0).y() - distortionVector.back().y()) / (distortionVector.at(0).x() - distortionVector.back().x());
    double b = distortionVector. at(0).y() - k * distortionVector.at(0).x();
    D = createMatrix(distortionVector, k, b);
    double norm = dist(D);
    for (int g = 0; g < iterations; g++)
    {
        // Fill the matrices
        for (int i = 0; i < num; i++)
        {
            ELEM(J, i, 0) = distortionVector.at(i).x();
            ELEM(J, i, 1) = 1;
        }
        // Now form the equation
        // (trans(J) * J) * delta = trans(J) * (y - f(x))
        matrixTranspose(J, JT);
        matrixMultiply(JT, J, A);
        //printf("D size is %lf\n", norm);
        matrixMultiply(JT, D, B);
        bool f = false;
        while (lambda < 1e12 && !f)
        {
            matrixCopy(A, Asolve);
            matrixCopy(B, Bsolve);


            for (int j = 0; j < dimention; j++)
            {
                Asolve->data[j * dimention + j] *= (1.0 + lambda );
            }
            matrixSolveGaussian(Asolve, Bsolve);
            double k0 = k + Bsolve->data[0];
            double b0 = b + Bsolve->data[1];
            D = createMatrix(distortionVector, k0, b0);
            double norm1 = dist(D);
            if (norm1 < norm) // If the currnet solution is better
            {
                //printf("Current solution is better");
                lambda /= lambda_factor;
                norm = norm1;
                k = k0;
                b = b0;
                f = true;
            }
            else
                lambda *= lambda_factor;//"Current solution is worse. Try new lambda\n");
        }
    }
    for (int i = 0; i < num; i++)
    {
        double x = distortionVector.at(i).x();
        Vector2dd pnt(x, k * x + b);
        correctVector.push_back(pnt);
    }
    if (wasInverse)
    {
        return inverseCurve(correctVector);
    }
    return correctVector;
}

Matrix * LevenMarq::createMatrix(const vector<Vector2dd> &curve, double k, double b)
{
    Matrix *D = matrixCreate(curve.size(), 1);
    for (unsigned i = 0; i < curve.size(); i++)
    {
        double pntDist = curve.at(i).y() - (k * curve.at(i).x() + b);
        ELEM(D, i, 0) = pntDist;
    }
    return D;
}

double LevenMarq::dist(Matrix *m)
{
    double norm = 0;
    for (int i = 0; i < m->m; i++)
    {
        norm += pow(m->data[i], 2);
    }
    return norm;
}

vector<Vector2dd> LevenMarq::inverseCurve(const vector<Vector2dd> &curve)
{
    vector<Vector2dd> inversedCurve;
    for (unsigned i = 0; i < curve.size(); i++)
    {
        Vector2dd pnt(curve.at(i).y(), curve.at(i).x());
        inversedCurve.push_back(pnt);
    }
    return inversedCurve;
}

} //namespace corecvs

