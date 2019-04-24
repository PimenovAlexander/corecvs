#ifndef ERTFINDER_H
#define ERTFINDER_H

#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include "convertors.h"


class ertFinder
{
public:
    double focal;
    double x;
    double y;
    ertFinder(double focal, double x, double y);

    void findEssentialMatrix(corecvs::Matrix33 *E, corecvs::FlowBuffer *flow);

    void findRt(corecvs::Matrix33 *R, corecvs::Vector3dd *t, corecvs::Matrix33 E, corecvs::FlowBuffer *flow);

};

#endif // ERTFINDER_H
