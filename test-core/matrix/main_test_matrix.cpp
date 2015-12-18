/**
 * \file main_test_matrix.cpp
 * \brief This is the main file for the test matrix
 *
 * \date Aug 21, 2010
 * \author alexander
 *
 * \ingroup autotest
 */

#include <sstream>
#include <iostream>
#include <limits>
#include "gtest/gtest.h"

#include "global.h"

#include "mathUtils.h"
#include "matrix33.h"
#include "matrix.h"
#include "preciseTimer.h"

using namespace std;
using namespace corecvs;

TEST(MatrixTest, testMatrix33)
{
    // Test case 1: Test general constructors
    cout << "Testing Matrix33\n";
    double data[] = {
            1.0,2.0,3.0,
            4.0,5.0,7.0,
            8.0,9.0,10.0 };
    Matrix33 a1(data);
    Matrix33 a2(
            1.0,2.0,3.0,
            4.0,5.0,7.0,
            8.0,9.0,10.0);

    CORE_ASSERT_TRUE(a1 == a2, "Matrix should be equal");
    CORE_ASSERT_FALSE(a1 != a2, "Matrix should not be unequal");


    // Test case 2 : Test inversion and multiplication
    Matrix33 b1 = Matrix33 (
             1,  2,  3,
             0,  1,  4,
             5,  6,  0);
    Matrix33 b1inv = b1.inv();
    Matrix33 b1res = Matrix33(
             -24,  18,   5,
              20, -15,  -4,
              -5,   4,   1 );

    Matrix33 b2 = b1inv * b1;

    CORE_ASSERT_TRUE(b1inv.notTooFar(b1res, 1e-5), "Inversion gives wrong result");
    CORE_ASSERT_TRUE(b2.notTooFar(Matrix33(1.0), 1e-5), "Inversion of inverse should give unit matrix gives wrong result");

    // Test case 3 : Test constructors
    Matrix33 c1 = Matrix33(
             2.0,0.0,0.0,
             0.0,2.0,0.0,
             0.0,0.0,2.0 );
    Matrix33 c2 = Matrix33(2.0);
    CORE_ASSERT_TRUE(c1 == c2, "Matrix should be equal");

    // Test case 4: Determinant
    Matrix33 d1 = Matrix33 (
             1,  2,  3,
             0,  1,  4,
             5,  6,  0);
    double d1det = d1.det();
    double d1res = 1.0;

    Matrix33 d2 = d1;
    d2.transpose();
    double d2det = d2.det();
    double d2res = 1.0;

    Matrix33 d3 = d1 * 0.5;
    double d3det = d3.det();
    double d3res = 0.5 * 0.5 * 0.5;

    Matrix33 d4 = d1 / 0.5;
    double d4det = d4.det();
    double d4res = 2.0 * 2.0 * 2.0;

    Matrix33 d5 = d1.transposed();
    double d5det = d5.det();
    double d5res = 1.0;

    CORE_ASSERT_TRUE_P(d1det < d1res + 1e-5 && d1det > d1res - 1e-5, ("Determinant should be %lf but is %lf", d1res, d1det));
    CORE_ASSERT_TRUE_P(d2det < d2res + 1e-5 && d2det > d2res - 1e-5, ("Determinant should be %lf but is %lf", d2res, d2det));
    CORE_ASSERT_TRUE_P(d3det < d3res + 1e-5 && d3det > d3res - 1e-5, ("Determinant should be %lf but is %lf", d3res, d3det));
    CORE_ASSERT_TRUE_P(d4det < d4res + 1e-5 && d4det > d4res - 1e-5, ("Determinant should be %lf but is %lf", d4res, d4det));
    CORE_ASSERT_TRUE_P(d5det < d5res + 1e-5 && d5det > d5res - 1e-5, ("Determinant should be %lf but is %lf", d5res, d5det));
}

TEST(MatrixTest, testMatrix44)
{
    Matrix44 a = Matrix44(1.0);
    Matrix44 ares = Matrix44(1.0);

    a *= Matrix33::RotationX(0.05);
    a *= Matrix33::RotationY(0.05);
    a *= Matrix33::RotationZ(-0.05);
    a *= Matrix33::RotationZ(0.05);
    a *= Matrix33::RotationY(-0.05);
    a *= Matrix33::RotationX(-0.05);

    cout << a;
    cout << ares;

    CORE_ASSERT_TRUE(a.notTooFar(ares, 1e-5), "Rotation problem\n");
}

TEST(MatrixTest, testMatrixVectorProduct)
{
    Matrix33 a1 = Matrix33 (
             1,  2,  3,
             0,  1,  4,
             5,  6,  0);
    Vector3dd a2 = Vector3dd(1,2,3);

    Vector3dd a3 = a1 * a2;
    Vector3dd a3res = Vector3dd(1 + 4 + 9, 2 + 12, 5 + 12);
    CORE_ASSERT_TRUE(a3.notTooFar(a3res, 1e-5), "Av matrix vector multiplication fails");


    Vector3dd a4 = a2 * a1;
    Vector3dd a4res = Vector3dd(1 + 15, 2 + 2 + 18, 3 + 8);
    CORE_ASSERT_TRUE(a4.notTooFar(a4res, 1e-5), "vA matrix vector multiplication fails");
}

TEST(MatrixTest, testMatrix44VectorProduct)
{
    Matrix44 matrix = Matrix44 (
             2,  0,  0,  0,
             0,  2,  0,  0,
             0,  0,  2,  0,
             0,  0,  2,  0);


    FixedVector<double, 4> vector;
    vector.element[0] = 3.0;
    vector.element[1] = 4.0;
    vector.element[2] = 5.0;
    vector.element[3] = 1.0;
    FixedVector<double, 4> output = matrix * vector;

    cout << "Input :"  << vector << endl;
    cout << "Output:"  << output << endl;
}

TEST(MatrixTest, testMatrixVectorMult)
{
    double vec1data[] = {1.0, 2.0, 0.5};
    Vector a(3, vec1data);

    double vec2data[] = {1.0, 2.0};
    Vector b(2, vec2data);

    double mat1data[] = {
            -1.0, 0.5,  3.0,
             1.0, 1.5, -2.0 };

    Matrix M(2, 3, mat1data);

    Vector res1 = M * a;
    Vector res2 = b * M;

    cout << "M" << endl << M << endl;
    cout << "a" << endl << a << endl;
    cout << res1 << endl;
    cout << res2 << endl;


    double vec1rdata[2] = {1.5, 3.0};
    double vec2rdata[4] = {1.0, 3.5, -1.0};
    Vector vec1r(2, vec1rdata);
    Vector vec2r(3, vec2rdata);
    CORE_ASSERT_TRUE(vec1r.notTooFar(res1, 1e-7), "Wrong matrix x vector");
    CORE_ASSERT_TRUE(vec2r.notTooFar(res2, 1e-7), "Wrong vector x matrix");
}

TEST(MatrixTest, testMatrixStatics)
{
    Vector3dd a(3.0,-4.0,5.0);
    Matrix33 m1 = Matrix33::CrossProductLeft(a);
    Vector3dd b(-1.0,2.0,-2.0);

    Vector3dd c =  a ^ b;
    Vector3dd d = m1 * b;
    CORE_ASSERT_TRUE(c.notTooFar(d, 1e-5), "Matrix cross product left representation fails");

    Matrix33 m2 = Matrix33::CrossProductRight(a);

    Vector3dd c1 =  b ^ a;
    Vector3dd d1 = m2 * b;

    CORE_ASSERT_TRUE(c1.notTooFar(d1, 1e-5), "Matrix cross product right representation fails");
}

TEST(MatrixTest, testMatrixSVD)
{
    cout << "Testing SVD\n";
    // Test case 1: SVD with CMatrix
    double Adata[16] =
    {   7,      9,   10,  7.6,
       -5, 0.0001,    6, 2345,
        6,     45,  234,  -34,
      657, 0.0001,    0,  -1};

    Matrix *A     = new Matrix(4,4, Adata);
    Matrix *Acopy = new Matrix(A);

    Matrix *W     = new Matrix(1,4);
    Matrix *Wbig  = new Matrix(4,4);
    Matrix *V     = new Matrix(4,4);

    Matrix::svd(A, W, V);

    for (int i = 0; i < 4; i ++)
        Wbig->a(i, i) = W->a(0, i);

    Matrix *VT = V->transposed();

    Matrix *result = A->mul(*Wbig);
    Matrix *result1 = result->mul(*VT);

#ifdef TRACE_TEST
    cout << "A Matrix\n";
    A->print();
    cout << "Wbig Matrix\n";
    Wbig->print();
    cout << "V Matrix\n";
    V->print();
    cout << "Reconstruction Matrix\n";
    result1->print();
    cout << "Should be Matrix\n";
    Acopy->print();
#endif

    CORE_ASSERT_TRUE(result1->notTooFar(Acopy, 1e-7), "Matrix should be equal");
    delete A;
    delete Acopy;
    delete W;
    delete Wbig;
    delete V;

    delete result;
    delete result1;
    delete VT;
}

TEST(MatrixTest, testVector3d)
{
    cout << "Testing Vector3dd\n";

    /*
     * Test Case 1: Vector product
     *  [0.54, -0.7, 6] x [5, 0.4, -7] = [2.499999999999999,33.78,3.716]
     */
    Vector3dd v1 = Vector3dd(0.54, -0.7, 6);
    Vector3dd v2 = Vector3dd(5, 0.4, -7);

    Vector3dd v3   = v1 ^ v2;
    Vector3dd v3res(2.499999999999999,33.78,3.716);

    CORE_ASSERT_TRUE(v3.notTooFar(v3res, 1e-7), "Cross product is wrong");

    double angle1 = v3 & v1;
    double angle2 = v3 & v2;

    CORE_ASSERT_TRUE(angle1 < 1e-7 && angle1 > -1e-7, "Cross product must give orthogonal value");
    CORE_ASSERT_TRUE(angle2 < 1e-7 && angle2 > -1e-7, "Cross product must give orthogonal value");

    /* Test case 2 - arithmetic operations */
    Vector3dd in1(0);
    Vector3dd in2(1.0,-2.0, 3.0);
    Vector3dd in3(1.0, 2.0, 3.0);

    double dotProduct = in2 & in3;
    double dotProductRes = 6.0;
    CORE_ASSERT_DOUBLE_EQUAL(dotProduct, dotProductRes, "Wrong dot product\n");

    in1 = in2 * in3;
    Vector3dd prodRes = Vector3dd(1.0, -4.0, 9.0);
    CORE_ASSERT_TRUE(in1.notTooFar(prodRes, 1e-10), "Wrong elementwise product\n")

    in1 = in2 + in3;
    Vector3dd sumRes = Vector3dd(2.0, 0.0, 6.0);
    CORE_ASSERT_TRUE(in1.notTooFar(sumRes, 1e-10), "Wrong elementwise sum\n")
    in1 = in2;
    in1 += in3;
    CORE_ASSERT_TRUE(in1.notTooFar(sumRes, 1e-10), "Wrong elementwise += sum\n")

    in1 = in2 - in3;
    Vector3dd subRes = Vector3dd(0.0, -4.0, 0.0);
    CORE_ASSERT_TRUE(in1.notTooFar(subRes, 1e-10), "Wrong elementwise subtraction\n")
    in1 = in2;
    in1 -= in3;
    CORE_ASSERT_TRUE(in1.notTooFar(subRes, 1e-10), "Wrong elementwise subtraction\n")

    in1 = -in2;
    Vector3dd negRes = Vector3dd(-1.0,2.0,-3.0);
    CORE_ASSERT_TRUE(in1.notTooFar(negRes, 1e-10), "Wrong elementwise negation\n")

    in1 = in2 * 2;
    Vector3dd intconstRes = Vector3dd(2.0,-4.0,6.0);
    CORE_ASSERT_TRUE(in1.notTooFar(intconstRes, 1e-10), "Wrong elementwise int const mul\n")

    in1 = in2 * 2.0;
    Vector3dd constRes = Vector3dd(2.0,-4.0,6.0);
    CORE_ASSERT_TRUE(in1.notTooFar(constRes, 1e-10), "Wrong elementwise const mul\n")
}

TEST(MatrixTest, testVector3dOrtogonal)
{
    Vector3dd in(100, 20, 30);
    Vector3dd ort1;
    Vector3dd ort2;

    in.orthogonal(ort1, ort2);
    ASSERT_TRUE( fabs(!ort1 - 1.0) < 1e-8);
    ASSERT_TRUE( fabs(!ort2 - 1.0) < 1e-8);

    ASSERT_TRUE( fabs((ort1 & in)) < 1e-8);
    ASSERT_TRUE( fabs((ort2 & in)) < 1e-8);
    ASSERT_TRUE( fabs((ort2 & ort1)) < 1e-8);
}


TEST(MatrixTest, testVector2d)
{
    cout << "Testing Vector2d\n";
    Vector2dd v1 = Vector2dd(cos(M_PI / 6.0), sin(M_PI / 6.0));
    Vector2dd v2 = Vector2dd(cos(M_PI / 6.0), -sin(M_PI / 6.0));

    double sine    = v1.sineTo(v2);
    double sineRes = -sqrt(3.0) / 2.0;

    DOTRACE(("Sine is %lg should be %lg\n", sine, sineRes));
    CORE_ASSERT_DOUBLE_EQUAL(sine, sineRes, "Sine calculation failed\n");

    double cosine    = v1.cosineTo(v2);
    double cosineRes = 1.0 / 2.0;
    DOTRACE(("Cosine is %lg should be %lg\n", cosine, sineRes));
    CORE_ASSERT_DOUBLE_EQUAL(cosine, cosineRes, "Cosine calculation failed\n");

    Vector2dd v3 = Vector2dd(2.0, 1.0);
    Vector2dd leftNormal  = Vector2dd(-1.0,  2.0);
    Vector2dd rightNormal = Vector2dd( 1.0, -2.0);

    CORE_ASSERT_TRUE(leftNormal.notTooFar(v3.leftNormal(), 1e-10), "Left  Normal Error\n");
    CORE_ASSERT_TRUE(rightNormal.notTooFar(v3.rightNormal(), 1e-10), "Right Normal Error\n");
}

TEST(MatrixTest, testGeneralMatrixVectorProduct)
{
    Matrix A(3, 3);
    Vector V(3);
    A.a(0, 0) = 1.0; A.a(0, 1) = 2.0; A.a(0, 2) = 3.0;
    A.a(1, 0) = 4.0; A.a(1, 1) =-3.0; A.a(1, 2) =-1.0;
    A.a(2, 0) =-1.0; A.a(2, 1) =-1.0; A.a(2, 2) = 3.0;
    V[0] = 2.0; V[1] = 3.0; V[2] = 4.0;

    Vector AV = A * V;
    Vector VA = V * A;
    ASSERT_NEAR(AV[0], 20.0, 1e-9);
    ASSERT_NEAR(AV[1], -5.0, 1e-9);
    ASSERT_NEAR(AV[2],  7.0, 1e-9);
    ASSERT_NEAR(VA[0], 10.0, 1e-9);
    ASSERT_NEAR(VA[1], -9.0, 1e-9);
    ASSERT_NEAR(VA[2], 15.0, 1e-9);
}

TEST(MatrixTest, testMatrixSerialisation)
{
    Matrix33 m = Matrix33::RotationX(0.1) * Matrix33::RotationY(0.1);
    Matrix33 m1;
    ostringstream oss;
    m.serialise(oss);
    istringstream iss (oss.str(),istringstream::in);
    iss >> m1;
    m.print();
    m1.print();
    CORE_ASSERT_TRUE(m1.notTooFar(m), "serialization fails\n");
}

// XXX: https://connect.microsoft.com/VisualStudio/feedback/details/863099/istringstream-fails-to-convert-certain-double-values-correctly
//      This test will never be working on MS VS 2013
#if _MSC_VER == 1800
TEST(MatrixTest, DISABLED_testDouble)
#else
TEST(MatrixTest, testDouble)
#endif
{
    cout << "Testing Double support\n";
    double vals[] = {numeric_limits<double>::min(), 1.0, numeric_limits<double>::max()};
    for (int i = 0; i < 3; i++)
    {
        ostringstream oss;
        //cout.precision(numeric_limits<double>::digits10 + 3);
        oss.precision(numeric_limits<double>::digits10 + 3);
        oss  << (double)vals[i];
        oss.flush();
        cout << oss.str() << endl;
        istringstream iss (oss.str(), istringstream::in);
        double result;
        iss >> result;
        CORE_ASSERT_TRUE_P(result == vals[i], ("Internal problem with double and stdout"));
    }
}

/* TODO: Use abstract buffer binaryOperation elementWize */
Matrix addMatrix(const Matrix &A, const Matrix &B)
{
    //CORE_ASSERT_TRUE (A.w == B.w && A.h == B.h, "Matrices have wrong sizes");
    Matrix result(2, 2, false);

    for (int row = 0; row < result.h ; row++)
    {
        for (int column = 0; column < result.w ; column++)
        {
            result.a(row, column) = A.a(row, column) + B.a(row, column);
        }
    }

    cout << result;

    return result;
}

TEST(MatrixTest, testMatrixOperations)
{
    /*Matrix result(5,5, false);

    printf("PTR: %p\n", &result);
    cout << result <<  endl;*/


    Matrix I2( 5, 5, 2.0);
    Matrix I3( 5, 5, 3.0);
    Matrix I6(5,5);
    Matrix I6a(5,5);
    I6a = (I6 = I2 * I3);
    cout << "I6" << endl << I6 << endl;

    double Adata[] = {
        1.0, 2.0, 3.0,
        0.0, 1.0, 4.0,
        5.0, 6.0, 0.0
    };

    double invAdata[] = {
        -24.0,  18.0,   5.0,
         20.0, -15.0,  -4.0,
         -5.0,   4.0,   1.0
    };

    Matrix A(3,3, Adata);
    Matrix Aorig(3,3, Adata);
    cout << "Org" << endl << A << endl;

    Matrix invA = A.inv();
    Matrix invAr(3,3, invAdata);
    CORE_ASSERT_TRUE(invA.notTooFar(&invAr, 1e-7), "Invalid inversion");
    CORE_ASSERT_TRUE(A.notTooFar(&Aorig, 1e-7), "inversion was not const");
    cout << "Inv" << endl << invA << endl;

    Matrix invSVDA = A.invSVD();
    CORE_ASSERT_TRUE(invSVDA.notTooFar(&invAr, 1e-7), "Invalid inversion");
    CORE_ASSERT_TRUE(A.notTooFar(&Aorig, 1e-7), "inversion was not const");
    cout << "Inv SVD" << endl << invA << endl;


    Matrix Acopy = A;
    Acopy.transpose();
    Matrix Atrans = A.t();

    cout << "Trans" << endl << Atrans << endl;
    cout << "Trans inplace" << endl << Acopy << endl;

    CORE_ASSERT_TRUE(Acopy.notTooFar(&Atrans, 1e-7), "Invalid transposition");


    /* Test arithmetic operations */
    Matrix Arg1(2, 2);
    Arg1.fillWithArgs(-5.0, 4.0, 3.0, 2.5);
    Matrix Arg2(2, 2);
    Arg2.fillWithArgs(1.0, -2.0, 0.0, -1.5);

    Matrix SumResult(2,2);
    SumResult.fillWithArgs(-4.0, 2.0, 3.0, 1.0);
    CORE_ASSERT_TRUE(addMatrix(Arg1, Arg2).notTooFar(&SumResult, 1e-7), "Invalid addition");
    CORE_ASSERT_TRUE((Arg1 + Arg2).notTooFar(&SumResult, 1e-7), "Invalid addition");

    Matrix SubResult(2,2);
    SubResult.fillWithArgs(-6.0, 6.0, 3.0, 4.0);
    CORE_ASSERT_TRUE((Arg1 - Arg2).notTooFar(&SubResult, 1e-7), "Invalid subtracion");

    /* Scalar multplication and division */

    Matrix ScalarMultResult(2,2);
    ScalarMultResult.fillWithArgs(-10.0, 8.0, 6.0, 5.0);
    Matrix resMul = Arg1 * 2.0;
    cout << "Multiplication real result:"   << endl << resMul << endl;
    cout << "Multiplication exp  result:"   << endl << ScalarMultResult << endl;

    CORE_ASSERT_TRUE(resMul.notTooFar(&ScalarMultResult, 1e-7), "Invalid scalar Multiplication");

    /* In place multplication and division */
    Matrix ScalarDivResult(2,2);
    ScalarDivResult.fillWithArgs(-2.5, 2.0, 1.5, 1.25);

    Matrix Arg1ToDiv(Arg1);
    Matrix Arg1ToMul(Arg1);

    Arg1ToDiv /= 2.0;
    Arg1ToMul /= 0.5;

    cout << "Division result:"   << endl << Arg1ToDiv << endl;
    cout << "Second div result:" << endl << Arg1ToMul << endl;
    cout << flush;
    fflush(stdout);

    CORE_ASSERT_TRUE(Arg1ToDiv.notTooFar(&ScalarDivResult, 1e-7), "Invalid scalar Division1");
    CORE_ASSERT_TRUE(Arg1ToMul.notTooFar(&ScalarMultResult, 1e-7), "Invalid scalar Division2");
}

TEST(MatrixTest, testVectorMatrixConversions)
{
    Matrix44 input(Matrix33::RotationY(0.1), Vector3dd(6,7,8));
    Vector3dd pos(45.0,45.0,45.0);
    Vector2dd shift(4.0,5.0);

 //   Vector2dd out0 = input * pos - shift;
 //   Vector2dd out1 = ((Vector2dd)(input * pos)) - shift;

    Vector3dd product = Vector3dd(1.0,2.0,3.0);
    Vector2dd out2 = (product.project() - shift);
    Vector2dd out3 = product.project() - shift;

    Vector2dd t1   = product.project();
    Vector2dd out4 = t1 - shift;

//    cout << out0  << endl;
//    cout << out1 << endl;
    cout << out2 << endl;
    cout << out3 << endl;
    cout << out4 << endl;
}

/**
 *   We need matrix of 10x10 to trigger vector path
 *
 *
 **/
TEST(MatrixTest, testMatrixProps)
{
    double dataAnan[] = {NAN, 1, 0, 2};
    Matrix Anan(2, 2, dataAnan);


    double dataAinf[] = {5, 1, INFINITY, 2};
    Matrix Ainf(2, 2, dataAinf);

    double dataAok[] = {5, 1, -57245, 2};
    Matrix Aok(2, 2, dataAok);

    ASSERT_TRUE(!Ainf.isFinite());
    ASSERT_TRUE(!Anan.isFinite());
    ASSERT_TRUE( Aok .isFinite());
}

TEST(MatrixTest, test10MatrixMultiply)
{
     Matrix A(10,10);
     auto touch = [](int i, int j, double &el) -> void { el = ((i+1) * (j + 1)) + ((j + 1) / 5.0); };
     A.touchOperationElementwize(touch);

    // cout << A << endl;

     double data[] = {
       554.4, 1016.4,  1478.4, 1940.4, 2402.4, 2864.4, 3326.4,  3788.4,  4250.4,  4712.4,
      1016.4, 1863.4,  2710.4, 3557.4, 4404.4, 5251.4, 6098.4,  6945.4,  7792.4,  8639.4,
      1478.4, 2710.4,  3942.4, 5174.4, 6406.4, 7638.4, 8870.4, 10102.4, 11334.4, 12566.4,
      1940.4, 3557.4,  5174.4, 6791.4, 8408.4,10025.4,11642.4, 13259.4, 14876.4, 16493.4,
      2402.4, 4404.4,  6406.4, 8408.4,10410.4,12412.4,14414.4, 16416.4, 18418.4, 20420.4,
      2864.4, 5251.4,  7638.4,10025.4,12412.4,14799.4,17186.4, 19573.4, 21960.4, 24347.4,
      3326.4, 6098.4,  8870.4,11642.4,14414.4,17186.4,19958.4, 22730.4, 25502.4, 28274.4,
      3788.4, 6945.4, 10102.4,13259.4,16416.4,19573.4,22730.4, 25887.4, 29044.4, 32201.4,
      4250.4, 7792.4, 11334.4,14876.4,18418.4,21960.4,25502.4, 29044.4, 32586.4, 36128.4,
      4712.4, 8639.4, 12566.4,16493.4,20420.4,24347.4,28274.4, 32201.4, 36128.4, 40055.4
     };

     Matrix result(10,10, data);
     Matrix AT = A.t();
     Matrix AAT = A * AT;

     ASSERT_TRUE(AAT. notTooFar(&result, 1e-8, true));

     Matrix AATHpv = Matrix::multiplyHomebrew(A, AT, true, true);
     cout << "AATHpv" << endl << AATHpv << endl;
     cout << "result" << endl << result << endl;
     ASSERT_TRUE(AATHpv. notTooFar(&result, 1e-8, true));

     Matrix AATHp = Matrix::multiplyHomebrew(A, AT, true, false);
     ASSERT_TRUE(AATHp. notTooFar(&result, 1e-8, true));

     Matrix AATHv = Matrix::multiplyHomebrew(A, AT, false, true);
     ASSERT_TRUE(AATHv. notTooFar(&result, 1e-8, true));

     Matrix AATH  = Matrix::multiplyHomebrew(A, AT, false, false);
     ASSERT_TRUE(AATH. notTooFar(&result, 1e-8, true));

}
