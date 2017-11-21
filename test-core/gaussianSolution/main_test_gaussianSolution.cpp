#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/math/matrix/matrix.h"

using namespace corecvs;


TEST(GaussianSolution, CorrectSolution)
{
    double a = 1;
    double b = 0;
    double c = 0;
    double d = 0;
    double derAY = 0;
    double derCY = 0;
    double derBY = 0;
    double derDY = 0;
    double derAX = 0;
    double derBX = 0;
    double derCX = 0;
    double derDX = 0;

    Matrix * A = new Matrix(16, 16);
    Matrix * B = new Matrix(16, 1);

    A->a(0, 0) = 0;
    A->a(0, 1) = 0;
    A->a(0, 2) = 0;
    A->a(0, 3) = 0;
    A->a(0, 4) = 0;
    A->a(0, 5) = 0;
    A->a(0, 6) = 0;
    A->a(0, 7) = 0;
    A->a(0, 8) = 0;
    A->a(0, 9) = 0;
    A->a(0, 10) = 0;
    A->a(0, 11) = 0;
    A->a(0, 12) = 0;
    A->a(0, 13) = 0;
    A->a(0, 14) = 0;
    A->a(0, 15) = 1;

    B->a(0, 0) = a;

    A->a(1, 0) = 0;
    A->a(1, 1) = 0;
    A->a(1, 2) = 0;
    A->a(1, 3) = 1;
    A->a(1, 4) = 0;
    A->a(1, 5) = 0;
    A->a(1, 6) = 0;
    A->a(1, 7) = 1;
    A->a(1, 8) = 0;
    A->a(1, 9) = 0;
    A->a(1, 10) = 0;
    A->a(1, 11) = 1;
    A->a(1, 12) = 0;
    A->a(1, 13) = 0;
    A->a(1, 14) = 0;
    A->a(1, 15) = 1;

    B->a(1, 0) = b;

    A->a(2, 0) = 0;
    A->a(2, 1) = 0;
    A->a(2, 2) = 0;
    A->a(2, 3) = 0;
    A->a(2, 4) = 0;
    A->a(2, 5) = 0;
    A->a(2, 6) = 0;
    A->a(2, 7) = 0;
    A->a(2, 8) = 0;
    A->a(2, 9) = 0;
    A->a(2, 10) = 0;
    A->a(2, 11) = 0;
    A->a(2, 12) = 1;
    A->a(2, 13) = 1;
    A->a(2, 14) = 1;
    A->a(2, 15) = 1;

    B->a(2, 0) = c;

    A->a(3, 0) = 1;
    A->a(3, 1) = 1;
    A->a(3, 2) = 1;
    A->a(3, 3) = 1;
    A->a(3, 4) = 1;
    A->a(3, 5) = 1;
    A->a(3, 6) = 1;
    A->a(3, 7) = 1;
    A->a(3, 8) = 1;
    A->a(3, 9) = 1;
    A->a(3, 10) = 1;
    A->a(3, 11) = 1;
    A->a(3, 12) = 1;
    A->a(3, 13) = 1;
    A->a(3, 14) = 1;
    A->a(3, 15) = 1;

    B->a(3, 0) = d;

    A->a(4, 0) = 0;
    A->a(4, 1) = 0;
    A->a(4, 2) = 0;
    A->a(4, 3) = 0;
    A->a(4, 4) = 0;
    A->a(4, 5) = 0;
    A->a(4, 6) = 0;
    A->a(4, 7) = 0;
    A->a(4, 8) = 0;
    A->a(4, 9) = 0;
    A->a(4, 10) = 0;
    A->a(4, 11) = 1;
    A->a(4, 12) = 0;
    A->a(4, 13) = 0;
    A->a(4, 14) = 0;
    A->a(4, 15) = 0;

    B->a(4, 0) = derAX;

    A->a(5, 0) = 0;
    A->a(5, 1) = 0;
    A->a(5, 2) = 0;
    A->a(5, 3) = 3;
    A->a(5, 4) = 0;
    A->a(5, 5) = 0;
    A->a(5, 6) = 0;
    A->a(5, 7) = 2;
    A->a(5, 8) = 0;
    A->a(5, 9) = 0;
    A->a(5, 10) = 0;
    A->a(5, 11) = 1;
    A->a(5, 12) = 0;
    A->a(5, 13) = 0;
    A->a(5, 14) = 0;
    A->a(5, 15) = 0;

    B->a(5, 0) = derBX;

    A->a(6, 0) = 0;
    A->a(6, 1) = 0;
    A->a(6, 2) = 0;
    A->a(6, 3) = 0;
    A->a(6, 4) = 0;
    A->a(6, 5) = 0;
    A->a(6, 6) = 0;
    A->a(6, 7) = 0;
    A->a(6, 8) = 1;
    A->a(6, 9) = 1;
    A->a(6, 10) = 1;
    A->a(6, 11) = 1;
    A->a(6, 12) = 0;
    A->a(6, 13) = 0;
    A->a(6, 14) = 0;
    A->a(6, 15) = 0;

    B->a(6, 0) = derCX;

    A->a(7, 0) = 3;
    A->a(7, 1) = 3;
    A->a(7, 2) = 3;
    A->a(7, 3) = 3;
    A->a(7, 4) = 2;
    A->a(7, 5) = 2;
    A->a(7, 6) = 2;
    A->a(7, 7) = 2;
    A->a(7, 8) = 1;
    A->a(7, 9) = 1;
    A->a(7, 10) = 1;
    A->a(7, 11) = 1;
    A->a(7, 12) = 0;
    A->a(7, 13) = 0;
    A->a(7, 14) = 0;
    A->a(7, 15) = 0;

    B->a(7, 0) = derDX;

    A->a(8, 0) = 0;
    A->a(8, 1) = 0;
    A->a(8, 2) = 0;
    A->a(8, 3) = 0;
    A->a(8, 4) = 0;
    A->a(8, 5) = 0;
    A->a(8, 6) = 0;
    A->a(8, 7) = 0;
    A->a(8, 8) = 0;
    A->a(8, 9) = 0;
    A->a(8, 10) = 0;
    A->a(8, 11) = 0;
    A->a(8, 12) = 0;
    A->a(8, 13) = 0;
    A->a(8, 14) = 1;
    A->a(8, 15) = 0;

    B->a(8, 0) = derAY;

    A->a(9, 0) = 0;
    A->a(9, 1) = 0;
    A->a(9, 2) = 1;
    A->a(9, 3) = 0;
    A->a(9, 4) = 0;
    A->a(9, 5) = 0;
    A->a(9, 6) = 1;
    A->a(9, 7) = 0;
    A->a(9, 8) = 0;
    A->a(9, 9) = 0;
    A->a(9, 10) = 1;
    A->a(9, 11) = 0;
    A->a(9, 12) = 0;
    A->a(9, 13) = 0;
    A->a(9, 14) = 1;
    A->a(9, 15) = 0;

    B->a(9, 0) = derBY;

    A->a(10, 0) = 0;
    A->a(10, 1) = 0;
    A->a(10, 2) = 0;
    A->a(10, 3) = 0;
    A->a(10, 4) = 0;
    A->a(10, 5) = 0;
    A->a(10, 6) = 0;
    A->a(10, 7) = 0;
    A->a(10, 8) = 0;
    A->a(10, 9) = 0;
    A->a(10, 10) = 0;
    A->a(10, 11) = 0;
    A->a(10, 12) = 3;
    A->a(10, 13) = 2;
    A->a(10, 14) = 1;
    A->a(10, 15) = 0;

    B->a(10, 0) = derCY;

    A->a(11, 0) = 3;
    A->a(11, 1) = 2;
    A->a(11, 2) = 1;
    A->a(11, 3) = 0;
    A->a(11, 4) = 3;
    A->a(11, 5) = 2;
    A->a(11, 6) = 1;
    A->a(11, 7) = 0;
    A->a(11, 8) = 3;
    A->a(11, 9) = 2;
    A->a(11, 10) = 1;
    A->a(11, 11) = 0;
    A->a(11, 12) = 3;
    A->a(11, 13) = 2;
    A->a(11, 14) = 1;
    A->a(11, 15) = 0;

    B->a(11, 0) = derDY;

    A->a(12, 0) = 0;
    A->a(12, 1) = 0;
    A->a(12, 2) = 0;
    A->a(12, 3) = 0;
    A->a(12, 4) = 0;
    A->a(12, 5) = 0;
    A->a(12, 6) = 0;
    A->a(12, 7) = 0;
    A->a(12, 8) = 0;
    A->a(12, 9) = 0;
    A->a(12, 10) = 1;
    A->a(12, 11) = 0;
    A->a(12, 12) = 0;
    A->a(12, 13) = 0;
    A->a(12, 14) = 0;
    A->a(12, 15) = 0;

    B->a(12, 0) = 0;

    A->a(13, 0) = 0;
    A->a(13, 1) = 0;
    A->a(13, 2) = 3;
    A->a(13, 3) = 0;
    A->a(13, 4) = 0;
    A->a(13, 5) = 0;
    A->a(13, 6) = 2;
    A->a(13, 7) = 0;
    A->a(13, 8) = 0;
    A->a(13, 9) = 0;
    A->a(13, 10) = 1;
    A->a(13, 11) = 0;
    A->a(13, 12) = 0;
    A->a(13, 13) = 0;
    A->a(13, 14) = 0;
    A->a(13, 15) = 0;

    B->a(13, 0) = 0;

    A->a(14, 0) = 0;
    A->a(14, 1) = 0;
    A->a(14, 2) = 0;
    A->a(14, 3) = 0;
    A->a(14, 4) = 0;
    A->a(14, 5) = 0;
    A->a(14, 6) = 0;
    A->a(14, 7) = 0;
    A->a(14, 8) = 3;
    A->a(14, 9) = 2;
    A->a(14, 10) = 1;
    A->a(14, 11) = 0;
    A->a(14, 12) = 0;
    A->a(14, 13) = 0;
    A->a(14, 14) = 0;
    A->a(14, 15) = 0;

    B->a(14, 0) = 0;

    A->a(15, 0) = 9;
    A->a(15, 1) = 6;
    A->a(15, 2) = 3;
    A->a(15, 3) = 0;
    A->a(15, 4) = 6;
    A->a(15, 5) = 4;
    A->a(15, 6) = 2;
    A->a(15, 7) = 0;
    A->a(15, 8) = 3;
    A->a(15, 9) = 2;
    A->a(15, 10) = 1;
    A->a(15, 11) = 0;
    A->a(15, 12) = 0;
    A->a(15, 13) = 0;
    A->a(15, 14) = 0;
    A->a(15, 15) = 0;

    B->a(15, 0) = 0;

    Matrix * C = new Matrix(A);
    Matrix * D = new Matrix(B);

    if (!Matrix::matrixSolveGaussian(A, B)) {
        CORE_ASSERT_FAIL("matrixSolveGaussian return false");
    }

    Matrix * F = C->mul(*B);
    for (int i = 0; i < 16; i ++)
    {
        if (F->a(i, 0) != D->a(i, 0)) {
            cout << "mistake at " << i << F->a(i, 0) - D->a(i, 0) << endl;
            CORE_ASSERT_FAIL("matrixSolveGaussian return bad solution");
        }
        cout << B->a(i, 0) << " ";
    }

    delete_safe(A);
    delete_safe(B);
    delete_safe(C);
    delete_safe(D);
    delete_safe(F);
}

TEST(GaussianSolution, CorrectSolutionForPolynomInterpolation)
{
    int degree = 2;
    Matrix * A = new Matrix(pow(degree + 1, 2), pow(degree + 1, 2));
    Matrix * B = new Matrix(pow(degree + 1, 2), 1);

    for (int i = -1; i < degree; i++) {
        for (int j = -1; j < degree; j++)
        {
            for (int k = 0; k <= degree; k++)
            {
                for (int g = 0; g <= degree; g++) {
                    A->a((degree + 1) * (i + 1) + j + 1, k * (degree + 1) + g) = pow(i, k) * pow(j, g);
                }
            }
        }
    }
    for (int i = 0; i < pow(degree + 1, 2); i ++) {
        for (int j = 0; j < pow(degree + 1, 2); j ++) {
            cout << A->a(i, j) << " ";
        }
        cout << endl;
    }
    for (int i = 0; i < pow(degree + 1, 2); i ++) {
        B->a(i, 0) = 0;
    }

    B->a(8, 0) = 1;

    Matrix * C = new Matrix(A);
    Matrix * D = new Matrix(B);

    if (!Matrix::matrixSolveGaussian(A, B)) {
        CORE_ASSERT_FAIL("matrixSolveGaussian return false");
    }

    Matrix * F = C->mul(*B);
    for (int i = 0; i < pow(degree + 1, 2); i ++)
    {
        if (fabs(F->a(i, 0) - D->a(i, 0)) > pow(10, -15)) {
            cout << "mistake at " << i << " " << F->a(i, 0) - D->a(i, 0) << endl;
            CORE_ASSERT_FAIL("matrixSolveGaussian return bad solution");
        }
        cout << B->a(i, 0) << endl << " ";
    }

    delete_safe(D);
    delete_safe(C);
    delete A;
    delete B;
    delete F;
}
