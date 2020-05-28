#ifndef OPENCVDATALOADER_H
#define OPENCVDATALOADER_H

#include "tinyxml2/tinyxml2.h"
#include "math/matrix/matrix.h"

#include "cameracalibration/cameraModel.h"

namespace corecvs {


class OpenCVDataLoader
{
public:
    OpenCVDataLoader();

    static int readInt(tinyxml2::XMLNode *node, const char *name);
    static Matrix readMatrix (tinyxml2::XMLNode *node);

    static AbstractBuffer<Vector2dd> readMatrix2f (tinyxml2::XMLNode *node);


    /* Loading calibration */
    struct OpenCVCAL{
        int H;
        int W;

        int bH;
        int bW;

        double sqSize;

        CameraModel camera;
        vector<Affine3DQ> positions;
    };

    static OpenCVCAL parseOpenCVOutput(const char *filename);

};


} // namespace corecvs

#endif // OPENCVDATALOADER_H
