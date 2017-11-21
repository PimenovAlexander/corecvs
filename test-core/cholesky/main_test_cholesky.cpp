/**
 * \file main_test_cholesky.cpp
 * \brief This is the main file for the test cholesky 
 *
 * \date Jan 10, 2011
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"
#include "core/math/matrix/matrix.h"
#include "core/kalman/cholesky.h"
#include "core/math/matrix/diagonalMatrix.h"

using namespace corecvs;

TEST(CholeskyTest, testCholesky)
{
    Matrix A = Matrix(3,3,false);
    A.fillWithArgs(
             2.0,-1.0, 1.0,
            -1.0, 2.0,-1.0,
             1.0,-1.0, 2.0 );

/*    A.fillWithArgs(
      2.0, 0.0, 0.0,
      0.0, 2.0, 0.0,
      0.0, 0.0, 2.0 );*/


    cout << "Input Matrix:" << endl << A << endl;

    Matrix *U;
    DiagonalMatrix *D;
    Cholesky::udutDecompose(&A, &U, &D);
    cout << *U;

    cout << endl << "[";
    for (int i = 0; i < D->size(); i++)
    {
        cout << D->at(i) << " ";
    }
    cout << "]" << endl;

    UpperUnitaryMatrix *U1;
    DiagonalMatrix *D1;
    Cholesky::udutDecompose(&A,&U1,&D1);

    cout << *U1;

    cout << endl << "[";
    for (int i = 0; i < D->size(); i++)
    {
        cout << D->at(i) << " ";
    }
    cout << "]" << endl;

    delete U;
    delete U1;
    delete D;
    delete D1;

    {
        Matrix *U2;
        Cholesky::uutDecompose(&A,&U2);
        Matrix B = (*U2) * U2->t();
        ASSERT_TRUE( A.notTooFar(&B, 1e-7, true));
        delete_safe(U2);
    }

}

TEST(CholeskyTest, testCholesky1)
{
    DiagonalMatrix *D;
    UpperUnitaryMatrix *U1;
    Matrix *A = new Matrix(4,4,1.0);
    Cholesky::udutDecompose(A, &U1, &D);

    delete U1;
    delete D;
    delete A;

}

