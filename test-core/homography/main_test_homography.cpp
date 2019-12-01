/**
 * \file main_test_homography.cpp
 * \brief This is the main file for the test homography 
 *
 * \date Jun 26, 2011
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include <cameracalibration/projection/pinholeCameraIntrinsics.h>
#include <cameracalibration/cameraModel.h>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/math/vector/vector2d.h"
#include "core/math/matrix/homographyReconstructor.h"
#include "core/math/matrix/matrix33.h"

#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/bmpLoader.h"

using namespace corecvs;

/**
 * Trival homography test
 **/
TEST(Homography, testProjectiveFromPoints)
{
    cout << "Test 4 point to point reconstruction" << endl;
    Vector2dd points[4] = {
            Vector2dd(0,0), Vector2dd(1,0), Vector2dd(1,1), Vector2dd(0,1)
    };
    Vector2dd images[4] = {
            Vector2dd(0,0), Vector2dd(1,0), Vector2dd(1,1), Vector2dd(0,1)
    };

    HomographyReconstructor reconstructor;
    for (unsigned i = 0; i < 4; i++)
        reconstructor.addPoint2PointConstraint(points[i], images[i]);

    Matrix33 result = reconstructor.getBestHomographyLSE();

    for (unsigned i = 0; i < 4; i++)
    {
        cout << "Predicted:" << result * points[i] << "<->" << images[i] << endl;
    }

    CORE_ASSERT_TRUE(result.notTooFar(Matrix33(1.0), 1e-7), "Wrong points reconstructions");
}

TEST(Homography, testProjectiveInverse)
{
    Matrix33 R = Matrix33::RotationZ(0.4);
    Matrix33 S = Matrix33::ShiftProj(3,4);

    Matrix33 H    = S*R;
    Matrix33 Hinv = H.inv();

    cout << "Test 4 point to point reconstruction" << endl;
    Vector2dd points[4] = {
            Vector2dd(0,0), Vector2dd(1,0), Vector2dd(1,1), Vector2dd(0,1)
    };
    Vector2dd images[4];

    for (unsigned i = 0; i < 4; i++)
    {
        images[i] = H * points[i];
    }

    HomographyReconstructor p2i;
    HomographyReconstructor i2p;


    for (unsigned i = 0; i < 4; i++)
    {
        p2i.addPoint2PointConstraint(points[i], images[i]);
        i2p.addPoint2PointConstraint(images[i], points[i]);
    }

    Matrix33 gH    = p2i.getBestHomographyLSE();
    Matrix33 gHinv = i2p.getBestHomographyLSE();

    for (unsigned i = 0; i < 4; i++)
    {
        cout << "Predicted:" << gH * points[i] << "<->" << images[i] << endl;
    }

    cout << "H" << endl << H <<  endl;
    cout << "gH" << endl << gH <<  endl;
    cout << "Hinv" << endl << Hinv <<  endl;
    cout << "gHinv" << endl << gHinv <<  endl;

    CORE_ASSERT_TRUE(H.notTooFar(gH, 1e-7), "Wrong points reconstructions");
    CORE_ASSERT_TRUE(Hinv.notTooFar(gHinv, 1e-7), "Wrong points reconstructions");
}

TEST(Homography, testProjectiveFromPoints2)
{
    static unsigned const NUM = 5;
    cout << "Test " << NUM << " point to point reconstruction" << endl;

    Vector2dd points[5] = {
        Vector2dd ( 32.4,  88.9),
        Vector2dd (177.8,  88.9),
        Vector2dd (281.9,  59.7),
        Vector2dd (281.9,  88.9),
        Vector2dd (281.9, 118.1)
    };

    Vector2dd images[5] = {
         Vector2dd ( 536.0, 372.0),
         Vector2dd ( 903.0, 463.0),
         Vector2dd (1324.0, 514.0),
         Vector2dd (1277.0, 551.0),
         Vector2dd (1219.0, 597.0)
    };

    Line2d  cushionsLine[11] = {
        Line2d (-0, 355.6, -933.45),
        Line2d (-0, 355.6, -933.45),
        Line2d (-0, 355.6, -933.45),
        Line2d (-177.8, 0, 62759),
        Line2d (-177.8, 0, 62759),
        Line2d (-0, -355.6, 62292.2),
        Line2d (-0, -355.6, 62292.2),
        Line2d (-0, -355.6, 62292.2),
        Line2d (-0, -355.6, 62292.2),
        Line2d (177.8, 0, -466.725),
        Line2d (177.8, 0, -466.725)
    };

    Vector2dd cushionImage[11] = {
        Vector2dd (731.0, 323.0),
        Vector2dd (1168.0, 406.0),
        Vector2dd (1534.0, 476.0),
        Vector2dd (1674.0, 536.0),
        Vector2dd (1540.0, 756.0),
        Vector2dd (275.0, 443.0),
        Vector2dd (542.0, 532.0),
        Vector2dd (783.0, 611.0),
        Vector2dd (1320.0, 788.0),
        Vector2dd (614.0, 324.0),
        Vector2dd (338.0, 398.0),
    };

 /*Vector2dd points[5 * 2] = {
         Vector2dd(0,0)    , Vector2dd(0,1),
         Vector2dd(1,0)    , Vector2dd(3,1),
         Vector2dd(1,1)    , Vector2dd(3,4),
         Vector2dd(0,1)    , Vector2dd(0,4),
         Vector2dd(0.5,1.5), Vector2dd(1.5,5.5)
 };*/

    Vector2dd corners[4] = {
         Vector2dd(  0.0,   0.0),
         Vector2dd(355.6,   0.0),
         Vector2dd(355.6, 178.8),
         Vector2dd(  0.0, 178.8)
    };

    Vector2dd cornersImage[4] = {
         Vector2dd( 669.0, 298.0),
         Vector2dd(1714.0, 496.0),
         Vector2dd(1506.0, 861.0),
         Vector2dd( 203.0, 424.0)
    };


    HomographyReconstructor reconstructor;
    HomographyReconstructor reconstructorN;
    HomographyReconstructor reconstructorI;

    for (unsigned i = 0; i < NUM; i++)
    {
        reconstructor .addPoint2PointConstraint(points[i], images[i]);
        reconstructorI.addPoint2PointConstraint(images[i], points[i]);
        reconstructorN.addPoint2PointConstraint(points[i], images[i]);
    }

    for (unsigned i = 0; i < 11; i++)
    {
        reconstructorI.addPoint2LineConstraint(cushionImage[i], cushionsLine[i]);
    }


    Matrix33 resultLSE = reconstructor.getBestHomographyLSE();
    Matrix33 resultLSE1 = reconstructor.getBestHomographyLSE1();
    Matrix33 resultLSE2 = reconstructor.getBestHomographyLSE2();

 //resultLSE2 /= resultLSE2.a(2,2);

 Matrix33 L;
 Matrix33 R;
 reconstructorN.normalisePoints(L, R);
 Matrix33 resultLSEn = reconstructorN.getBestHomographyLSE();
 Matrix33 resultLSEn1 = reconstructorN.getBestHomographyLSE1();
 Matrix33 resultLSEn2 = reconstructorN.getBestHomographyLSE2();

 resultLSEn  = R.inv() * resultLSEn  * L;
 resultLSEn1 = R.inv() * resultLSEn1 * L;
 resultLSEn2 = R.inv() * resultLSEn2 * L;

 reconstructorI.normalisePoints(L, R);
 Matrix33 resultLSEii = reconstructorI.getBestHomographyLSE1();
 Matrix33 resultLSEi = L.inv() * resultLSEii.inv() * R;

 Matrix33 resultOld = Matrix33::ProjectiveByPoints(NUM, points, images);

 cout << "=================Problem Statement===============================" << endl;
 cout << reconstructor << endl;
 cout << "=================Inverted Problem Statement======================" << endl;
 cout << reconstructorI << endl;
 cout << "=================================================================" << endl;

 cout << "LSE Cost Function is:" << sqrt(reconstructor.getCostFunction(resultLSE)) << endl;
 cout << "LSE"     << endl << resultLSE << endl;
 for (unsigned i = 0; i < 4; i++)
 {
     cout << "Predicted:" << i << "->" << resultLSE/*.inv()*/ * corners[i] << endl;
 }
 cout << endl;
 cout << "LSE1 Cost Function is:" << sqrt(reconstructor.getCostFunction(resultLSE1)) << endl;
 cout << "LSE1"     << endl << resultLSE1 << endl;
 for (unsigned i = 0; i < 4; i++)
 {
     cout << "Predicted:" << i << "->" << resultLSE1/*.inv()*/ * corners[i] << endl;
 }
 cout << endl;
 cout << "LSE2 Cost Function is:" << sqrt(reconstructor.getCostFunction(resultLSE2)) << endl;
 cout << "LSE2"     << endl << resultLSE2 << endl;
 for (unsigned i = 0; i < 4; i++)
 {
     cout << "Predicted:" << i << "->" << resultLSE2/*.inv()*/ * corners[i] << endl;
 }
 cout << endl;

 cout << "============================Inverted===========================" << endl;
 cout << "Inverted Cost Function is:" << sqrt(reconstructor.getCostFunction(resultLSEi)) << endl;
 cout << "Inverted Back Cost Function is:" << sqrt(reconstructorI.getCostFunction(resultLSEii)) << endl;
 cout << "Inverted"     << endl << resultLSEi << endl;
 cout << "InvertedBack" << endl << resultLSEii << endl;
 for (unsigned i = 0; i < 4; i++)
 {
     cout << "Predicted:" << i << "->" << resultLSEi/*.inv()*/ * corners[i] << endl;
 }
 cout << endl;
 cout << "===========================Normalized==========================" << endl;

 cout << "LSEn Cost Function is:" << sqrt(reconstructor.getCostFunction(resultLSEn)) << endl;
 cout << "LSEn"     << endl << resultLSEn << endl;
 for (unsigned i = 0; i < 4; i++)
 {
     cout << "Predicted:" << i << "->" << resultLSEn/*.inv()*/ * corners[i] << endl;
 }
 cout << endl;
 cout << "LSEn1 Cost Function is:" << sqrt(reconstructor.getCostFunction(resultLSEn1)) << endl;
 cout << "LSEn1"     << endl << resultLSEn1 << endl;
 for (unsigned i = 0; i < 4; i++)
 {
     cout << "Predicted:" << i << "->" << resultLSEn1/*.inv()*/ * corners[i] << endl;
 }
 cout << endl;
 cout << "LSEn2 Cost Function is:" << sqrt(reconstructor.getCostFunction(resultLSEn2)) << endl;
 cout << "LSEn2"     << endl << resultLSEn2 << endl;
 for (unsigned i = 0; i < 4; i++)
 {
     cout << "Predicted:" << i << "->" << resultLSEn2/*.inv()*/ * corners[i] << endl;
 }
 cout << endl;
 cout << "OLD Cost Function is:" << sqrt(reconstructor.getCostFunction(resultOld)) << endl;
 cout << "OLD"     << endl << resultOld << endl;
 for (unsigned i = 0; i < 4; i++)
 {
     cout << "Predicted:" << i << "->" << resultOld/*.inv()*/ * corners[i] << endl;
 }
 cout << endl;

 cout << "===========================Somehow close==========================" << endl;

 Matrix33 imageToTableMatrix = Matrix33::ProjectiveByPoints(corners, cornersImage);
 cout << "SOME Cost Function is:" << sqrt(reconstructor.getCostFunction(imageToTableMatrix)) << endl;
 cout << "SOME"     << endl << imageToTableMatrix << endl;
 cout << "Predicted:" << endl;
 for (unsigned i = 0; i < 4; i++)
 {
   cout << corners[i] << "->" << (imageToTableMatrix/*.inv()*/ * corners[i]) << "->" << cornersImage[i] << endl;
 }
 cout << "Reproject balls" << endl;
 for (unsigned i = 0; i < NUM; i++)
 {
     cout << points[i] << "->" << (imageToTableMatrix/*.inv()*/ * points[i]) << "->" << images[i] << endl;
 }

 cout << endl;
}

TEST(Homography, testProjectiveFromPoints1)
{
    static unsigned const NUM = 5;
    cout << "Test " << NUM << " point to point reconstruction" << endl;

    Vector2dd points[NUM] = {
            Vector2dd(0,0), Vector2dd(1,0), Vector2dd(1,1), Vector2dd(0,1), Vector2dd(0.5,1.5)
    };
    Vector2dd images[NUM] = {
            Vector2dd(0,1), Vector2dd(3,1), Vector2dd(3,4), Vector2dd(0,4), Vector2dd(1.5,5.5)
    };

    HomographyReconstructor reconstructor;
    for (unsigned i = 0; i < NUM; i++)
        reconstructor.addPoint2PointConstraint(points[i], images[i]);

    Matrix33 resultLSE  = reconstructor.getBestHomographyLSE();
    Matrix33 resultLSE1 = reconstructor.getBestHomographyLSE1();
    Matrix33 resultLSE2 = reconstructor.getBestHomographyLSE2();  
    Matrix33 resultLMq  = reconstructor.getBestHomographyLM();

    cout << "LSE Cost Function is:" << reconstructor.getCostFunction(resultLSE) << endl << endl;
    cout << "LSE"     << endl << resultLSE << endl;
    for (unsigned i = 0; i < NUM; i++)
    {
        cout << "Predicted:" << resultLSE * points[i] << "<->" << images[i] << endl;
    }

    cout << endl;
    cout << "LSE1 Cost Function is:" << reconstructor.getCostFunction(resultLSE1) << endl << endl;
    cout << "LSE1"     << endl << resultLSE1 << endl;
    for (unsigned i = 0; i < NUM; i++)
    {
        cout << "Predicted:" << resultLSE1 * points[i] << "<->" << images[i] << endl;
    }

    cout << endl;
    cout << "LSE2 Cost Function is:" << sqrt(reconstructor.getCostFunction(resultLSE2)) << endl;
    cout << "LSE2"     << endl << resultLSE2 << endl;
    for (unsigned i = 0; i < NUM; i++)
    {
        cout << "Predicted:" << resultLSE2 * points[i] << "<->" << images[i] << endl;
    }
    cout << endl;

    /*cout << "Kalman"  << endl << resultKal << endl;*/
    cout << "MarqLev" << endl << resultLMq << endl;

    Matrix33 realH = Matrix33::ShiftProj(0.0, 1.0) * Matrix33::Scale2(3.0);
    CORE_ASSERT_TRUE(resultLSE.notTooFar(realH, 1e-7), "Wrong points1 reconstructions");
    CORE_ASSERT_TRUE(resultLSE1.notTooFar(realH, 1e-7), "Wrong points1 reconstructions");
    CORE_ASSERT_TRUE(resultLSE2.notTooFar(realH, 1e-7), "Wrong points1 reconstructions");
}

TEST(Homography, testProjectiveFromLine)
{
    static unsigned const NUM = 8;
    cout << "Test " << NUM << " line to point reconstruction" << endl;

    Segment2d seg[NUM] = {
            Segment2d(Vector2dd(0.0,0.0), Vector2dd(2.0,0.0)),
            Segment2d(Vector2dd(2.0,0.0), Vector2dd(2.0,2.0)),
            Segment2d(Vector2dd(2.0,2.0), Vector2dd(0.0,2.0)),
            Segment2d(Vector2dd(0.0,2.0), Vector2dd(0.0,0.0)),

            Segment2d(Vector2dd(0.0,0.0), Vector2dd(2.0,0.0)),
            Segment2d(Vector2dd(2.0,0.0), Vector2dd(2.0,2.0)),
            Segment2d(Vector2dd(2.0,2.0), Vector2dd(0.0,2.0)),
            Segment2d(Vector2dd(0.0,2.0), Vector2dd(0.0,0.0))
    };

    Vector2dd points[NUM] = {
            Vector2dd(0.0,0.0),
            Vector2dd(1.0,0.0),
            Vector2dd(1.0,1.0),
            Vector2dd(0.0,1.0),

            Vector2dd(1.0,0.0),
            Vector2dd(1.0,1.0),
            Vector2dd(0.0,1.0),
            Vector2dd(0.0,0.0)
    };

    HomographyReconstructor reconstructor;
    for (unsigned i = 0; i < NUM; i++)
        reconstructor.addPoint2LineConstraint(points[i],Line2d(seg[i]));

    Matrix33 H = reconstructor.getBestHomographyLSE();
    cout << reconstructor << endl;

/*    for (unsigned i = 0; i < NUM; i++)
    {
        cout << "Predicted:" << result * seg[i].a << "<->" << images[i] << endl;
    }*/

    cout << "H" << endl;
    cout << H << endl;

    CORE_ASSERT_TRUE(H.notTooFar(Matrix33::Scale2(2.0), 1e-7), "Wrong line based reconstructions");
}


TEST(Homography, testStability)
{

    Vector2dd corners[9][2] = {
        {{ 0.0 , 0    } , {2437.83, 1692.57}},
        {{ 0.05, 0    } , {2441.2 , 1692.66}},
        {{ 0.1 , 0    } , {2445.79, 1692.79}},
        {{ 0.0 , 0.05 } , {2437.82, 1696.71}},
        {{ 0.05, 0.05 } , {2441.19, 1696.62}},
        {{ 0.1 , 0.05 } , {2445.78, 1696.49}},
        {{ 0.0 , 0.1  } , {2437.83, 1700.44}},
        {{ 0.05, 0.1  } , {2441.2 , 1700.54}},
        {{ 0.1 , 0.1  } , {2445.79, 1700.67}}
    };

    Matrix33 expected(
         -5813.48, 267.084, 2437.79,
         -4087.58, 255.313, 1693.1,
         -2.40965, 0.109391, 1);


    HomographyReconstructor reconstructor;
    RGB24Buffer *image = new RGB24Buffer(300, 300);
    Matrix33 transform = Matrix33::ScaleProj(30) * Matrix33::ShiftProj(-2436.0, -1691.5);

    for (unsigned i = 0; i < CORE_COUNT_OF(corners); i++)
    {
        reconstructor.addPoint2PointConstraint(corners[i][0],corners[i][1]);
        Vector2dd out = transform * corners[i][1];
        Vector2d<int> outi(fround(out.x()), fround(out.y()));

        if (image->isValidCoord(outi)) {
            image->element(outi) = RGBColor::White();
        }
    }

    Matrix33 H[HomographyAlgorithm::HOMOGRAPHY_ALGORITHM_LAST];

    cout << reconstructor << endl;


    for (unsigned i = 0; i < CORE_COUNT_OF(corners); i++)
    {
        {
            Vector2dd out = transform * expected * corners[i][0];
            Vector2d<int> outi(fround(out.x()), fround(out.y()));
            if (image->isValidCoord(outi)) {
                image->element(outi) = RGBColor::Green();
            }
        }
    }

    for (int method = 0; method < HomographyAlgorithm::HOMOGRAPHY_ALGORITHM_LAST; method++)
    {
        H[method] = reconstructor.getBestHomography((HomographyAlgorithm::HomographyAlgorithm)method);
        cout << "Method: " << HomographyAlgorithm::getName((HomographyAlgorithm::HomographyAlgorithm)method)  << endl;

        cout << H[method] << endl;
        for (unsigned i = 0; i < CORE_COUNT_OF(corners); i++)
        {
            Vector2dd out = transform * H[method] * corners[i][0];
            Vector2d<int> outi(fround(out.x()), fround(out.y()));
            if (image->isValidCoord(outi)) {
                image->element(outi) = RGBColor::rainbow(lerp(0.0, 1.0, method, 0.0, HomographyAlgorithm::HOMOGRAPHY_ALGORITHM_LAST));
            }
        }

        cout << "cost:" << reconstructor.getCostFunction(H[method]) << endl;

    }

    cout << "outsize:" << reconstructor.getCostFunction(expected) << endl;

    BMPLoaderBase().save("homograpy-draw.bmp", image);
}


/**
 *  This method attempts to restore camera position from the homography.
 **/
TEST(Homography, testPoseReconstrution)
{
    CameraModel model(
        new PinholeCameraIntrinsics(
               Vector2dd(500,500),
               Vector2dd(250,250),
               250.0
        )
    );


    Affine3DQ inputPoses[] =
        {
           Affine3DQ::Shift(0, 0, -10) * Affine3DQ::RotationX(degToRad(1)),
           Affine3DQ::Shift(1, 0, -10) * Affine3DQ::RotationX(degToRad(2)) * Affine3DQ::RotationY(degToRad(22)),
           Affine3DQ::Shift(0, 2, -10) * Affine3DQ::RotationX(degToRad(1)) * Affine3DQ::RotationZ(degToRad(-5)),
           Affine3DQ::Shift(1, 1, -10) * Affine3DQ::RotationY(degToRad(1)) * Affine3DQ::RotationZ(degToRad(-5))
        };

    for (int i = 0; i < CORE_COUNT_OF(inputPoses); i++)
    {
        Affine3DQ cameraPose = inputPoses[i];

        cout << "Pose Matrix\n" << endl;
        cout << (Matrix44)cameraPose << endl;
        model.setLocation(cameraPose);
        //cout << model.extrinsics

        /*Plane points*/
        Vector3dd p[4] = {
            Vector3dd(0,0,0),
            Vector3dd(1,0,0),
            Vector3dd(1,1,0),
            Vector3dd(0,1,0)
        };

        Vector2dd q[4];

        HomographyReconstructor reconstrutuctor;

        for (int i = 0; i < CORE_COUNT_OF(p); i++)
        {
            q[i] = model.project(p[i]);
            cout << "P " << p[i] << "  -  " << q[i] << endl;

            reconstrutuctor.addPoint2PointConstraint(p[i].xy(), q[i]);
        }

        Matrix33 H = reconstrutuctor.getBestHomography();
        cout << "Homography\n" << H << endl;

        PinholeCameraIntrinsics *pinhole = model.getPinhole();
        Matrix33 K = pinhole->getKMatrix33();

        /** Try to reconstruct **/
        Affine3DQ pose = HomographyReconstructor::getAffineFromHomography(K, H);
        cout << "Pose:" << endl;
        cout << (Matrix44)pose << endl;
        cout << pose << endl;

        CORE_ASSERT_TRUE(cameraPose.notToFar(pose, 1e-7), "Failed pose reconstruction");
    }
}
