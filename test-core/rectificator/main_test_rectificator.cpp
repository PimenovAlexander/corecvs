/**
 * \file main_test_rectify.cpp
 * \brief Add Comment Here
 *
 * \date Jul 29, 2010
 * \author alexander
 */

#include <random>
#include <vector>
#include <algorithm>
#include <fstream>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/math/mathUtils.h"
#include "core/rectification/correspondenceList.h"
#include "core/math/matrix/matrix.h"
#include "core/rectification/essentialMatrix.h"
#include "core/rectification/ransacEstimator.h"
#include "core/rectification/essentialEstimator.h"
#include "core/cameracalibration/cameraModel.h"
#include "core/geometry/polygons.h"
#include "core/geometry/mesh3d.h"
#include "core/cameracalibration/calibrationDrawHelpers.h"

#include "core/rectification/iterativeEstimator.h"

using namespace corecvs;


const unsigned GRID_STEP = 10;
const unsigned GRID_SIZE = GRID_STEP * GRID_STEP;
const int DEFAULT_SEED = 777;
const int RNG_RETRIES = 16384;

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

TEST(Rectification, testEssentialDecomposition)
{
    CameraModel cam1(PinholeCameraIntrinsics(100.0, 100.0, 100.0, 100.0, 0.0, Vector2dd(800, 800), Vector2dd(800, 800)));
    CameraModel cam2 = cam1;
	cam1.extrinsics.position = corecvs::Vector3dd(10, 0, 0);
	EssentialDecomposition ed = cam1.essentialDecomposition(cam2);
    std::mt19937 rng(DEFAULT_SEED);
    std::uniform_real_distribution<double> runif(-1e3, 1e3);


    PinholeCameraIntrinsics *pinhole1 = cam1.getPinhole();
    PinholeCameraIntrinsics *pinhole2 = cam2.getPinhole();

    ASSERT_TRUE(pinhole1 != NULL && pinhole2 != NULL);

	for (int i = 0; i < RNG_RETRIES; ++i)
	{
        Vector3dd pt(runif(rng), runif(rng), +1000);
        Vector2dd ptl = cam1.project(pt);
        Vector2dd ptr = cam2.project(pt);
		double scaleL, scaleR, foo;
        Matrix33 K1 = pinhole1->getKMatrix33().inv();
        Matrix33 K2 = pinhole2->getKMatrix33().inv();

		ed.getScaler(K1 * ptl, K2 * ptr, scaleL, scaleR, foo);
		ASSERT_TRUE(scaleL > 0.0 && scaleR > 0.0);
	}
	for (int i = 0; i < RNG_RETRIES; ++i)
	{
        Vector3dd pt(runif(rng), runif(rng), -1000);
        Vector2dd ptl = cam1.project(pt);
        Vector2dd ptr = cam2.project(pt);
		double scaleL, scaleR, foo;
        Matrix33 K1 = pinhole1->getKMatrix33().inv();
        Matrix33 K2 = pinhole2->getKMatrix33().inv();

        ed.getScaler(K1 * ptl, K2 * ptr, scaleL, scaleR, foo);
		ASSERT_TRUE(scaleL < 0.0 && scaleR < 0.0);
	}
}

TEST(Rectification, testFundamentalEstimator)
{
    cout << "=================Fundamental Estimator test===============" << endl;

    CorrespondenceList points;
    for (unsigned i = 0; i < GRID_STEP; i++)
    {
        for (unsigned j = 0; j < GRID_STEP; j++)
        {

            Correspondence corr;
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


TEST(CorrespondenceList, fromFloat)
{
    FloatFlowBuffer buffer(4,4);
    for (int i = 0; i < buffer.h; i++)
        for (int j = 0; j < buffer.w; j++)
            buffer.element(i,j) = FloatFlow(1.1, 1.1);


    CorrespondenceList list(&buffer);

    for (size_t i = 0; i < list.size(); i++ )
        cout << i << " " << list[i] << endl;

    ASSERT_TRUE((list[0].end.notTooFar(Vector2dd(1.1, 1.1)), "First element fail"));
    ASSERT_TRUE((list[15].end.notTooFar(Vector2dd(4.1, 4.1)), "Last element fail"));

}


#define GRID_STEP1 4

TEST(Rectification, testIterativeEstimator)
{
    cout << "=================Iterative Estimator test===============" << endl;

    CorrespondenceList *points = new CorrespondenceList;
    for (unsigned i = 0; i < GRID_STEP1; i++)
    {
        for (unsigned j = 0; j < GRID_STEP1; j++)
        {

            Correspondence corr;
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

            points->push_back(corr);

        }
    }

    /* This can be created only when data is fully formed */
    vector<Correspondence *> pointsPtr;
    pointsPtr.reserve(points->size());
    for (size_t p = 0; p < points->size(); p++)
    {
        pointsPtr.push_back(&points->operator [](p));
    }

    IterativeEstimator iterativeEstimator;
    iterativeEstimator.params.setIterationsNumber(1);
    srand(1);
    Matrix33 F = iterativeEstimator.getEssential(pointsPtr);
    printMatrixInfo(F);
    printf("====================================\n");
    delete_safe(points);
}

TEST(Rectification, testSVDDesc)
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

TEST(Rectification, DISABLED_testComposeDecompose)
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

#ifdef WITH_BLAS
TEST(Rectification, test7point)
{
    corecvs::Matrix33 P1(4000.0, 0.0, 2000.0, 0.0, 4000.0, 2000.0, 0.0, 0.0, 1.0);
    corecvs::Matrix33 P2 = P1;
    corecvs::Matrix33 R2(cos(.5), sin(.5), 0, -sin(.5), cos(.5), 0.0, 0.0, 0.0, 1.0);
    corecvs::Vector3dd T(1.0, 2.0, 3.0);

    std::mt19937 rng(DEFAULT_SEED);
    std::uniform_real_distribution<double> runif(-1e3, 1e3);
    std::normal_distribution<double> rnorm(0.0, 0.5);

    for (int rngi = 0; rngi < RNG_RETRIES; ++rngi)
    {
        std::vector<Correspondence> cv;
        for (int i = 0; i < 7; ++i)
        {
            corecvs::Vector3dd p(runif(rng), runif(rng), runif(rng));
            auto ppl = P1 * p;
            auto ppr = P2 * (R2 * p + T);
            auto projL = corecvs::Vector2dd(ppl[0], ppl[1]) / ppl[2];
            auto projR = corecvs::Vector2dd(ppr[0], ppr[1]) / ppr[2];
            corecvs::Correspondence corr;
            corr.start = projL;
            corr.end   = projR;
            cv.push_back(corr);
        }
        std::vector<Correspondence*> cl;
        for (auto& cc: cv)
            cl.push_back(&cc);
        auto Fv = corecvs::EssentialEstimator().getEssential7point(cl);
        
        for (uint i = 0; i < cv.size(); ++i)
        {
            corecvs::Vector3dd L(cv[i].start[0], cv[i].start[1], 1.0);
            corecvs::Vector3dd R(cv[i].end[0], cv[i].end[1], 1.0);
            for (auto& F: Fv)
            {
                auto line = F * R;
                double diff = L & line;
                double lineNorm = std::sqrt(line[0] * line[0] + line[1] * line[1]);
                ASSERT_NEAR(diff / lineNorm, 0.0, 1);
            }
        }
    }
}

TEST(Rectification, test5point)
{
    corecvs::Matrix33 P1(4000.0,    0.0, 2000.0,
                            0.0, 4000.0, 2000.0,
                            0.0,    0.0,    1.0);
    corecvs::Matrix33 P2(3000.0,    0.0, 1500.0,
                            0.0, 3000.0, 1500.0,
                            0.0,    0.0,    1.0);
    corecvs::Matrix33  R(cos(.5), sin(.5), 0.0,
                        -sin(.5), cos(.5), 0.0,
                            0.0,     0.0,  1.0);
    corecvs::Vector3dd T(1.0, 2.0, 3.0);

    std::mt19937 rng(DEFAULT_SEED);
    std::uniform_real_distribution<double> runif(-1e3, 1e3);
    std::normal_distribution<double> rnorm(0.0, 0.5);

    for (int rngi = 0; rngi < RNG_RETRIES; ++rngi)
    {
        std::vector<Correspondence> cv;
        for (int i = 0; i < 5; ++i)
        {
            corecvs::Vector3dd p(runif(rng), runif(rng), runif(rng));
            auto ppl = P1 * p;
            auto ppr = P2 * (R * p + T);
            auto projL = corecvs::Vector2dd(ppl[0], ppl[1]) / ppl[2];
            auto projR = corecvs::Vector2dd(ppr[0], ppr[1]) / ppr[2];
            ppl = P1.inv() * corecvs::Vector3dd(projL[0], projL[1], 1.0);
            ppr = P2.inv() * corecvs::Vector3dd(projR[0], projR[1], 1.0);
            projL = corecvs::Vector2dd(ppl[0], ppl[1]) / ppl[2];
            projR = corecvs::Vector2dd(ppr[0], ppr[1]) / ppr[2];
            corecvs::Correspondence corr;
            
            corr.start = projL;
            corr.end   = projR;
            cv.push_back(corr);
        }
        std::vector<Correspondence*> cl;
        for (auto& cc: cv)
            cl.push_back(&cc);
        auto Fv = corecvs::EssentialEstimator().getEssential5point(cl);
        
        for (size_t i = 0; i < cv.size(); ++i)
        {
            corecvs::Vector3dd L(cv[i].start[0], cv[i].start[1], 1.0);
            corecvs::Vector3dd R(cv[i].end[0], cv[i].end[1], 1.0);
            for (auto& F: Fv)
            {
                auto line = F * R;
                double diff = L & line;
                double lineNorm = std::sqrt(line[0] * line[0] + line[1] * line[1]);
                ASSERT_NEAR(diff / lineNorm, 0.0, 1);
            }
        }
    }

}
#endif // WITH_BLAS

TEST(Rectification, testEssentialEstimator)
{
    using corecvs::Matrix33;
    using corecvs::Vector3dd;
    using corecvs::Vector2dd;
    using corecvs::EssentialEstimator;

    using std::vector;

    Matrix33 P1(
        4000.0,    0.0, 2000.0,
           0.0, 4000.0, 2000.0,
           0.0,    0.0,    1.0);

    Matrix33 P2 = P1;
    double angle = -0.5;
    Matrix33 R2 = Matrix33::RotationZ(angle);

    Vector3dd T(1.0, 2.0, 3.0);

    std::mt19937 rng(DEFAULT_SEED);
    std::uniform_real_distribution<double> runif(-1e3, 1e3);
    std::normal_distribution<double> rnorm(0.0, 0.5);

    vector<Correspondence> cv;
    for (int i = 0; i < 8; ++i)
    {
        Vector3dd p(runif(rng), runif(rng), runif(rng));
        Vector3dd ppl = P1 * p;
        Vector3dd ppr = P2 * (R2 * p + T);
        corecvs::Correspondence corr;
        corr.start = ppl.project();
        corr.end   = ppr.project();
        cv.push_back(corr);
    }
    std::vector<Correspondence*> cl;
    for (auto& cc: cv)
        cl.push_back(&cc);

    Matrix33 F = corecvs::EssentialEstimator().getEssentialLSE(cl);
    
    for (size_t i = 0; i < cv.size(); ++i)
    {
        Vector3dd L(cv[i].start, 1.0);
        Vector3dd R(cv[i].end  , 1.0);

        double diff = L & (F * R);
        ASSERT_NEAR(diff, 0.0, 1);
    }

    std::vector<double> rmses;
    for (int iii = 0; iii < RNG_RETRIES; ++iii)
    {
        int N = (rng() % 1024) + 8;
        vector<Correspondence> cv;
        for (int i = 0; i < N; ++i)
        {
            Vector2dd projL, projR;
            do
            {
                Vector3dd p(runif(rng), runif(rng), runif(rng));
                Vector3dd ppl = P1 * p;
                Vector3dd ppr = P2 * (R2 * p + T);
                projL = ppl.project();
                projR = ppr.project();
            } while(!projL.isInRect(Vector2dd(0.0, 0.0), Vector2dd(4000.0, 4000.0)) ||
                    !projR.isInRect(Vector2dd(0.0, 0.0), Vector2dd(4000.0, 4000.0)));

            for (int ii = 0; ii < 2; ++ii)
            {
                assert(projL[ii] >= 0.0 && projL[ii] <= 4000);
                assert(projR[ii] >= 0.0 && projR[ii] <= 4000);
            }

            corecvs::Correspondence corr;
            corr.start = projL;
            corr.end   = projR;
            cv.push_back(corr);
        }
        vector<Correspondence*> cl;
        for (auto& cc: cv)
            cl.push_back(&cc);
        EssentialMatrix F1 = EssentialEstimator().getEssentialLSE(cl);
        F1.assertRank2();
        for (size_t i = 0; i < cl.size(); ++i)
        {
            Vector2dd noiseHigh( 1.0,  1.0);
            Vector2dd noiseLow (-1.0, -1.0);
            cl[i]->start += Vector2dd(rnorm(rng), rnorm(rng)).mappedToRect(noiseLow, noiseHigh);
            cl[i]->end   += Vector2dd(rnorm(rng), rnorm(rng)).mappedToRect(noiseLow, noiseHigh);
        }
        EssentialMatrix F = EssentialEstimator().getEssentialLSE(cl);
        F.assertRank2();
        double rmse1 = 0.0;
        for (size_t i = 0; i < cv.size(); ++i)
        {
            double diff = F1.epipolarDistanceFirst(cv[i].start, cv[i].end);
            rmse1 += diff * diff;
        }
        rmse1 = std::sqrt(rmse1 / cv.size());
        ASSERT_NEAR(rmse1, 0.0, 5);
       
        double rmse = 0.0;
        for (size_t i = 0; i < cv.size(); ++i)
        {
            double diff = F1.epipolarDistanceFirst(cv[i].start, cv[i].end);
            rmse += diff * diff;
        }
        rmse = std::sqrt(rmse / cv.size());
        //double rmseValid = rmse;
        ASSERT_NEAR(rmse, 0.0, (15.0));
//      std::cout << "ORIG:OK" << std::endl; 
        rmse = 0.0;
        for (size_t i = 0; i < cv.size(); ++i)
        {
            double diff = F.epipolarDistanceFirst(cv[i].start, cv[i].end);
            rmse += diff * diff;
        }
        rmse = std::sqrt(rmse / cv.size());
//      ASSERT_TRUE(rmseValid >= rmse);
        rmses.push_back(rmse);   
    }
    std::ofstream of;
    of.open("errors_fest_n.m", std::ios_base::app);
    of << "EN=[";
    for (auto&r : rmses)
        of << r << ",";
    of << "];";
}

#if 1

TEST(Rectification, testRectifiedModel)
{
    /* Trivial camera */
    CameraModel model;
    model.setLocation(Affine3DQ::Identity());
    model.intrinsics.reset(new PinholeCameraIntrinsics(Vector2dd(200,200), degToRad(60)));

    /* New Plane */
    PlaneFrame xy = PlaneFrame::PlaneXY();

    cout << "Original:" << xy << endl;    
    Affine3DQ move = Affine3DQ(
                Quaternion::RotationY(degToRad(10)) ^ Quaternion::RotationZ(degToRad(10)),
                Vector3dd(0.0, 0.0, 1.0)
                );
    cout << "Move:" << move << endl;

    xy.transform(move);
    cout << "Transformed:" << xy << endl;


    /* Carefully creating new camera and homography */
    Vector3dd norm = xy.getNormal();

    CameraModel model1;
    Affine3DQ location;
    location.shift = model.getAffine().shift;

    Vector3dd cameraX = xy.e1;
    Vector3dd cameraY = xy.e2;
    Vector3dd cameraAxis = norm;

    Matrix33 newCameraRotation = Matrix33::FromRows(cameraX, cameraY, cameraAxis);
    location.rotor = Quaternion::FromMatrix(newCameraRotation);


    model1.setLocation(location);


    /* Mesh3d */
    Mesh3D mesh;
    mesh.switchColor(true);
    CalibrationDrawHelpers helpers;
    mesh.setColor(RGBColor::White());
    helpers.drawCamera(mesh, model, 1.0);
    mesh.addPlaneFrame(xy, 3.0);

    mesh.setColor(RGBColor::Green());
    helpers.drawCamera(mesh, model1, 1.0);

    mesh.dumpPLY("debug.ply");

}


TEST(Rectification, testEssentialDecompositionToRotations)
{
    EssentialDecomposition D;
    D.direction = Vector3dd(5,2,1);
    D.rotation  =
            // Matrix33::Identity();
            //Quaternion::RotationX(degToRad(10)).toMatrix();
             (Quaternion::RotationZ(degToRad(25)) ^ Quaternion::RotationY(degToRad(35)) ^ Quaternion::RotationX(degToRad(40))).toMatrix();

    Quaternion q1 = Quaternion::Identity();
    Quaternion q2 = Quaternion::Identity();

    D.toTwoRotations(q1,q2);
    cout << q1 << endl;
    cout << q2 << endl;

    Mesh3D debug;
    debug.switchColor();

    debug.addOrts(1.0, true);
    debug.mulTransform(D.toSecondCameraAffine());
    debug.addOrts(1.0, true);
    debug.popTransform();


    debug.mulTransform(Affine3DQ(q1));
    debug.addOrts(1.0);
    debug.popTransform();

    debug.mulTransform(Affine3DQ::Shift( Vector3dd::OrtX() * D.direction.l2Metric() ));
    debug.mulTransform(Affine3DQ(q2));
    debug.addOrts(1.0);
    debug.popTransform();
    debug.popTransform();



    /* Some Checks */
    Vector3dd A1x = Vector3dd::OrtX();
    Vector3dd A1y = Vector3dd::OrtY();
    Vector3dd A1z = Vector3dd::OrtZ();

    Vector3dd A2x = D.rotation * Vector3dd::OrtX() + D.direction;
    Vector3dd A2y = D.rotation * Vector3dd::OrtY() + D.direction;
    Vector3dd A2z = D.rotation * Vector3dd::OrtZ() + D.direction;

    Vector3dd B1x = q1 * Vector3dd::OrtX();
    Vector3dd B1y = q1 * Vector3dd::OrtY();
    Vector3dd B1z = q1 * Vector3dd::OrtZ();

    Vector3dd B2x = q2 * Vector3dd::OrtX() + Vector3dd::OrtX() * D.direction.l2Metric();
    Vector3dd B2y = q2 * Vector3dd::OrtY() + Vector3dd::OrtX() * D.direction.l2Metric();
    Vector3dd B2z = q2 * Vector3dd::OrtZ() + Vector3dd::OrtX() * D.direction.l2Metric();

    cout << "Debug" << debug.vertexes.size() << endl;

    double r = 0.05;

    debug.setColor(RGBColor::Red());
    debug.addIcoSphere(A1x, r); debug.addIcoSphere(A1y, r); debug.addIcoSphere(A1z, r);
    debug.addIcoSphere(A2x, r); debug.addIcoSphere(A2y, r); debug.addIcoSphere(A2z, r);

    debug.setColor(RGBColor::Green());
    debug.addIcoSphere(B1x, r); debug.addIcoSphere(B1y, r); debug.addIcoSphere(B1z, r);
    debug.addIcoSphere(B2x, r); debug.addIcoSphere(B2y, r); debug.addIcoSphere(B2z, r);

    cout << "Debug" << debug.vertexes.size() << endl;

    cout << (A1x-A2x) << " " << !(A1x-A2x) << " vs " <<  (B1x-B2x) << " " << !(B1x-B2x) << endl;
    cout << (A1y-A2y) << " " << !(A1y-A2y) << " vs " <<  (B1y-B2y) << " " << !(B1y-B2y) << endl;
    cout << (A1z-A2z) << " " << !(A1z-A2z) << " vs " <<  (B1z-B2z) << " " << !(B1z-B2z) << endl;

    debug.dumpPLY("decomposeR.ply");

    CORE_ASSERT_DOUBLE_EQUAL_E(!(A1x-A2x),!(B1x-B2x), 1e-7, "This is not a shift X");
    CORE_ASSERT_DOUBLE_EQUAL_E(!(A1y-A2y),!(B1y-B2y), 1e-7, "This is not a shift Y");
    CORE_ASSERT_DOUBLE_EQUAL_E(!(A1z-A2z),!(B1z-B2z), 1e-7, "This is not a shift Z");
}

TEST(Rectification, testMoveCameras)
{
    EssentialDecomposition D;
    D.direction = Vector3dd(5,2,1).normalised();
    D.rotation  =
            // Matrix33::Identity();
            // Quaternion::RotationX(degToRad(10)).toMatrix();
             (Quaternion::RotationZ(degToRad(25)) ^ Quaternion::RotationY(degToRad(35)) ^ Quaternion::RotationX(degToRad(40))).toMatrix();

    Quaternion q1 = Quaternion::Identity();
    Quaternion q2 = Quaternion::Identity();
    D.toTwoRotations(q1,q2);

    /* */
    Affine3DQ posOrig1 = Affine3DQ::Shift(20, 10, 12) * Affine3DQ::RotationY(degToRad(10.0)) * Affine3DQ::RotationZ(degToRad( 10.0));
    Affine3DQ posOrig2 = Affine3DQ::Shift(27, 12, 11) * Affine3DQ::RotationX(degToRad(10.0)) * Affine3DQ::RotationZ(degToRad(-10.0));

    Vector3dd realDirection =  posOrig2.shift - posOrig1.shift;

    /* Ok Let's see how cameras are positioned now */

    Vector3dd dir1 = posOrig1.rotor * Vector3dd::OrtZ();
    Vector3dd dir2 = posOrig2.rotor * Vector3dd::OrtZ();

    Vector3dd ox = realDirection.normalised();
    Vector3dd oz1 = (dir1 + dir2).normalised();
    Vector3dd oy = (oz1 ^ ox).normalised();
    Vector3dd oz = (ox ^ oy).normalised();
    Matrix33 currentRotationM = Matrix33::FromColumns(ox, oy, oz);  /*Common rotation frame*/
    SYNC_PRINT(("Rotation is right: %lf\n", currentRotationM.det()));

    Quaternion currentRotation = Quaternion::FromMatrix(currentRotationM);

    /* */

    Affine3DQ pos1 = posOrig1;
    Affine3DQ pos2 = posOrig2;
    pos1.rotor = currentRotation ^ q1;
    pos2.rotor = currentRotation ^ q2;

    Affine3DQ pos1id = posOrig1;
    Affine3DQ pos2id = posOrig2;
    pos1id.rotor = currentRotation;
    pos2id.rotor = currentRotation;


    Mesh3D debug;
    debug.switchColor();

#if 1
    /* Some Checks */
    double scale = realDirection.l2Metric();

    Vector3dd A1o = Vector3dd::Zero();
    Vector3dd A1x = Vector3dd::OrtX();
    Vector3dd A1y = Vector3dd::OrtY();
    Vector3dd A1z = Vector3dd::OrtZ();

    Vector3dd B1o = pos1 * Vector3dd::Zero();
    Vector3dd B1x = pos1 * Vector3dd::OrtX();
    Vector3dd B1y = pos1 * Vector3dd::OrtY();
    Vector3dd B1z = pos1 * Vector3dd::OrtZ();

    Vector3dd A2o = D.rotation * Vector3dd::Zero() + D.direction * scale;
    Vector3dd A2x = D.rotation * Vector3dd::OrtX() + D.direction * scale;
    Vector3dd A2y = D.rotation * Vector3dd::OrtY() + D.direction * scale;
    Vector3dd A2z = D.rotation * Vector3dd::OrtZ() + D.direction * scale;

    Vector3dd B2o = pos2 * Vector3dd::Zero();
    Vector3dd B2x = pos2 * Vector3dd::OrtX();
    Vector3dd B2y = pos2 * Vector3dd::OrtY();
    Vector3dd B2z = pos2 * Vector3dd::OrtZ();

    cout << "Debug" << debug.vertexes.size() << endl;

    double r = 0.05;

    debug.setColor(RGBColor::Red());
    debug.addIcoSphere(A1o, r); debug.addIcoSphere(A1x, r); debug.addIcoSphere(A1y, r); debug.addIcoSphere(A1z, r);
    debug.addIcoSphere(A2o, r); debug.addIcoSphere(A2x, r); debug.addIcoSphere(A2y, r); debug.addIcoSphere(A2z, r);

    debug.setColor(RGBColor::Green());
    debug.addIcoSphere(B1o, r); debug.addIcoSphere(B1x, r); debug.addIcoSphere(B1y, r); debug.addIcoSphere(B1z, r);
    debug.addIcoSphere(B2o, r); debug.addIcoSphere(B2x, r); debug.addIcoSphere(B2y, r); debug.addIcoSphere(B2z, r);

    cout << "Debug" << debug.vertexes.size() << endl;

    cout << (A1x-A2x) << " " << !(A1x-A2x)  << " vs " <<  (B1x-B2x) << " " << !(B1x-B2x) << endl;
    cout << (A1y-A2y) << " " << !(A1y-A2y)  << " vs " <<  (B1y-B2y) << " " << !(B1y-B2y) << endl;
    cout << (A1z-A2z) << " " << !(A1z-A2z)  << " vs " <<  (B1z-B2z) << " " << !(B1z-B2z) << endl;

    debug.dumpPLY("moveR1.ply");

    CORE_ASSERT_DOUBLE_EQUAL_E(!(A1x-A2x),!(B1x-B2x), 1e-7, "This is not a shift X");
    CORE_ASSERT_DOUBLE_EQUAL_E(!(A1y-A2y),!(B1y-B2y), 1e-7, "This is not a shift Y");
    CORE_ASSERT_DOUBLE_EQUAL_E(!(A1z-A2z),!(B1z-B2z), 1e-7, "This is not a shift Z");

#endif


#if 1
    cout << "Sanity1:" << (currentRotation *  Vector3dd::OrtX()).normalised() << endl;
    cout << "Sanity2:" << realDirection.normalised() << endl;

    CameraModel dummy;
    dummy.intrinsics.reset(new PinholeCameraIntrinsics(Vector2dd(300,200), degToRad(50)));
    dummy.setLocation(Affine3DQ::Identity());

    CalibrationDrawHelpers drawer;
    //drawer.setSolidCameras(true);

    Vector3dd zero = posOrig1.shift;
    posOrig1.shift -= zero;
    posOrig2.shift -= zero;
    pos1.shift -= zero;
    pos2.shift -= zero;
    pos1id.shift -= zero;
    pos2id.shift -= zero;

    debug.setColor(RGBColor::Red());
    debug.mulTransform(posOrig1);
    drawer.drawCamera(debug, dummy, 2);
    debug.popTransform();

    debug.mulTransform(posOrig2);
    drawer.drawCamera(debug, dummy, 2);
    debug.popTransform();

    debug.setColor(RGBColor::Blue());
    debug.mulTransform(pos1id);
    drawer.drawCamera(debug, dummy, 2);
    debug.popTransform();

    debug.mulTransform(pos2id);
    drawer.drawCamera(debug, dummy, 2);
    debug.popTransform();

    debug.setColor(RGBColor::Green());
    debug.mulTransform(pos1);
    drawer.drawCamera(debug, dummy, 2);
    debug.popTransform();

    debug.mulTransform(pos2);
    drawer.drawCamera(debug, dummy, 2);
    debug.popTransform();


    debug.setColor(RGBColor::Cyan());
    debug.addLine(Vector3dd::Zero(), ox * 4);
    debug.setColor(RGBColor::Yellow());
    debug.addLine(Vector3dd::Zero(), dir1 * 3);
    debug.addLine(Vector3dd::Zero(), dir2 * 3);
    debug.addLine(Vector3dd::Zero(), oz1 * 4);
    debug.setColor(RGBColor::Magenta());
    debug.addLine(Vector3dd::Zero(), oy * 4);
    debug.setColor(RGBColor::Khaki());
    debug.addLine(Vector3dd::Zero(), oz * 4);

    debug.dumpPLY("moveR.ply");
#endif




}

#endif

//int main (int /*argC*/, char ** /*argV*/)
//{
//    //vector<int> a;
//    //vector<int>::iterator b = find (a.begin(), a.end(), 1);
//    //printf("%s\n", b == a.end() ? "true" : "flase");

//    //testSVDDesc();
//    //testFundamentalEstimator ();
//    testComposeDecompose();


//}

