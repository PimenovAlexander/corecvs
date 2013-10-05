/**
 * \file main_test_matrix.cpp
 * \brief This is the main file for the test matrix
 *
 * \date Aug 21, 2010
 * \author alexander
 *
 * \ingroup autotest
 */

#ifndef ASSERTS
#define ASSERTS
#endif

#include <sstream>
#include <iostream>
#include <limits>

#include "global.h"

#include "mathUtils.h"
#include "matrix33.h"
#include "matrix.h"

using namespace std;
using namespace corecvs;

void testMatrix33 ( void )
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

    ASSERT_TRUE  (a1 == a2, "Matrix should be equal");
    ASSERT_FALSE (a1 != a2, "Matrix should not be unequal");


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

    ASSERT_TRUE(b1inv.notTooFar(b1res, 1e-5), "Inversion gives wrong result");
    ASSERT_TRUE(b2.notTooFar(Matrix33(1.0), 1e-5), "Inversion of inverse should give unit matrix gives wrong result");

    // Test case 3 : Test constructors
    Matrix33 c1 = Matrix33(
             2.0,0.0,0.0,
             0.0,2.0,0.0,
             0.0,0.0,2.0 );
    Matrix33 c2 = Matrix33(2.0);
    ASSERT_TRUE  (c1 == c2, "Matrix should be equal");

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


    ASSERT_TRUE_P(d1det < d1res + 1e-5 && d1det > d1res - 1e-5, ("Determinant should be %lf but is %lf", d1res, d1det));
    ASSERT_TRUE_P(d2det < d2res + 1e-5 && d2det > d2res - 1e-5, ("Determinant should be %lf but is %lf", d2res, d2det));
    ASSERT_TRUE_P(d3det < d3res + 1e-5 && d3det > d3res - 1e-5, ("Determinant should be %lf but is %lf", d3res, d3det));
    ASSERT_TRUE_P(d4det < d4res + 1e-5 && d4det > d4res - 1e-5, ("Determinant should be %lf but is %lf", d4res, d4det));
    ASSERT_TRUE_P(d5det < d5res + 1e-5 && d5det > d5res - 1e-5, ("Determinant should be %lf but is %lf", d5res, d5det));

}

void testMatrix44 ( void )
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

    ASSERT_TRUE(a.notTooFar(ares, 1e-5), "Rotation problem\n");
}

void testMatrixVectorProduct (void)
{
    Matrix33 a1 = Matrix33 (
             1,  2,  3,
             0,  1,  4,
             5,  6,  0);
    Vector3dd a2 = Vector3dd(1,2,3);

    Vector3dd a3 = a1 * a2;
    Vector3dd a3res = Vector3dd(1 + 4 + 9, 2 + 12, 5 + 12);
    ASSERT_TRUE(a3.notTooFar(a3res, 1e-5), "Av matrix vector multiplication fails");


    Vector3dd a4 = a2 * a1;
    Vector3dd a4res = Vector3dd(1 + 15, 2 + 2 + 18, 3 + 8);
    ASSERT_TRUE(a4.notTooFar(a4res, 1e-5), "vA matrix vector multiplication fails");

}

void testMatrix44VectorProduct (void)
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


void testMatrixGaussianSolve (void)
{

}


void testMatrixVectorMult (void)
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
    ASSERT_TRUE(vec1r.notTooFar(res1, 1e-7), "Wrong matrix x vector");
    ASSERT_TRUE(vec2r.notTooFar(res2, 1e-7), "Wrong vector x matrix");
}


void testMatrixStatics (void)
{
    Vector3dd a(3.0,-4.0,5.0);
    Matrix33 m1 = Matrix33::CrossProductLeft(a);
    Vector3dd b(-1.0,2.0,-2.0);

    Vector3dd c =  a ^ b;
    Vector3dd d = m1 * b;
    ASSERT_TRUE(c.notTooFar(d, 1e-5), "Matrix cross product left representation fails");

    Matrix33 m2 = Matrix33::CrossProductRight(a);

    Vector3dd c1 =  b ^ a;
    Vector3dd d1 = m2 * b;

    ASSERT_TRUE(c1.notTooFar(d1, 1e-5), "Matrix cross product right representation fails");
}


void testMatrixSVD (void)
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

    ASSERT_TRUE(result1->notTooFar(Acopy, 1e-7), "Matrix should be equal" );
    delete A;
    delete Acopy;
    delete W;
    delete Wbig;
    delete V;

    delete result;
    delete result1;
    delete VT;
}





void testVector3d (void)
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

    ASSERT_TRUE(v3.notTooFar(v3res, 1e-7),"Cross product is wrong");

    double angle1 = v3 & v1;
    double angle2 = v3 & v2;

    ASSERT_TRUE(angle1 < 1e-7 && angle1 > -1e-7, "Cross product must give orthogonal value");
    ASSERT_TRUE(angle2 < 1e-7 && angle2 > -1e-7, "Cross product must give orthogonal value");

    /* Test case 2 - arithmetic operations */
    Vector3dd in1(0);
    Vector3dd in2(1.0,-2.0, 3.0);
    Vector3dd in3(1.0, 2.0, 3.0);

    double dotProduct = in2 & in3;
    double dotProductRes = 6.0;
    ASSERT_DOUBLE_EQUAL(dotProduct, dotProductRes, "Wrong dot product\n");

    in1 = in2 * in3;
    Vector3dd prodRes = Vector3dd(1.0, -4.0, 9.0);
    ASSERT_TRUE(in1.notTooFar(prodRes, 1e-10), "Wrong elementwise product\n")

    in1 = in2 + in3;
    Vector3dd sumRes = Vector3dd(2.0, 0.0, 6.0);
    ASSERT_TRUE(in1.notTooFar(sumRes, 1e-10), "Wrong elementwise sum\n")
    in1 = in2;
    in1 += in3;
    ASSERT_TRUE(in1.notTooFar(sumRes, 1e-10), "Wrong elementwise += sum\n")

    in1 = in2 - in3;
    Vector3dd subRes = Vector3dd(0.0, -4.0, 0.0);
    ASSERT_TRUE(in1.notTooFar(subRes, 1e-10), "Wrong elementwise subtraction\n")
    in1 = in2;
    in1 -= in3;
    ASSERT_TRUE(in1.notTooFar(subRes, 1e-10), "Wrong elementwise subtraction\n")

    in1 = -in2;
    Vector3dd negRes = Vector3dd(-1.0,2.0,-3.0);
    ASSERT_TRUE(in1.notTooFar(negRes, 1e-10), "Wrong elementwise negation\n")

    in1 = in2 * 2;
    Vector3dd intconstRes = Vector3dd(2.0,-4.0,6.0);
    ASSERT_TRUE(in1.notTooFar(intconstRes , 1e-10), "Wrong elementwise int const mul\n")


    in1 = in2 * 2.0;
    Vector3dd constRes = Vector3dd(2.0,-4.0,6.0);
    ASSERT_TRUE(in1.notTooFar(constRes , 1e-10), "Wrong elementwise const mul\n")

}

void testVector2d (void)
{
    cout << "Testing Vector2d\n";
    Vector2dd v1 = Vector2dd(cos(M_PI / 6.0), sin(M_PI / 6.0));
    Vector2dd v2 = Vector2dd(cos(M_PI / 6.0), -sin(M_PI / 6.0));

    double sine    = v1.sineTo(v2);
    double sineRes = -sqrt(3.0) / 2.0;

    DOTRACE(("Sine is %lg should be %lg\n", sine, sineRes));
    ASSERT_DOUBLE_EQUAL(sine, sineRes, "Sine calculation failed\n");

    double cosine    = v1.cosineTo(v2);
    double cosineRes = 1.0 / 2.0;
    DOTRACE(("Cosine is %lg should be %lg\n", cosine, sineRes));
    ASSERT_DOUBLE_EQUAL(cosine, cosineRes, "Cosine calculation failed\n");

    Vector2dd v3 = Vector2dd(2.0, 1.0);
    Vector2dd leftNormal  = Vector2dd(-1.0,  2.0);
    Vector2dd rightNormal = Vector2dd( 1.0, -2.0);

    ASSERT_TRUE(leftNormal .notTooFar(v3.leftNormal()  , 1e-10), "Left  Normal Error\n");
    ASSERT_TRUE(rightNormal.notTooFar(v3.rightNormal() , 1e-10), "Right Normal Error\n");
}

void testMatrixSerialisation (void)
{
    Matrix33 m = Matrix33::RotationX(0.1) * Matrix33::RotationY(0.1);
    Matrix33 m1;
    ostringstream oss;
    m.serialise(oss);
    istringstream iss (oss.str(),istringstream::in);
    iss >> m1;
    m.print();
    m1.print();
    ASSERT_TRUE(m1.notTooFar(m), "serialization fails\n");
}

void testDouble (void)
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
        istringstream iss (oss.str(),istringstream::in);
        double result;
        iss >> result;
        ASSERT_TRUE_P(result == vals[i], ("Internal problem with double and stdout"));

    }
}

/* TODO: Use abstract buffer binaryOperation elementWize */
Matrix addMatrix(const Matrix &A, const Matrix &B)
{
    //ASSERT_TRUE (A.w == B.w && A.h == B.h, "Matrices have wrong sizes");
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

void testMatrixOperations (void)
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
    ASSERT_TRUE(invA.notTooFar(&invAr, 1e-7), "Invalid inversion");
    ASSERT_TRUE(A.notTooFar(&Aorig, 1e-7), "inversion was not const");
    cout << "Inv" << endl << invA << endl;

    Matrix invSVDA = A.invSVD();
    ASSERT_TRUE(invSVDA.notTooFar(&invAr, 1e-7), "Invalid inversion");
    ASSERT_TRUE(A.notTooFar(&Aorig, 1e-7), "inversion was not const");
    cout << "Inv SVD" << endl << invA << endl;


    Matrix Acopy = A;
    Acopy.transpose();
    Matrix Atrans = A.t();

    cout << "Trans" << endl << Atrans << endl;
    cout << "Trans inplace" << endl << Acopy << endl;

    ASSERT_TRUE(Acopy.notTooFar(&Atrans, 1e-7), "Invalid transposition");


    /* Test arithmetic operations */
    Matrix Arg1(2, 2);
    Arg1.fillWithArgs(-5.0, 4.0, 3.0, 2.5);
    Matrix Arg2(2, 2);
    Arg2.fillWithArgs(1.0, -2.0, 0.0, -1.5);

    Matrix SumResult(2,2);
    SumResult.fillWithArgs(-4.0, 2.0, 3.0, 1.0);
    ASSERT_TRUE(addMatrix(Arg1,Arg2).notTooFar(&SumResult, 1e-7), "Invalid addition");
    ASSERT_TRUE((Arg1 + Arg2).notTooFar(&SumResult, 1e-7), "Invalid addition");

    Matrix SubResult(2,2);
    SubResult.fillWithArgs(-6.0, 6.0, 3.0, 4.0);
    ASSERT_TRUE((Arg1 - Arg2).notTooFar(&SubResult, 1e-7), "Invalid subtracion");


}

int main (int /*argC*/, char ** /*argV*/)
{
    cout << "Testing " << endl;
    //testMatrixVectorMult();
    //testMatrixOperations();
    testMatrix44VectorProduct();
    return 0;
    //return 0;
    testMatrix44();
    testDouble();
    testMatrixSerialisation();
    testMatrixVectorProduct  ();
    testMatrix33 ();
    testMatrixStatics();
    testMatrixSVD ();
    testVector3d ();
    testVector2d ();

    cout << "PASSED" << endl;
    return 0;
}
