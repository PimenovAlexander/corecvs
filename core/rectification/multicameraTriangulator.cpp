#include "rectification/multicameraTriangulator.h"
#include "geometry/twoViewOptimalTriangulation.h"
#include "math/levenmarq.h"
#include "geometry/ellipticalApproximation.h"

#include <iostream>

using std::cout;
using std::endl;

namespace corecvs {


Vector3dd MulticameraTriangulator::triangulate(bool *ok)
{
    hasError = true;
    if (P.size() != xy.size())
    {
        if (trace) {
            SYNC_PRINT(("MulticameraTriangulator::triangulate(): P.size != xy.size"));
        }
        if (ok != NULL) *ok = !hasError;
        return Vector3dd(0.0);
    }

    if (P.size() < 2)
    {
        if (trace) {
            SYNC_PRINT(("MulticameraTriangulator::triangulate(): P.size() < 2"));
        }

        if (ok != NULL) *ok = !hasError;
        return Vector3dd(0.0);
    }

    if (P.size() == 2)
        return TwoViewOptimalTriangulor(true, P[0], xy[0], P[1], xy[1]).triangulate();

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
    if (trace)
    {
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

double MulticameraTriangulator::reprojErrorAlgbra(const Vector3dd &input)
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

vector<Vector2dd> MulticameraTriangulator::reprojectionError(const Vector3dd &input)
{
    vector<Vector2dd> toReturn;
    for (unsigned i = 0; i < P.size(); i++)
    {
        toReturn.push_back((P[i] * input).project() - xy[i]);
    }
    return toReturn;
}

/*double MulticameraTriangulator::repojError(const Vector3dd &input)
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

double MulticameraTriangulator::reprojError(const Vector3dd &input)
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

MulticameraTriangulator MulticameraTriangulator::subset(const std::vector<bool> &mask)
{
    MulticameraTriangulator toReturn;
    size_t len = std::min(P.size(), mask.size());
    for (size_t i = 0; i < len; i++)
    {
        if (mask[i])
            toReturn.addCamera(P[i], xy[i]);
    }
    return toReturn;
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


Matrix MulticameraTriangulator::CostFunction::getJacobian(const double in[], double)
{
    Vector3dd input(in[0], in[1], in[2]);
    auto &T= mTriangulator;
    int N = (int)T->P.size();
    Matrix res(N * 2, Vector3dd::LENGTH);
    for (int i = 0; i < N; ++i)
    {
        auto P = T->P[i];
        auto p = P * input;
        auto x = p[0], y = p[1], z = p[2];
        auto z2= z * z;
        auto J = Matrix44(1.0 / z,     0.0, -x / z2, 0.0,
                              0.0, 1.0 / z, -y / z2, 0.0,
                              0.0,     0.0,     0.0, 0.0,
                              0.0,     0.0,     0.0, 0.0);
        auto JP = J * P;
        auto dx = JP * Vector4dd(1.0, 0.0, 0.0, 0.0),
             dy = JP * Vector4dd(0.0, 1.0, 0.0, 0.0),
             dz = JP * Vector4dd(0.0, 0.0, 1.0, 0.0);
        for (int j = 0; j < 2; ++j)
        {
            res.a(i * 2 + j, 0) = dx[j];
            res.a(i * 2 + j, 1) = dy[j];
            res.a(i * 2 + j, 2) = dz[j];
        }
    }
    return res;
}

Matrix MulticameraTriangulator::CostFunction::getLSQHessian(const double *in, double delta)
{
    CORE_UNUSED(delta);

    Matrix H(3, 3);
    Vector3dd input(in[0], in[1], in[2]);
    auto &T = mTriangulator;
    int N = (int)T->P.size();

    for (int i = 0; i < N; ++i)
    {
        auto P = T->P[i];
        auto X = P * input;
        auto U = X.project();
        auto p = T->xy[i];

        double u = p[0], v = p[1], /*X_x = X[0], X_y = X[1],*/ X_z = X[2], U_u = U[0], U_v = U[1];
        double U_v2 = U_v * U_v, U_u2 = U_u * U_u;
        double U_v3 = U_v2* U_v, U_u3 = U_u2* U_u;
        double X_z2 = X_z * X_z;
        double p11 = P.a(0, 0), p12 = P.a(0, 1), p13 = P.a(0, 2), p21 = P.a(1, 0), p22 = P.a(1, 1), p23 = P.a(1, 2), p31 = P.a(2, 0), p32 = P.a(2, 1), p33 = P.a(2, 2);
        double p312 = p31 * p31, p322 = p32 * p32, p332 = p33 * p33;


        // dx2
        H.a(0, 0) += (u - U_u)*(4*p11*p31/X_z2 - 4*p312*U_u3) + (v - U_v)*(4*p21*p31/X_z2 - 4*p312*U_v3) + (-2*p11/(X_z) + 2*p31*U_u2)*(-p11/(X_z) + p31*U_u2) + (-2*p21/(X_z) + 2*p31*U_v2)*(-p21/(X_z) + p31*U_v2);
        // dxdy
        H.a(0, 1) += (u - U_u)*(2*p11*p32/X_z2 + 2*p12*p31/X_z2 - 4*p31*p32*U_u3) + (v - U_v)*(2*p21*p32/X_z2 + 2*p22*p31/X_z2 - 4*p31*p32*U_v3) + (-2*p11/(X_z) + 2*p31*U_u2)*(-p12/(X_z) + p32*U_u2) + (-2*p21/(X_z) + 2*p31*U_v2)*(-p22/(X_z) + p32*U_v2);
        // dxdz
        H.a(0, 2) += (u - U_u)*(2*p11*p33/X_z2 + 2*p13*p31/X_z2 - 4*p31*p33*U_u3) + (v - U_v)*(2*p21*p33/X_z2 + 2*p23*p31/X_z2 - 4*p31*p33*U_v3) + (-2*p11/(X_z) + 2*p31*U_u2)*(-p13/(X_z) + p33*U_u2) + (-2*p21/(X_z) + 2*p31*U_v2)*(-p23/(X_z) + p33*U_v2);
        // dy2
        H.a(1, 1) += (u - U_u)*(4*p12*p32/X_z2 - 4*p322*U_u3) + (v - U_v)*(4*p22*p32/X_z2 - 4*p322*U_v3) + (-2*p12/(X_z) + 2*p32*U_u2)*(-p12/(X_z) + p32*U_u2) + (-2*p22/(X_z) + 2*p32*U_v2)*(-p22/(X_z) + p32*U_v2);
        // dydz
        H.a(1, 2) += (u - U_u)*(2*p12*p33/X_z2 + 2*p13*p32/X_z2 - 4*p32*p33*U_u3) + (v - U_v)*(2*p22*p33/X_z2 + 2*p23*p32/X_z2 - 4*p32*p33*U_v3) + (-2*p12/(X_z) + 2*p32*U_u2)*(-p13/(X_z) + p33*U_u2) + (-2*p22/(X_z) + 2*p32*U_v2)*(-p23/(X_z) + p33*U_v2);
        // dz2
        H.a(2, 2) += (u - U_u)*(4*p13*p33/X_z2 - 4*p332*U_u3) + (v - U_v)*(4*p23*p33/X_z2 - 4*p332*U_v3) + (-2*p13/(X_z) + 2*p33*U_u2)*(-p13/(X_z) + p33*U_u2) + (-2*p23/(X_z) + 2*p33*U_v2)*(-p23/(X_z) + p33*U_v2);
    }
    H *= 0.5;
    H.a(1, 0) = H.a(0, 1);
    H.a(2, 0) = H.a(0, 2);
    H.a(2, 1) = H.a(1, 2);
    return H;
}

Vector3dd MulticameraTriangulator::triangulateLM(Vector3dd initialGuess, bool * /*ok*/)
{
    LevenbergMarquardt LMfit;
    CostFunction F(this);
    LMfit.f = &F;
    LMfit.maxIterations = 1000;
    LMfit.traceProgress = false;
    LMfit.trace = false;

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
 * For LSQ-estimator now we have an symbolically-derived hessian computation
 *
 */
corecvs::Matrix33 MulticameraTriangulator::getCovarianceInvEstimation(const Vector3dd &at) const
{
    CostFunction f(const_cast<MulticameraTriangulator*>(this));
    auto H = f.getLSQHessian(&at[0]);
    return H;
}

} // namespace corecvs
