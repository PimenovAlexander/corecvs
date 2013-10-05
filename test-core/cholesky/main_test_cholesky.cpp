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
#include "global.h"
#include "matrix.h"
#include "cholesky.h"
#include "diagonalMatrix.h"


using namespace std;
using namespace corecvs;

void testCholesky(void)
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
}

void testCholesky1(void)
{
    DiagonalMatrix *D;
    UpperUnitaryMatrix *U1;
    Matrix *A = new Matrix(4,4,1.0);
    Cholesky::udutDecompose(A, &U1, &D);

    delete U1;
    delete D;
    delete A;

}

int main (int /*argC*/, char ** /*argV*/)
{
    //testCholesky();
    testCholesky1();
        cout << "PASSED" << endl;
        return 0;
}
