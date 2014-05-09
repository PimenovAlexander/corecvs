#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>

#include "global.h"

#include "faceDetect.h"
#include "OpenCVTools.h"


using namespace std;
using namespace cv;

const char * FaceDetectorOpenCV::cascadeNames[] = {
        "data/openCV/haarcascade_profileface.xml",
        "data/openCV/haarcascade_frontalface_alt.xml",
        "data/openCV/haarcascade_frontalface_alt2.xml",
        "data/openCV/haarcascade_frontalface_default.xml",
        "data/openCV/haarcascade_frontalface_alt_tree.xml"
};

CascadeClassifier *FaceDetectorOpenCV::cascades[] = {NULL, NULL, NULL, NULL, NULL};

STATIC_ASSERT(CORE_COUNT_OF(FaceDetectorOpenCV::cascadeNames) ==
              CORE_COUNT_OF(FaceDetectorOpenCV::cascades), open_cv_cascades_data);

const int FaceDetectorOpenCV::MAX_ID = CORE_COUNT_OF(FaceDetectorOpenCV::cascadeNames);


void FaceDetectorOpenCV::detectFacesOpenCV(G12Buffer *input, vector<DetectedObject> *objects,
        int id,
        double scaleFactor,
        int minNeighbors,
        int minSize,
        int maxSize
)
{
    if (id < 0 || id >= MAX_ID) id = 2;

    if (cascades[id] == NULL)
    {
        cascades[id] = new CascadeClassifier();
        if( !cascades[id]->load( cascadeNames[id] ) )
        {
            cerr << "ERROR: Could not load classifier cascade:" << cascadeNames[id] << endl;
            return;
        }
    }

    CascadeClassifier *cascade = cascades[id];

    IplImage *inputIpl = OpenCVTools::getCVImageFromG12Buffer(input);
    Mat matrix(inputIpl, false);

    vector<Rect> faces;

    cascade->detectMultiScale(
            matrix
         ,  faces
         ,  scaleFactor
         , minNeighbors, 0 /*|CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_DO_ROUGH_SEARCH */ |CV_HAAR_SCALE_IMAGE
         , Size(minSize, minSize)
         , Size(maxSize, maxSize)
    );
    cvReleaseImage(&inputIpl);

//    printf("Detected %lu faces:\n", sfaces.size());

    for (vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); ++r)
    {
        int x = cvRound(r->x + r->width  * 0.5);
        int y = cvRound(r->y + r->height * 0.5);
        int h = cvRound(r->height);
        int w = cvRound(r->width);

        DetectedObject object(Vector2dd(x,y), Vector2dd(w,h));
        objects->push_back(object);
        //printf("  Center [%d %d] h=%d w=%d\n", x, y, h, w);
    }
}

FaceDetectorOpenCV::~FaceDetectorOpenCV()
{
    for(int id = 0; id < MAX_ID; id++)
    {
        delete_safe(cascades[id]);  // TODO: crashed here on invalid usage of this block!
    }
}
