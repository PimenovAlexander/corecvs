#ifndef FACE_DETECT_H
#define FACE_DETECT_H

#include <vector>

#include "g12Buffer.h"
#include "detectedObject.h"

using corecvs::G12Buffer;
using corecvs::DetectedObject;
using std::vector;


namespace cv
{
    class CascadeClassifier;
}

class FaceDetectorOpenCV
{
public:
    static cv::CascadeClassifier *cascades[];
    static const char *cascadeNames[];
    static const int MAX_ID;

    FaceDetectorOpenCV()
    {

    }

    void detectFacesOpenCV (
            G12Buffer *input,
            vector<DetectedObject> *objects, int id = 2,
            double scaleFactor = 1.1,
            int minNeighbors = 2,
            int minSize = 30,
            int maxSize = 100500);


    virtual ~FaceDetectorOpenCV();

};

#endif


