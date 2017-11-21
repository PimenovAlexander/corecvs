/**
 * \file main_test_openCV.cpp
 * \brief This is the main file for the test openCV 
 *
 * \date нояб. 26, 2012
 * \author alex
 *
 * \ingroup autotest  
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

#include "core/utils/global.h"

#include "core/buffers/g12Buffer.h"
#include "featureDetectorCV.h"
#include "core/fileformats/bmpLoader.h"
#include "core/utils/preciseTimer.h"


using namespace std;

int main (int /*argC*/, char ** /*argV*/)
{
    BMPLoader *loader = new BMPLoader();
    cout << "Test begin" << endl;
    G12Buffer *image   = loader->load("../data/Image4.bmp");
    G12Buffer *pattern = loader->load("../data/pattern.bmp");
    //G12Buffer *image   = loader->load("../data/left.bmp");
    //G12Buffer *pattern = loader->load("../data/right.bmp");
    //RGB24Buffer *image = loader->loadRGB("../data/scene1.row3.col1.BMP");
    //RGB24Buffer *pattern = loader->loadRGB("../data/scene1.row3.col2.BMP");
    //RGB24Buffer *image = loader->loadRGB("../data/bmp3.bmp");
    //RGB24Buffer *pattern = loader->loadRGB("../data/extinguisher.bmp");

    if (image != NULL && pattern != NULL)
    {
        cout << "Buffers created" << endl;
        FeatureDetectorCV<SURF> surfer;
        //FeatureDetectorCV<SURF> surfer( 100.
        //                 , 3
        //                 , 4
        //                 , false
        //);
        cout << "Surfer created" << endl;
//        PreciseTimer timer = PreciseTimer::currentTime();
//        for (int i = 0; i < 100; i++)
//        {
//            surfer.calculate(image, pattern);
//        }
        surfer.calculate(image, pattern);

        //cout << "Surfer done in: " << (timer.usecsToNow() / 1000.0 / 100.0) << "ms" << endl;

        CorrespondenceList* result = new  CorrespondenceList(surfer.getCorrespondenceList());
        vector<KeyPoint> imagePoints   = surfer.getKeypointsImage();
        vector<KeyPoint> patternPoints = surfer.getKeypointsPattern();

        /* Save flow in .bmp. Square points to Right features */
        //RGB24Buffer *flowDump = new RGB24Buffer(image);
        RGB24Buffer *flowDump = new RGB24Buffer(image->getSize());
        flowDump->drawG12Buffer(image);

//        vector<KeyPoint>::iterator it;
//        for (it = imagePoints.begin(); it != imagePoints.end(); ++it)
//        {
//            KeyPoint point = (*it);
//            flowDump->drawArc(int(point.pt.x), int(point.pt.y), int(point.size), RGBColor(0, 0, 0xFF));
//            cout  << int(point.pt.x) << "  "<< int(point.pt.y) <<  "   " <<  int(point.size) << endl;
//        }

        flowDump->drawG12Buffer(pattern);

        flowDump->drawCorrespondenceList(result);
        BMPLoader().save("OUTPUT.bmp", flowDump);
        delete_safe(flowDump);

        cout << "Number of correspondences: " << result->size() << endl;

        while  (!result->empty())
        {
            Correspondence tmpCor = result->back();
            result->pop_back();
            cout << " from pattern (x,y)= (" << tmpCor.start.x() << ", " << tmpCor.start.y() << ")";
            cout << " to image (x,y)= ("   << tmpCor.end.x() << ", " << tmpCor.end.y() << ")";
            cout << endl;
        }

        delete_safe(result);
        delete_safe(image);
        delete_safe(pattern);

        cout << "PASSED" << endl;
    } else
    {
        cout << "FAULT. EMPTY BUFFER" << endl;
    }
    delete_safe(loader);

    return 0;
}

#endif // WITH_OPENCV
