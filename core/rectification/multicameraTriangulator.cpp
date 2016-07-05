#include "multicameraTriangulator.h"

#include <iostream>
#include "levenmarq.h"
#include "ellipticalApproximation.h"

using std::cout;
using std::endl;

namespace corecvs {


Vector3dd MulticameraTriangulator::triangulate(bool *ok)
{
    hasError = true;
    if (P.size() != xy.size()) {
        if (trace) {
            SYNC_PRINT(("MulticameraTriangulator::triangulate(): P.size != xy.size"));
        }
        if (ok != NULL) *ok = !hasError;
        return Vector3dd(0.0);
    }


    if (P.size() < 2) {
        if (trace) {
            SYNC_PRINT(("MulticameraTriangulator::triangulate(): P.size() < 2"));
        }

        if (ok != NULL) *ok = !hasError;
        return Vector3dd(0.0);
    }

    Matrix A = constructMatrix();
    Matrix Acopy(A);
    if (trace) {
        cout << "Matrix A:" <<  endl << A << endl;
    }

    Matrix W(1, A.w);
    Matrix V(A.w, A.w);
    Matrix::svd(&A, &W, &V);

    Vector2d32 minid = W.getMinCoord();
    Matrix column = V.column(minid.y());
    if (trace) {
        cout << "Matrix A:" <<  endl << A << endl;
        cout << "Matrix W:" <<  endl << W << endl;
        cout << "Matrix V:" <<  endl << V << endl;

        cout << "A Matrix unity check" << endl;
        printUnitySums(A);
        cout << "V Matrix unity check" << endl;
        printUnitySums(V);

        cout << "Minpos" << minid << endl;
        Matrix Wdia(A.w, A.w);
        for (int i = 0; i < A.w; i++)
        {
            Wdia.a(i,i) = W.a(0, i);
            cout << V.a(i, minid.y()) << " ";
        }
        cout << endl;

        //cout << "Reconstructed 0" << endl << (A * W) * V << endl;
        cout << "Reconstructed 1" << endl << (A * Wdia * V.t()) << endl;
        cout << "Diff" << endl << (Acopy - (A * Wdia * V.t())) << endl;

        cout << "Min Column" << endl;
        cout << column << endl;
        double norm = ((Acopy * column).frobeniusNorm());
        cout << "Total error " << norm << " sqrt= " << sqrt(norm) << " columnNorm: " << column.frobeniusNorm() << endl;
    }

    column /= column.a(3, 0);
    Vector3dd result(column.a(0, 0), column.a(1, 0), column.a(2, 0));
    if (trace) {
        cout << result << endl;
    }

    hasError = false;
    if (ok != NULL) *ok = !hasError;

    return result;
}

Matrix MulticameraTriangulator::constructMatrix()
{
    int rows = 3 * (int)P.size();
    int cols = (int)P.size() + 4;

    Matrix A(rows, cols);

    int added = 0;
    for (unsigned i = 0; i < P.size(); i++)
    {
        const Matrix44 &Pi = P[i];

        for (int c1 = 0; c1 < 3; c1++) {
            for (int c2 = 0; c2 < 4; c2++)
            {
                A.a(added * 3 + c1, c2) = -Pi.a(c1, c2);
            }
        }
        A.a(added * 3 + 0, 4 + added) = xy[i].x();
        A.a(added * 3 + 1, 4 + added) = xy[i].y();
        A.a(added * 3 + 2, 4 + added) = 1.0;

        added++;
    }
    return A;
}

void MulticameraTriangulator::printUnitySums(const Matrix &A)
{
    for (int i = 0; i < A.h ; i++)
    {
        double sumsq(0.0);
        for (int j = 0; j < A.w ; j++)
        {
            sumsq += A.a(i,j) * A.a(i,j);
        }
        cout << sumsq << " ";
    }
    cout << endl;
    for (int i = 0; i < A.h ; i++)
    {
        double sumsq(0.0);
        for (int j = 0; j < A.w ; j++)
        {
            sumsq += A.a(j,i) * A.a(j,i);
        }
        cout << sumsq << " ";
    }
    cout << endl;
}

double MulticameraTriangulator::reprojErrorAlgbra(const corecvs::Vector3dd &input)
{
    Matrix A = constructMatrix();
    Matrix column = Matrix(A.w, 1);
    column.a(0, 0) = input[0];
    column.a(1, 0) = input[1];
    column.a(2, 0) = input[2];
    column.a(3, 0) = 1.0;
    for (unsigned i = 0; i < P.size(); i++)
    {
        column.a(i + 4, 0) = (P[i] * input)[2];
    }

    // cout << "column:" << column << endl;
    column /= sqrt(column.frobeniusNorm());
    // cout << "column:" << column << endl;

    double norm = ((A * column).frobeniusNorm());
    // cout << "Total error" << norm << " sqrt= " << sqrt(norm) << endl;
    return sqrt(norm);
}

vector<Vector2dd> MulticameraTriangulator::reprojectionError(const corecvs::Vector3dd &input)
{
    vector<Vector2dd> toReturn;
    for (unsigned i = 0; i < P.size(); i++)
    {
        toReturn.push_back((P[i] * input).project() - xy[i]);
    }
    return toReturn;
}

/*double MulticameraTriangulator::repojError(const corecvs::Vector3dd &input)
{
    Vector2dd sum  (0.0, 0.0);
    Vector2dd sumSq(0.0, 0.0);

    for (unsigned i = 0; i < P.size(); i++)
    {

        Vector2dd v = P[i] * input - xy[i];
        sum += v;
        sumSq += v * v;
    }
    sum   /= P.size();
    sumSq /= P.size();

    return sqrt((sumSq - (sum * sum)).sumAllElements());
}*/

double MulticameraTriangulator::reprojError(const corecvs::Vector3dd &input)
{
    EllipticalApproximation approx;

    for (unsigned i = 0; i < P.size(); i++)
    {
        Vector2dd v = (P[i] * input).project() - xy[i];
        //cout << "MulticameraTriangulator::repojError():" << endl << P[i] << endl;
        //cout << "Pinput" <<  P[i] * input   << endl;
        //cout << "V:" << v << endl;
        approx.addPoint(v);
    }
    return approx.getRadiusAround0();
}

void MulticameraTriangulator::CostFunction::operator()(const double in[], double out[])
{
    Vector3dd input(in[0], in[1], in[2]);
    int count = 0;
    for (unsigned i = 0; i < mTriangulator->P.size(); i++)
    {
        Vector2dd err = (mTriangulator->P[i] * input).project() - mTriangulator->xy[i];
        out[count++] = err.x();
        out[count++] = err.y();
    }
}



Vector3dd MulticameraTriangulator::triangulateLM(Vector3dd initialGuess, bool * /*ok*/)
{
    LevenbergMarquardt LMfit;
    CostFunction F(this);
    LMfit.f = &F;
    LMfit.maxIterations = 1000;
    LMfit.traceProgress = false;

    vector<double> guess(3);
    guess[0] = initialGuess.x();
    guess[1] = initialGuess.y();
    guess[2] = initialGuess.z();

    vector<double> output(F.outputs, 0.0);
    vector<double> optInput = LMfit.fit(guess, output);

    return Vector3dd(optInput[0], optInput[1], optInput[2]);
}

/*
 * Assuming we have an MLE-estimator it (under certain circumstances) is asympthotically-normal
 * So if (and it is the only point that should be used for evaluation) "at" is an extreme value,
 * than hessian is asympthotically unbiased estimate for inverse of covariance matrix
 * For LSQ-estimator JTJ is a good enough estimation of hessian near of extremum
 *
 * NOTE: Due to some reasons on real-life scenes this does not work as expected
 */
corecvs::Matrix33 MulticameraTriangulator::getCovarianceInvEstimation(const corecvs::Vector3dd &at) const
{
    CostFunction f(const_cast<MulticameraTriangulator*>(this));
    auto H = f.getLSQHessian(&at[0]);
    return H;
}

} // namespace corecvs
