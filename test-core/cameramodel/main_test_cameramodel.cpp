/**
 * \file main_test_cameramodel.cpp
 * \brief This is the main file for the test cameramodel
 *
 * \date Nov 21, 2012
 * \author apimenov
 *
 * \ingroup autotest
 */

#include <iostream>
#include "gtest/gtest.h"

#include "global.h"
#include "g12Buffer.h"
#include "ppmLoader.h"
#include "mathUtils.h"
#include "vector3d.h"
#include "matrix44.h"
#include "vector2d.h"
#include "cameraParameters.h"
#include "matrix33.h"
#include "bufferFactory.h"
#include "bmpLoader.h"
#include "mathUtils.h"
#include "abstractPainter.h"

using corecvs::G12Buffer;
using corecvs::PPMLoader;
using corecvs::PPMLoader;
using corecvs::lerp;
using corecvs::Vector3dd;
using corecvs::Matrix44;
using corecvs::Vector2dd;
using corecvs::CameraIntrinsicsLegacy;
using corecvs::Matrix33;
using corecvs::BufferFactory;
using corecvs::BMPLoader;
using corecvs::fround;
using corecvs::AbstractPainter;

TEST(Cameramodel, generateReality)
{
	//const double BASELINE = -210;
	const double FOCAL    =  640;

	const int RESOLUTION_X = 640;
	const int RESOLUTION_Y = 480;
	/**
	 *   Camera positions and orientations
	 *
	 *    |..........|
	 *        60mm
	 *
	 **/
	Vector3dd  leftCameraPosition = Vector3dd(  -350.0  ,-30.0, 115.0);
	Vector3dd rightCameraPosition = Vector3dd(  -140.0  ,-30.0, 120.0);

	//Vector3dd direction = rightCameraPosition.normalised();

	Matrix44  iden4 = Matrix44(1.0);

	Matrix33 rot  = Matrix33::RotationY(corecvs::degToRad(22)) * Matrix33::RotationX(corecvs::degToRad(0));
	Matrix44 rot4 = Matrix44(rot, Vector3dd(0.0));

	Matrix44  leftCameraMatrix = Matrix44::Shift( leftCameraPosition) * rot4;
	Matrix44 rightCameraMatrix = Matrix44::Shift(rightCameraPosition) * iden4;

	/**
	 *  Camera intrinsics
	 **/

	Vector2dd imageResolution = Vector2dd(RESOLUTION_X, RESOLUTION_Y);

	CameraIntrinsicsLegacy  leftCameraIntrinsics(imageResolution,imageResolution / 2.0, FOCAL, 1.0);
	CameraIntrinsicsLegacy rightCameraIntrinsics(imageResolution,imageResolution / 2.0, FOCAL, 1.0);

	Matrix44 LK  = leftCameraIntrinsics .getKMatrix();
	Matrix44 RK  = rightCameraIntrinsics.getKMatrix();

	Matrix44 leftCamera  = LK * leftCameraMatrix.inverted();
	Matrix44 rightCamera = RK * rightCameraMatrix.inverted();

    cout << "Left Matrix :\n"  << leftCamera  << "\n";
	cout << "Right Matrix:\n" << rightCamera << "\n";

	/* Now let's project some points */

	struct Billboard {
		double x1;
		double y1;
	    double z;
		double w;
		double h;
        double dz;
		const char *name;
	};

	Billboard scene[] = {
			//{-600, -200, 600, 200, 1000},
			//  x1    y1   z    w   h    dz      name
			/*{-300, -290,  1250,195, 290,  0, "SUPPLY"}, // SUPPLY
			{-440, -200,  1000,135, 200,  0, "TEXET" }, // TEXET
			{-130,  -82,  750 ,125,  82,  0, "RHOS"  }, // ROHS
			{   0, -240,  500 ,205, 240,  0 , "AVR"   } // AVR*/

			{-400, -135,  800, 200, 135,  0, "TEXT"  }, // ROHS
			{-200, -240,  600, 205, 240,  0, "AVR"   }, // AVR
			{-280, -45,   400,  65,  45,  0, "CCTVB" }, // TEXET
			{-165, -70,   340,  60,  70,  0, "CCTVW" } // SUPPLY
	};


   	G12Buffer *input = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c0.pgm");
    CORE_ASSERT_TRUE(input != NULL, "The Cameramodel data is absent");

	G12Buffer *outputL = new G12Buffer(RESOLUTION_Y, RESOLUTION_X);
	G12Buffer *outputR = new G12Buffer(RESOLUTION_Y, RESOLUTION_X);

	for(unsigned k = 0; k < CORE_COUNT_OF(scene); k++)
	{
		Billboard *billboard = &(scene[k]);
		int disp;

		G12Buffer *inputText = new G12Buffer(input);
		AbstractPainter<G12Buffer> painter(inputText);
		painter.drawFormat(20, 20, G12Buffer::BUFFER_MAX_VALUE, 1, "%s", billboard->name);

		for (int i = 0; i < input->h; i++)
		{
			for (int j = 0; j < input->w; j++)
			{
				double x = lerp(billboard->x1, billboard->x1 + billboard->w , j, 0, inputText->w);
				double y = lerp(billboard->y1, billboard->y1 + billboard->h , i, 0, inputText->h);
                double z = lerp(billboard->z , billboard->z  + billboard->dz, j, 0, inputText->w);

				Vector3dd point = Vector3dd(x, y, z);
                Vector2dd imageL = ( leftCamera * point).project();
                Vector2dd imageR = (rightCamera * point).project();

				if (outputL->isValidCoord(imageL.y(), imageL.x()))
					outputL->element(imageL.y(), imageL.x()) = inputText->element(i,j);
				if (outputR->isValidCoord(imageR.y(), imageR.x()))
					outputR->element(imageR.y(), imageR.x()) = inputText->element(i,j);

				if ((i == 0 && j == 0) ||
			        (i == input->h - 1 && j == input->w - 1))
				{
					disp = fround(imageL.x() - imageR.x());
					printf("%s - Disparity: %d\n", billboard->name, disp);
				}
			}
		}
		delete_safe(inputText);
	}

	AbstractPainter<G12Buffer> painterL(outputL);
	painterL.drawChar(10,10, 'L',  G12Buffer::BUFFER_MAX_VALUE);
   	BMPLoader().save("out_c0.bmp", outputL);

	AbstractPainter<G12Buffer> painterR(outputR);
	painterR.drawChar(10,10, 'R',  G12Buffer::BUFFER_MAX_VALUE);
   	BMPLoader().save("out_c1.bmp", outputR);

    delete_safe(outputR);
    delete_safe(outputL);
    delete_safe(input);
}

#if NONFREE
#include "calibrationCamera.h"

using corecvs::PinholeCameraIntrinsics;

TEST(Cameramodel, newPinholeCameraIntrinsics)
{
    PinholeCameraIntrinsics input;

    SYNC_PRINT((" Testing matrix \n"));

    PrinterVisitor printer;
    SYNC_PRINT(("Default input\n"));
    input.accept(printer);

    input.skew = 0.01;
    input.focal = Vector2dd(input.size.x() - 100, input.size.x() + 100);
    SYNC_PRINT(("With skew and focal\n"));
    input.accept(printer);

    Matrix44 forward  = input.getKMatrix();
    Matrix44 backward = input.getInvKMatrix();
    SYNC_PRINT(("Forward Matrix\n"));
    cout << forward << endl;
    SYNC_PRINT(("Forward Matrix\n"));
    cout << backward << endl;

    SYNC_PRINT(("Product Matrix\n"));
    cout << (backward * forward) << endl;

    ASSERT_TRUE( (forward * backward).notTooFar(Matrix44(1.0), 1e-10));

    SYNC_PRINT((" Testing point projection \n"));

    Vector3dd P(100.0, 200.0, 300.0);

    Vector2dd im = input.project(P);
    Vector3dd im1A = input.getKMatrix33() * P;
    Vector2dd im1 =  im1A.xy() / im1A.z();

    cout << "Point " << P << endl;
    cout << "PinholeCameraIntrinsics::project " << im  << endl;
    cout << "PinholeCameraIntrinsics::matrix "  << im1 << endl;

    ASSERT_TRUE(im.notTooFar(im1, 1e-10));

    Vector3dd back  = input.reverse(im);
    Vector3dd back1 = input.getInvKMatrix33() * Vector3dd(im.x(), im.y(), 1.0);

    cout << "PinholeCameraIntrinsics::reverse " << back  << endl;
    cout << "PinholeCameraIntrinsics::matrix "  << back1 << endl;

    ASSERT_TRUE(back.notTooFar(back1, 1e-4));

}
#endif
