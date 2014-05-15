#pragma once

#include "rgb24Buffer.h"
#include "../../utils/corestructs/g12Image.h"

using corecvs::RGB24Buffer;

class HoughSpace
{

public:
    HoughSpace(G12Buffer *image);
    G12Buffer* getHoughImage(bool withCorrection);
    QLine* getLineOfPoint(QPoint *point);
    ~HoughSpace();

private:
    G12Buffer *originalImage;
    AbstractBuffer<double> *accumulator;
    int numberOfAngles = 500;
    int numberOfDistances;
    double maxDist;
    double scale = 2;
};

