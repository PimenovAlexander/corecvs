/**
 * \file main_test_rectify.cpp
 * \brief Add Comment Here
 *
 * \date Jul 29, 2010
 * \author alexander
 */

#include <vector>
#include <algorithm>

#include "global.h"

#include "mathUtils.h"
#include "correspondanceList.h"
#include "matrix.h"
#include "essentialMatrix.h"
#include "ransacEstimator.h"

using namespace std;
using namespace corecvs;


const unsigned GRID_STEP = 10;
const unsigned GRID_SIZE = GRID_STEP * GRID_STEP;


void printMatrixInfo (const Matrix33 &matrix)
{
    printf("Guessed fundamental matrix:\n");
    matrix.print();
    printf("\n");

    Vector3dd E1;
    Vector3dd E2;
    EssentialMatrix(matrix).nullspaces(&E1, &E2);
    printf("Guessed Epipoles:\n");
    E1.print();
    printf("\n\n");
    E2.print();
    printf("\n\n");


    Matrix33  X = matrix;
    Vector3dd W;
    Matrix33  V;
    Matrix::svd(&X, &W, &V);
    printf("Fundamental Singular values:\n");
    W.println();



/*    Matrix33 leftTranstorm;
    Matrix33 rightTranstorm;

    rectifier.getRectificationTransformationSimple1(
        matrix,
        &leftTranstorm,
        &rightTranstorm, Vector3dd(1.0,0,0));

    printf("Guessed left transform:\n");
    leftTranstorm.print();
    printf("\n");
    printf("Guessed right transform:\n");
    rightTranstorm.print();
    printf("\n");*/
}

void testFundamentalEstimator ( void )
{
    cout << "=================Fundamental Estimator test===============" << endl;

    CorrespondanceList points;
    for (unsigned i = 0; i < GRID_STEP; i++)
    {
        for (unsigned j = 0; j < GRID_STEP; j++)
        {

            Correspondance corr;
            double step = 1.0 / (GRID_STEP - 1);
            double x = (step * j) - 0.5;
            double y = (step * i) - 0.5;
            double alpha = 0.05;

            Matrix33 rotation = Matrix33::RotationZ(alpha);

            /*TODO: Shift only causes the memory error */
            corr.start = Vector2dd(x, y);
            corr.end = rotation * corr.start + Vector2dd (+ 0.01, - 0.1);

            corr.flags = 0;
            corr.value = 1;

            points.push_back(corr);
        }
    }

    RansacEstimator ransacEstimator(14, 1, 0.000001);
    srand(1);
    Matrix33 F = ransacEstimator.getFundamentalRansac1(&points);
    srand(1);
    Matrix33 F1 = ransacEstimator.getFundamentalRansac1(&points);
    srand(1);
    Matrix33 E = ransacEstimator.getEssentialRansac1(&points);
    srand(1);
    Matrix33 E1 = ransacEstimator.getEssentialRansac1(&points);

    printMatrixInfo(F);
    printf("====================================\n");
    printMatrixInfo(F1);
    printf("==========Essential ================\n");
    printMatrixInfo(E);
    printf("====================================\n");
    printMatrixInfo(E1);

}

void testSVDDesc ( void )
{
    Matrix33  R = Matrix33::RotationZ(M_PI/6);
    Vector3dd t = Vector3dd(2.0, 1.0, 0.0);
    Matrix33  T = Matrix33::CrossProductLeft(t);
    Matrix33  E = R * T;

    Matrix33  U = E;
    Vector3dd S;
    Matrix33  V;
    Matrix::svdDesc(&U, &S, &V);

    cout << E;
    cout << (U * Matrix33::Scale3(S) * V.transposed());

}

void testComposeDecompose ( void )
{
    Matrix33  R = Matrix33::RotationZ(M_PI/6);
    Vector3dd t = Vector3dd(2.0, 1.0, 0.0);
    //Matrix33  T = Matrix33::CrossProductLeft(t);
    EssentialMatrix E;
    E = E.compose(R,t);

    Matrix33 rot[4];
    Vector3dd trans[4];
    E.decompose(rot,trans);

    for(int i = 0; i < 4; i++ )
    {
        cout << "rot: " << endl;
        cout << rot[i] << endl;
        cout << "trans: " << endl;
        cout << trans[i] << endl;
    }
    cout << "R = " << endl;
    cout << R << endl;
    cout << "t = " << endl;
    cout << t << endl;


}



int main (int /*argC*/, char ** /*argV*/)
{
    //vector<int> a;
    //vector<int>::iterator b = find (a.begin(), a.end(), 1);
    //printf("%s\n", b == a.end() ? "true" : "flase");

    //testSVDDesc();
    //testFundamentalEstimator ();
    testComposeDecompose();


}

