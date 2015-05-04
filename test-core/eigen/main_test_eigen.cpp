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
#include "global.h"
#include "matrix.h"

using namespace std;
using namespace corecvs;

void testJacobi( void )
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

int main (int /*argC*/, char **/*argV*/)
{
    testJacobi();
    cout << "PASSED" << endl;
    return 0;
}
