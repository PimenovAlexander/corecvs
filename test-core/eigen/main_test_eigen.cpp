/**
 * \file main_test_eigen.cpp
 * \brief This is the main file for the test eigen 
 *
 * \date февр. 13, 2015
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include <random>

#include <fenv.h>


#include "gtest/gtest.h"

#include "core/utils/global.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/math/matrix/matrix.h"
#include "core/math/matrix/matrix22.h"
#include "core/math/matrix/covMatrix22.h"

#include "core/fileformats/bmpLoader.h"
#include "core/kalman/cholesky.h"

using namespace corecvs;

TEST(Eigen, testJacobi)
{
    Matrix A(4, 4);

    A.a(0,0) = 9;
    A.a(1,0) =-6; A.a(1,1) =-2;
    A.a(2,0) = 7; A.a(2,1) =-2; A.a(2,2) =-4;
    A.a(3,0) = 1; A.a(3,1) = 4; A.a(3,2) = 19; A.a(3,3) = 6;

    for (int i = 0; i < A.h; i++)
    {
        for (int j = i; j < A.w; j++)
        {
            A.a(i,j) = A.a(j,i);
        }
    }

    cout << "Matrix A:" << endl;
    A.print(cout);
    cout << endl;

    Matrix A1(A);
    DiagonalMatrix D(A.h);
    Matrix V(A.h, A.w);
    Matrix::jacobi(&A1, &D, &V, NULL);
    cout << "Eigen values:" << D << endl;
    cout << "Eigen vectors:" << V << endl;

    cout << endl;
    cout << "================" << endl;
    cout << endl;

    for (int i = 0; i < V.h; i++)
    {
        Matrix T(A.h, 1);
        Matrix T1(A.h, 1);
        for (int j = 0; j < V.w; j++)
        {
//            T.a(j, 0) = V.a(i,j);
//            T1.a(j, 0) = V.a(i,j) /  V.a(i,3);
            T .a(j, 0) = V.a(j,i);
            T1.a(j, 0) = V.a(j,i) /  V.a(3,i);

        }
        cout << T << endl;
        cout << T1 << endl;
        cout << "=--------------=" << endl;
        cout << A * T << endl;
        cout << D.a(i) * T << endl;
        cout << "================" << endl;
    }
}


TEST(Eigen, testJacobiConverge)
{
    /*
    double data[] =
             {
              1.440000000000002e+02, -4.092000000000002e+03, -7.080000000000003e+02,
             -4.092000000000002e+03,  1.162820000000000e+05,  2.011900000000000e+04,
             -7.080000000000003e+02,  2.011900000000000e+04,  3.482000000000000e+03 };
    double data[] =
    {
        7.48030000000000000e+04, 7.99990000000000000e+04, 3.70590000000000000e+04,
        7.99990000000000000e+04, 8.55570000000000000e+04, 3.96340000000000000e+04,
        3.70590000000000000e+04, 3.96340000000000000e+04, 1.83610000000000000e+04
    };    */

    double data[] =
    {
         9.23705556488130115e-14, -5.32907051820075139e-15, 3.73034936274052598e-14,
        -5.32907051820075139e-15,  2.50000000000000000e-01, 0.00000000000000000e+00,
         3.73034936274052598e-14,  0.00000000000000000e+00, 2.50000000000000000e-01
    };

    //feenableexcept(FE_INVALID | FE_OVERFLOW);

    Matrix A(3, 3, data);

    cout << "Matrix A:" << endl;
    cout << A;
    cout << endl;

    Matrix A1(A);
    DiagonalMatrix D(A.h);
    Matrix V(A.h, A.w);
    int res = Matrix::jacobi(&A1, &D, &V, NULL, true);

    if (res != 0) {
        cout << "Not Converged";
        CORE_ASSERT_FAIL("Not Converged");
    }

    cout << "Eigen values:" << D << endl;
    cout << "Eigen vectors:" << V << endl;

    cout << endl;
    cout << "================" << endl;
    cout << endl;

}

TEST(Eigen, testMatrix22)
{
    {
        GenericMatrix22<float> A(22, 0, 0 ,0);
        float l1, l2;
        Vector2df d1, d2;
        GenericMatrix22<float>::eigen(A, l1, d1, l2, d2);
        cout << "Matrix :\n" << A;
        cout << "Eigen 1:" << l1 << " " << d1 << endl;
        cout << "Eigen 2:" << l2 << " " << d2 << endl;

        GenericMatrix22<float> Arec(0.0);

        Arec += l1 * GenericMatrix22<float>::VectorByVector(d1, d1);
        Arec += l2 * GenericMatrix22<float>::VectorByVector(d2, d2);

        cout << "Matrix1:\n" << Arec;
        CORE_ASSERT_TRUE(Arec.notTooFar(A, 1e-7), "failed decomp");
    }
    cout << endl;

    {
        GenericMatrix22<float> A(0, 0, 0, 22);
        float l1, l2;
        Vector2df d1, d2;
        GenericMatrix22<float>::eigen(A, l1, d1, l2, d2);
        cout << "Matrix :\n" << A;
        cout << "Eigen 1:" << l1 << " " << d1 << endl;
        cout << "Eigen 2:" << l2 << " " << d2 << endl;

        GenericMatrix22<float> Arec(0.0);

        Arec += l1 * GenericMatrix22<float>::VectorByVector(d1, d1);
        Arec += l2 * GenericMatrix22<float>::VectorByVector(d2, d2);

        cout << "Matrix1:\n" << Arec;
        CORE_ASSERT_TRUE(Arec.notTooFar(A, 1e-7), "failed decomp");
    }
}

TEST(Eigen, testCovarianceMatrix22)
{

    CovMatrix22f A0(0);
    A0.addVector(2,1);

    CovMatrix22f A1(0);
    A1.addVector(0,1);

    CovMatrix22f A2(0);
    A2.addVector(1,0);

    CovMatrix22f A3(0);
    A3.addVector(1,1);
    A3.addVector(-1,1);

    CovMatrix22f *m[] = {&A0, &A1, &A2, &A3};

    for (int i = 0; i < 4; i++)
    {
        CovMatrix22f& A = *m[i];

        float l1, l2;
        Vector2df d1, d2;
        A.eigen(l1, d1, l2, d2);

        cout << "Matrix :\n" << A;
        cout << "Eigen 1:" << l1 << " " << d1 << endl;
        cout << "Eigen 2:" << l2 << " " << d2 << endl;

        Matrix22f M = A.toMatrix22();
        Matrix22f M1 = M - l1 * Matrix22f::Identity();
        Matrix22f M2 = M - l2 * Matrix22f::Identity();

        cout << "M1:\n" << M1;
        cout << "M2:\n" << M2;

        cout << (M * d1) << " = " << l1 * d1 << endl;
        cout << (M * d2) << " = " << l2 * d2 << endl;
        CORE_ASSERT_TRUE_P((M * d1).notTooFar(l1 * d1), ("Wrong eigen1 at test %d", i));
        CORE_ASSERT_TRUE_P((M * d2).notTooFar(l2 * d2), ("Wrong eigen2 at test %d", i));
    }


}


TEST(Eigen, testEllipse)
{
    RGB24Buffer *buffer = new RGB24Buffer(200, 200);

    Matrix C(6,6,0.0);
    C.a(0,2) = 2;
    C.a(1,1) =-1;
    C.a(2,0) = 2;

    vector<Vector2dd> inputs;
    std::mt19937 rng;
    std::uniform_real_distribution<double> runif(-7, 7);


    for (int i = 0; i < 20; i++) {
        Vector2dd point = Vector2dd::FromPolar(i / 10.0, 40.0) * Vector2dd(1.0, 2.0);
        point = Matrix33::RotationZ(degToRad(30)) * point;
        point += Vector2dd(100 + runif(rng),100 + runif(rng));

        inputs.push_back(point);
        buffer->element(point.y(), point.x()) = RGBColor::Green();
    }

    /* Straitforward so far*/
    CORE_ASSERT_TRUE(((uint)inputs.size()) == inputs.size(), ("too much points in testEllipse"));
    Matrix dD((uint)inputs.size(), 6);
    for (vector<Vector2dd>::size_type i = 0; i < inputs.size(); i++) {
        Vector2dd p = inputs[i];
        dD.fillLineWithArgs((uint)i, p.x() * p.x(), p.x() * p.y(), p.y() * p.y(), p.x(), p.y(), 1.0 );
    }

    Matrix S = dD.t() * dD;


    Matrix *U;
    Cholesky::uutDecompose(&S, &U);
    cout << *U << endl;

    cout << "Sanity Check0" << endl;
    cout << (*U) * U->t() << endl;

    // S a = l C a
    // U U^T a = l C a
    // l^(-1) U U^T a = C a
    // l^(-1) U^T a  = U^(-1) C U^(-1)^T  U^T a
    // l^(-1)(U^T a) = U^(-1) C U^(-1)^T (U^T a)

    Matrix Right = U->inv() * C  * U->inv().t();

    cout <<    "S" << endl <<    S << endl;
    cout <<    "C" << endl <<    C << endl;
    cout << "Right" << endl << Right << endl;

    Matrix A(Right);
    DiagonalMatrix D(A.h);
    Matrix V(A.h, A.w);
    Matrix::jacobi(&A, &D, &V, NULL);
    cout << "Sanity Check1" << endl;
    cout << V * D * V.t() << endl;

    cout << "Sanity Check2"   << endl;
    cout << "Eigen values:"   << endl << D << endl;
    cout << "Eigen vectors0:" << endl << V << endl;

    for (int i = 0; i < 6; i++)
    {
        cout << "Check2.1" << endl;
        cout << (Right * V.column(i)).t() << endl;
        cout << "Check2.2" << endl;
        cout << (V.column(i) * D.a(i)).t() << endl;

        Matrix rev =  U->t().inv() * V.column(i);

        cout << "Check2.3" << endl;
        cout << (Right * U->t() * rev).t() << endl;
        cout << "Check2.4" << endl;
        cout << (U->t() * rev * D.a(i)).t() << endl;
        cout << "Check2.5" << endl;
        cout << (U->inv() * C  * U->inv().t() * U->t() * rev).t() << endl;
        cout << "Check2.6" << endl;
        cout << (U->inv() * C  * rev).t() << endl;

        cout << "Check3.1" << endl;
        cout << ((*U) * U->t() * rev * D.a(i)).t() << endl;
        cout << "Check3.2" << endl;
        cout << (C  * rev).t() << endl;
        cout << "Check3.3" << endl;
        cout << (S * rev * D.a(i) ).t() << endl;

    }



    cout << "Sanity Check" << endl;
    cout << "UTI" << endl;
    cout << U->t().inv();
    cout << "UIT" << endl;
    cout << U->inv().t();


    for (int i = 0; i < 6; i++)
    {
        cout << i << endl;
        Matrix column = U->t().inv() * V.column(i);
        Matrix L = (S * column).t();
        Matrix R = (C * column).t();
        cout << L << endl;
        cout << R << endl;
        for (int j = 0; j < 6; j++) {
            cout << (L.a(0,j) / R.a(0,j)) << " ";
        }
        cout << endl;
    }



    vector<double> result(6);
    for (int i = 0; i < 6; i++)
    {
        if (D.a(i) > 0) {
            cout << "Positive eigen " << i << endl;
            Matrix rev =  U->t().inv() * V.column(i);

            for (int j = 0; j < 6; j++) {
                result[j] = rev.a(j,0);
            }
            break;
        }
    }

    {
        double A = result[0];
        double B = result[1];
        double C = result[2];
        double D = result[3];
        double E = result[4];
        double F = result[5];

        cout << A << " " << B << " " << C << " " << D << " "<< E << " " << F << " " << endl;

        double Det = B * B - 4 * A * C;
        double T = (A * E * E + C * D * D - B * D * E + Det * F);
        double R = sqrt((A - C) * (A - C)  + B * B);
        double a = - sqrt(2 * T * (A + C + R)) / Det;
        double b = - sqrt(2 * T * (A + C - R)) / Det;

        double xc = (2 * C * D - B * E) / Det;
        double yc = (2 * A * E - B * D) / Det;
        double tetta = atan2(C - A - R, B);

        cout << "xc = " << xc << endl;
        cout << "yc = " << yc << endl;

        cout << "a = " << a << endl;
        cout << "b = " << b << endl;
        cout << "tetta = " << tetta << endl;

        int MAX = 850;
        for (int k = 0; k < MAX; k++)
        {
            double v = 2 * M_PI / MAX * k;
            Vector2dd point = Vector2dd(a * cos(v), b * sin(v));
            point = Matrix33::RotationZ(tetta) * point;
            point += Vector2dd(xc,yc);
            buffer->element(point.y(), point.x()) = RGBColor::Yellow();
        }

        for (size_t i = 0; i < inputs.size(); i++) {
            Vector2dd point = inputs[i];
            buffer->drawCrosshare3(point,RGBColor::Red());
        }


    }




    BMPLoader().save("ellipse.bmp", buffer);

}

