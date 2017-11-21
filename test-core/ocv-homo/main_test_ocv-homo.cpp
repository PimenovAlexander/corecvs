/**
 * \file main_test_ocv-homo.cpp
 * \brief This is the main file for the test ocv-homo 
 *
 * \date Aug 13, 2011
 * \author alexander
 *
 */
#ifndef WITH_OPENCV

#include <stdio.h>

int main (int /*argC*/, char** /*argV*/)
{
    printf("FAILED: OpenCV library was not found on this PC.\n");
    return -1;
}

#else // !WITH_OPENCV

#include <iostream>
#include <opencv2/highgui/highgui.hpp>

#include "core/utils/global.h"

#include "core/math/vector/vector2d.h"

using namespace std;

#if 0
int main (int /*argC*/, char** /*argV*/)
{
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

    CvMat *points1 = cvCreateMat(1,5,CV_64FC2);
    CvMat *points2 = cvCreateMat(1,5,CV_64FC2);

    for (int i = 0; i < 5; i++)
    {
        points1->data.db[i * 2]     = points[i].x();
        points1->data.db[i * 2 + 1] = points[i].y();

        points2->data.db[i * 2]     = images[i].x();
        points2->data.db[i * 2 + 1] = images[i].y();
    }

    CvMat *H = cvCreateMat( 3, 3, CV_64F);
    cvFindHomography( points1, points2, H);

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            cout << CV_MAT_ELEM( *H, double, j, i) << " ";
        }
        cout << endl;
    }

        cout << "PASSED" << endl;

        return 0;
}

#endif

int main(int /*argc*/, char* /*argv*/[])
{
    IplImage* frame1 = 0;
    IplImage* frame2 = 0;

    int domain = CV_CAP_ANY;
    int camId1 = domain + 0;
    int camId2 = domain + 0;

    CvCapture* capture1 = cvCaptureFromCAM(camId1); // capture from camera with index 0
    CvCapture* capture2 = cvCaptureFromCAM(camId2); // capture from camera with index 1
    printf("Capturing from ids 0x%x and 0x%x\n", camId1, camId2);
    printf("Capturing devices 0x%p 0x%p\n", (void *)capture1, (void *)capture2);
    fflush(stdout);

    // start of camera capture

    while(1)
    {
        frame1 = cvQueryFrame( capture1 );
        frame2 = cvQueryFrame( capture2 );

        if( !frame1 || !frame2 )
        {
           printf("Couldn't capture from camera");
           fflush(stdout);
           break;
        }
        cvShowImage("camera1", frame1);
        cvShowImage("camera2", frame2);

        char c = cvWaitKey(33);
        if( c == 27 ) break; // if user press ESC, stop the process

    }
    cvReleaseCapture( &capture1 );
    cvReleaseCapture( &capture2 );

    return 0;
}

#endif // WITH_OPENCV
