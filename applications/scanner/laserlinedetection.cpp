/*#include "laserlinedetection.h"

LaserLineDetection::LaserLineDetection(RGB24Buffer *frame = NULL, KernelGenerator *generator = NULL) :
    frame(frame),
    generator(generator)
{}

void LaserLineDetection::setFrame(RGB24Buffer *frame)
{
    this->frame = frame;
}

void LaserLineDetection::setGenerator(KernelGenerator *generator)
{
    this->generator = generator;
}


AbstractBuffer<double> *LaserLineDetection::calculateConvolutionNaive()
{
    Kernel1D kernel;

    if (generator == NULL || frame = NULL)
        return NULL;

    generator->generate(kernel);



    AbstractBuffer<double> *result = new AbstractBuffer<double>(frame->getSize())
    ge
}

vector<double> LaserLineDetection::extractLaserPoints()
{
}*/
