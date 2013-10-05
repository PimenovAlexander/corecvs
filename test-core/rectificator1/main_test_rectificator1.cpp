/**
 * \file main_test_rectify.cpp
 * \brief Add Comment Here
 *
 * \date Jul 29, 2010
 * \author alexander
 *
 * \ingroup autotest
 */

#include <stdio.h>
#include <vector>
#include <limits>

#include "global.h"

#include "matrix.h"
#include "matrix33.h"
#include "matrix44.h"
#include "vector3d.h"
#include "correspondanceList.h"
#include "bmpLoader.h"
#include "essentialMatrix.h"
#include "cameraParameters.h"
#include "mathUtils.h"
#include "simulation/flowSimuator.h"
#include "ransacEstimator.h"
#include "stereoAligner.h"

using namespace std;
using namespace corecvs;


#define GRID_STEP (10)

void testAligner (void)
{
    Vector3dd z1;

    Matrix33 F1;
    Matrix33 L,R;


    z1  = Vector3dd (-0.698165, 0.715936, 0 );
    F1 = Matrix33(
            -1.09302e-34, -4.79845e-19, 6.98724e-17,
             5.58512e-19, 3.42453e-20, -0.0110368,
             -7.88134e-17, 0.0110368, 3.33067e-15
             );

    StereoAligner::getAlignmentTransformation(F1, &L, &R, z1);

    Vector3dd z2;
    Matrix33 F2;

    F2 = Matrix33( -4.68373e-06, 5.35286e-05, -0.0168343,
     -5.35711e-05, -4.77577e-06, 0.00601651,
     0.0181705, -0.0037651, -0.307778);
    z2  = Vector3dd (1 , 0 , 0 );
    StereoAligner::getAlignmentTransformation(F2, &L, &R, z2);

}

void testEpipoles ( void )
{
    EssentialMatrix F = Matrix33( -4.68373e-06, 5.35286e-05, -0.0168343,
       -5.35711e-05, -4.77577e-06, 0.00601651,
       0.0181705, -0.0037651, -0.307778);

    Vector3dd e1(1.0,1.0,1.0);
    Vector3dd e2(1.0,1.0,1.0);

    F.nullspaces(&e1, &e2);

    Vector3dd leftResult(0.0);
    Vector3dd rightResult(0.0);

#ifdef TRACE
    e1.println();
    e2.println();
#endif

#ifdef ASSERTS
    Vector3dd left = e1 * F;
    Vector3dd right = F * e2;
    ASSERT_TRUE(left.notTooFar(leftResult, 1e-10), "Left Epipole Error");
    ASSERT_TRUE(right.notTooFar(rightResult, 1e-10), "Right Epipole Error");
#endif
}


void __testUnDistortion ( void )
{

    Vector3dd affineProjective(0.0,0.0,1.0);
    double distortion = StereoAligner::getDistortion(affineProjective, Vector2dd(100.0,100.0));

    printf("%lf\n", distortion);

    Vector3dd distortive(1.0,0.0,1.0);
    distortion = StereoAligner::getDistortion(distortive, Vector2dd(100.0,100.0));
    printf("%lf\n", distortion);


    /* Here goes the deep test*/
    EssentialMatrix F = Matrix33( -4.68373e-06, 5.35286e-05, -0.0168343,
           -5.35711e-05, -4.77577e-06, 0.00601651,
           0.0181705, -0.0037651, -0.307778);

  /*  F = Matrix33(
             -1.09302e-34, -4.79845e-19, 6.98724e-17,
              5.58512e-19, 3.42453e-20, -0.0110368,
              -7.88134e-17, 0.0110368, 3.33067e-15
              );*/

    Vector3dd E1;
    Vector3dd E2;
    F.nullspaces(&E2, &E1);


    double dist[1000][2];
    double min = numeric_limits<double>::max();
    double max = numeric_limits<double>::min();
    double minsum = numeric_limits<double>::max();
    double minalpha  = 0;

    for (unsigned i = 0; i < 1000; i++)
    {
        double alpha = i / 1000.0 * M_PI * 2.0;
        Vector3dd z = Vector3dd(cos(alpha), sin(alpha), 0.0);

        Vector3dd w1 = z ^ E1;
        Vector3dd w2 = F * z;

        w1 = w1.normalizeProjective();
        w2 = w2.normalizeProjective();

        dist[i][0] = StereoAligner::getDistortion(w1, Vector2dd(100.0,100.0));
        dist[i][1] = StereoAligner::getDistortion(w2, Vector2dd(100.0,100.0));

        if (min > dist[i][0])
            min = dist[i][0];
        if (min > dist[i][1])
            min = dist[i][1];
        if (max < dist[i][0])
            max = dist[i][0];
        if (max < dist[i][1])
            max = dist[i][1];

        if (dist[i][1] + dist[i][0] <  minsum)
        {
            minsum = dist[i][1] + dist[i][0];
            minalpha = alpha;
        }
    }

    RGB24Buffer *graph = new RGB24Buffer(500,1000);

    max = 10000;
    for (unsigned i = 0; i < 1000; i++)
    {
        int y;
        y = (dist[i][0] - min) / (max - min) * 500;
        if (y >= 0 && y < 500)
            graph->element(500 - y - 1,i) = RGBColor(0xFF00FF);

        y = (dist[i][1] - min) / (max - min) * 500;
        if (y >= 0 && y < 500)
            graph->element(500 - y - 1,i) = RGBColor(0xFFFFFF);
    }

    BMPLoader loader;
    loader.save("graph.bmp", graph);
    printf ("Minimum distortions are with alpha %lf", minalpha);
    delete graph;
}


void testRectificatorCubeEssential (void)
{
    vector<Vector3dd> *pointsIn3d = NULL;
    vector<Vector2dd> imagesL;
    vector<Vector2dd> imagesR;

    CorrespondanceList points;


    // A cube of 1.5 m side at the distance of 2 meters
    Matrix44 modelViewMatrix = Matrix44::Shift(0.0, 0.0, 2000.0) * Matrix44::Scale(1500);
    pointsIn3d = FlowSimuator::unitCube(3);

    for (unsigned i = 0; i < pointsIn3d->size(); i++)
        pointsIn3d->at(i) = modelViewMatrix * pointsIn3d->at(i);


    /**
     *   Camera positions and orientations
     *
     *    |..........|
     *        60mm
     *
     **/
    Vector3dd  leftCameraPosition = Vector3dd( 0.0, 0.0,  0.0);
    Vector3dd rightCameraPosition = Vector3dd(60.0, 5.0, -3.0);

    Matrix44  leftCameraRotation = Matrix44(1.0);
    Matrix44 rightCameraRotation = Matrix44(Matrix33::RotationZ(0.01), Vector3dd(0.0));

    Matrix44  leftCameraMatrix =  leftCameraRotation * Matrix44::Shift( leftCameraPosition);
    Matrix44 rightCameraMatrix = rightCameraRotation * Matrix44::Shift(rightCameraPosition);

    /**
     *
     *  Camera intrinsics
     **/

    Vector2dd resolution(400.0, 400.0);
    CameraIntrinsics  leftCameraIntrinsics(resolution, resolution  / 2.0, 360.0, 1.0);
    CameraIntrinsics rightCameraIntrinsics(resolution, resolution  / 2.0, 360.0, 1.0);


    Matrix44 LK = leftCameraIntrinsics.getKMatrix();
    Matrix44 RK = rightCameraIntrinsics.getKMatrix();
    Matrix44 LKI = leftCameraIntrinsics.getInvKMatrix();
    Matrix44 RKI = rightCameraIntrinsics.getInvKMatrix();


    Matrix44 leftCamera  = LK *  leftCameraMatrix.inverted();
    Matrix44 rightCamera = RK * rightCameraMatrix.inverted();

    cout << "Left Matrix:\n"  << leftCamera  << "\n";
    cout << "Right Matrix:\n" << rightCamera << "\n";

    Vector3dd in;
    in = Vector3dd(0,0,2);
    cout << "Before:\n" << in << "\n";
    cout << "After: \n" << rightCamera * in << "\n";


    for (unsigned i = 0; i < pointsIn3d-> size(); i++)
    {
        Correspondance corr;
        Vector3dd left3D  =  leftCamera * pointsIn3d->at(i);
        Vector3dd right3D = rightCamera * pointsIn3d->at(i);

        Vector2dd imageL = (Vector2dd)  left3D;
        Vector2dd imageR = (Vector2dd) right3D;
/*
        if (imageL.x() != (double)fround(imageL.x()))
            printf("Adacto %lf %lf\n", imageL.x(), (double)fround(imageL.x()));
        if (imageL.y() != (double)fround(imageL.y()))
            printf("Adacto %lf %lf\n", imageL.y(), (double)fround(imageL.y()));
        if (imageR.x() != (double)fround(imageR.x()))
            printf("Adacto %lf %lf\n", imageR.x(), (double)fround(imageR.x()));
        if (imageR.y() != (double)fround(imageR.y()))
            printf("Adacto %lf %lf\n", imageR.y(), (double)fround(imageR.y()));
*/

        imageL.x() = (double)fround(imageL.x());
        imageL.y() = (double)fround(imageL.y());
        imageR.x() = (double)fround(imageR.x());
        imageR.y() = (double)fround(imageR.y());


        imagesL.push_back(imageL);
        imagesR.push_back(imageR);

        corr.start = (Vector2dd)(LKI * Vector3dd(imageL, 1.0));
        corr.end   = (Vector2dd)(RKI * Vector3dd(imageR, 1.0));

        corr.flags = 0;
        corr.value = 1;
        points.push_back(corr);
    }

    /* dump data for gnuplot */
    FILE *gnuplot = fopen("points.dat", "wt");

    fprintf(gnuplot , "# Xleft  Yleft  Xright  Yright  X  Y Z\n" );


    for (unsigned i = 0; i < points.size(); i++)
    {
        //Correspondance *corr = &points[i];
        fprintf(gnuplot , "%2.7lf %2.7lf %2.7lf %2.7lf  %2.7lf %2.7lf %2.7lf\n",
                imagesL[i].x(),
                imagesL[i].y(),
                imagesR[i].x(),
                imagesR[i].y(),
                pointsIn3d->at(i).x(),
                pointsIn3d->at(i).y(),
                pointsIn3d->at(i).z()
                );
    }

    fprintf(gnuplot , "0 0 0 0 0 0 0\n");

    fclose(gnuplot);

    RansacEstimator ransacEstimator(14, 4, 1.00000);
    EssentialMatrix E = EssentialMatrix(ransacEstimator.getEssentialRansac1(&points));

    //EssentialMatrix E = EssentialMatrix(Matrix33::CrossProductLeft(Vector3dd(1.0,0.0,0.0)));


    Matrix33  R[4];
    Vector3dd t[4];

    cout << "=============================================================================" << endl;
    cout << "E" << endl;
    cout << E << endl;

    E.decompose(R, t);
    int selected = 0;
    for (selected = 0; selected < 4; selected++)
    {
        cout << "Selection: " << selected << endl;
        cout << "Rotation d=" << R[selected].det() << endl;
        cout << R[selected] << endl;
        cout << "Translation " << endl;
        cout << t[selected] << endl;
    }


    for (selected = 0; selected < 4; selected++)
    {

        Matrix33  sR = R[selected];
        Vector3dd st = t[selected];
        st *= (rightCameraPosition - leftCameraPosition).l2Metric() / st.l2Metric();

        /* reconstructing the real depth*/
        cout << "=============================================================================" << endl;
        cout << "Rotation1 d=" << sR.det() << endl;
        cout << sR << endl;
        cout << "Translation " << endl;
        cout << st << endl;

        cout << "Points " << endl;

        /* dump data for gnuplot */
        char name[100];
        snprintf2buf(name, "result%d.dat", selected);

        FILE *gnuplot = fopen(name, "wt");

        for (unsigned i = 0; i < points.size(); i++)
        {

            Correspondance *corr = &points[i];
            Vector3dd left  = Vector3dd(corr->start.x(), corr->start.y(), 1.0);
            Vector3dd right = Vector3dd(corr->  end.x(), corr->  end.y(), 1.0);

            Vector3dd n1 = left;
            Vector3dd n2 = sR * right;
            Matrix33 m = Matrix33::FromColumns(n1, n2, n1 ^ n2);

            Vector3dd dist = m.inv() * st;
            Vector3dd position = left * dist.x();
            double percent = (dist.x() - pointsIn3d->at(i).z()) / (pointsIn3d->at(i).z()) * 100.0;
            cout << dist << " " << position  << " <=> " << pointsIn3d->at(i) << " err=" << percent << "%" << endl;

            fprintf(gnuplot, "%lf %lf %lf %lf %lf %lf\n",
                    position.x(),
                    position.y(),
                    position.z(),
                    pointsIn3d->at(i).x(),
                    pointsIn3d->at(i).y(),
                    pointsIn3d->at(i).z()
            );
        }

        fclose(gnuplot);

    }
    delete pointsIn3d;

}



void testRectificatorCube (void)
{

    cout << "=============================Testing old style rectificator=================================" << endl;

    CorrespondanceList points;
    vector<Vector3dd> *pointsIn3d = FlowSimuator::unitCube(GRID_STEP);


    Matrix44 leftCamera  = Matrix44::ProjectParallelToZ() * Matrix44(Matrix33(1.0), Vector3dd(0,0,4));
    Matrix44 rightCamera = Matrix44::ProjectParallelToZ() * Matrix44(Matrix33(1.0), Vector3dd(0.1,0,4));

    cout << "Left Matrix:\n" << leftCamera << "\n";
    cout << "Right Matrix:\n" << rightCamera << "\n";

    Vector3dd in;
    in = Vector3dd(0,0,2);
    cout << "Before:\n" << in << "\n";
    cout << "After: \n" << rightCamera * in << "\n";


    for (unsigned i = 0; i < pointsIn3d->size(); i++)
    {
        Correspondance corr;
        Vector3dd left3D  =  leftCamera * pointsIn3d->at(i);
        Vector3dd right3D = rightCamera * pointsIn3d->at(i);

        corr.start = Vector2dd( left3D.x(),  left3D.y());
        corr.end   = Vector2dd(right3D.x(), right3D.y());

        corr.flags = 0;
        corr.value = 1;
        points.push_back(corr);
    }

    srand(1);
    RansacEstimator ransacEstimator(14, 4, 0.000001);


/*    Matrix33 leftNormalize;
    Matrix33 rightNormalize;*/
    Matrix33 F;
    //double scale;

    /*points.getNoramlisingTransform(leftNormalize, rightNormalize, &scale);
    CorrespondanceList *normalisedList = new CorrespondanceList(points);
    normalisedList->transform(leftNormalize, rightNormalize);

    Matrix33 leftNormalize1;
    Matrix33 rightNormalize1;
    normalisedList->getNoramlisingTransform(leftNormalize1, rightNormalize1, &scale);*/

    //F = rectifier.getEssentialRansac(&points);
    //printf("Matrix of the normalized data\n");
    //F.print();

    // Get the fundamental matrix for the real point list
    //F = leftNormalize.transposed() * F * rightNormalize;
    //delete normalisedList;
    F = ransacEstimator.getFundamentalRansac1(&points);

    printf("Guessed fundamental matrix:\n");
    F.print();
    printf("\n");

    Vector3dd E1;
    Vector3dd E2;
    EssentialMatrix(F).nullspaces(&E1, &E2);
    printf("Guessed Epipoles:\n");
    E1.print();
    printf("\n\n");
    E2.print();
    printf("\n\n");


    Matrix33 leftTranstorm;
    Matrix33 rightTranstorm;

    StereoAligner::getAlignmentTransformation(
        F,
        &leftTranstorm,
        &rightTranstorm, Vector3dd(1.0,0,0));

    printf("Guessed left transform:\n");
    leftTranstorm.print();
    printf("\n");
    printf("Guessed right transform:\n");
    rightTranstorm.print();
    printf("\n");

    /* Some comparison */

    Matrix33 Ix = Matrix33::CrossProductLeft(Vector3dd(1.0,0.0,0.0));

    Matrix33 Fprim = leftTranstorm.transposed() * Ix * rightTranstorm;

    printf("Recollect\n");
    Fprim.print();
    printf("\n");
    printf("Old Matrix was\n");
    F.print();
    printf("\n");
    ASSERT_TRUE(Fprim.notTooFar(F, 1e-5), "Matrix reconstruction failed");

    delete pointsIn3d;

}

int main (int /*argC*/, char ** /*argV*/)
{
    testRectificatorCubeEssential();
//    testRectificatorCube ();
    return 0;
    __testUnDistortion();
    testEpipoles();
    testAligner();
    cout << "PASSED" << endl;
    return 0;
}
