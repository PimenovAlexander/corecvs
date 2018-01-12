#include "core/geometry/ellipseFit.h"
#include "core/kalman/cholesky.h"

#include "wrappers/cblasLapack/cblasLapackeWrapper.h"

namespace corecvs {

EllipseFit::EllipseFit()
{

}

void EllipseFit::addPoint(const Vector2dd &point)
{
    points.push_back(point);
}

void EllipseFit::setPointList(const vector<Vector2dd> &pointList)
{
    points.clear();
    points = pointList;
}

/**
 *  This stuff uses a Cholesky decomposition to
 *  solve generalized eigenvalue problem
 *
 * \f[
 *    S a = l C a
 *    U U^T a = l C a
 *    l^(-1) U U^T a = C a
 *    l^(-1) U^T a  = U^(-1) C U^(-1)^T  U^T a
 *    l^(-1)(U^T a) = U^(-1) C U^(-1)^T (U^T a)
 * \f]
 *
 *
 **/
SecondOrderCurve EllipseFit::solveDummy()
{
    Matrix C(6,6,0.0);
    C.a(0,2) = 2;
    C.a(1,1) =-1;
    C.a(2,0) = 2;

    CORE_ASSERT_TRUE(((uint)points.size()) == points.size(), ("too much points for EllipseFit"));
    Matrix dD((uint)points.size(), 6);
    for (vector<Vector2dd>::size_type i = 0; i < points.size(); i++) {
        Vector2dd p = points[i];
        dD.fillLineWithArgs((uint)i, p.x() * p.x(), p.x() * p.y(), p.y() * p.y(), p.x(), p.y(), 1.0 );
    }

    Matrix S = dD.t() * dD;

    Matrix *U;
    Cholesky::uutDecompose(&S, &U);

    Matrix Right = U->inv() * C  * U->inv().t();

    Matrix A(Right);
    DiagonalMatrix D(A.h);
    Matrix V(A.h, A.w);
    Matrix::jacobi(&A, &D, &V, NULL);

    SecondOrderCurve result;

    for (int i = 0; i < 6; i++)
    {
        if (D.a(i) > 0) {
           // cout << "Positive eigen " << i << endl;
            Matrix rev =  U->t().inv() * V.column(i);

            result.a11() = rev.a(0,0);
            result.a22() = rev.a(2,0);
            result.a12() = rev.a(1,0) / 2;
            result.a13() = rev.a(3,0) / 2;
            result.a23() = rev.a(4,0) / 2;
            result.a33() = rev.a(5,0);
            break;
        }
    }

    return result;

}

#ifdef WITH_BLAS
SecondOrderCurve EllipseFit::solveBLAS()
{
    Matrix C(6,6,0.0);
    C.a(0,2) = 2;
    C.a(1,1) =-1;
    C.a(2,0) = 2;

    CORE_ASSERT_TRUE(((uint)points.size()) == points.size(), ("too much points for EllipseFit"));
    Matrix dD((uint)points.size(), 6);
    for (vector<Vector2dd>::size_type i = 0; i < points.size(); i++) {
        Vector2dd p = points[i];
        dD.fillLineWithArgs((uint)i, p.x() * p.x(), p.x() * p.y(), p.y() * p.y(), p.x(), p.y(), 1.0 );
    }

    Matrix S = dD.t() * dD;

    double er[6];
    double ei[6];
    double beta[6];

    Matrix VL(6, 6);
    Matrix VR(6, 6);

    cout << "VL.stride  :" << VL.stride << endl;
    cout << "VL.width   :" << VL.w      << endl;

    /**
         We should check alignment problems
     **/
    Matrix Sblas = S;
    Matrix Cblas = C;


    LAPACKE_dggev(LAPACK_ROW_MAJOR, 'N', 'V', 6, Sblas.data, 6, Cblas.data, 6, er, ei, beta, VL.data, 6, VR.data, 6);

    for (int i = 0; i < 6; i++)
    {
        cout << (er[i] / beta[i]) << endl;
    }

    cout << "VL" << endl << VL << endl;
    cout << "VR" << endl << VR << endl;

#ifdef DEBUG
    cout << "Sanity Check" << endl;
    for (int i = 0; i < 6; i++)
    {
        cout << "Checking vector: " << i << endl;
        //Matrix rev =  VR.row(i).t();
        Matrix rev =  VR.column(i);

        Matrix SR = S * rev;
        Matrix CR = C * rev;

        cout << "S * rev" << endl << SR << endl;
        cout << "C * rev" << endl << CR << endl;
        /*for (int j = 0; j < 6; j++)
            cout << "Sanity: " << SR.a(j,0) / CR.a(j,0) << endl;*/
        cout << "Sanity: " << (SR * beta[i] - CR * er[i]) << endl;
    }
#endif


    SecondOrderCurve result;

    for (int i = 0; i < 6; i++)
    {
        if (er[i] / beta[i] > 0) {
            cout << "Positive eigen " << i << endl;
            Matrix rev =  VR.column(i);
            cout << "Rev" << endl << rev << endl;

            result.a11() = rev.a(0,0);
            result.a22() = rev.a(2,0);
            result.a12() = rev.a(1,0) / 2;
            result.a13() = rev.a(3,0) / 2;
            result.a23() = rev.a(4,0) / 2;
            result.a33() = rev.a(5,0);
            break;
        }
    }

    result.prettyPrint();

    return result;

}
#endif

SecondOrderCurve EllipseFit::solveQuadricDummy()
{
    CORE_ASSERT_TRUE(((uint)points.size()) == points.size(), ("too much points for EllipseFit"));
    Matrix dD((uint)points.size(), 6);
    for (vector<Vector2dd>::size_type i = 0; i < points.size(); i++) {
        Vector2dd p = points[i];
        dD.fillLineWithArgs((uint)i, p.x() * p.x(), p.x() * p.y(), p.y() * p.y(), p.x(), p.y(), 1.0 );
    }

    Matrix S = dD.t() * dD;

    Matrix A(S);
    DiagonalMatrix D(A.h);
    Matrix V(A.h, A.w);
    Matrix::jacobi(&A, &D, &V, NULL);

    SecondOrderCurve result;


    int minnum = 0;
    double value = D.a(0);
    for (int i = 1; i < 6; i++)
    {
        if (D.a(i) < value) {
            minnum = i;
            value = D.a(i);
        }
    }

    Matrix rev = V.column(minnum);
    result.a11() = rev.a(0,0);
    result.a22() = rev.a(2,0);
    result.a12() = rev.a(1,0) / 2;
    result.a13() = rev.a(3,0) / 2;
    result.a23() = rev.a(4,0) / 2;
    result.a33() = rev.a(5,0);

    return result;
}


} // namespace corecvs

