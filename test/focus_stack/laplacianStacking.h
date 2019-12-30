#ifndef LAPLACIANSTACKING_H
#define LAPLACIANSTACKING_H

#include "core/buffers/rgb24/rgb24Buffer.h"
#include "fsAlgorithm.h"
using namespace std;

/**
 *       This class is a implementation of pixel based focus stacking algorithm with laplacian focus measure.
 *    Algorithm blurs images of stack with Gaussian 5x5 to prevent changing of the result image by noises.
 *    The size of filter isn't larger as we can spoil the image too.
 *
 *      In the second step laplacian filter 5x5 is applied to all images. It is used as a focus measure operator
 *    which helps to find sharp points using idea that sharp area has a lot of borders which are detected with
 *    laplacian as high absolute values of pixels in image to which laplacian filter was applied. The size of
 *    laplacian shouldn't be higher because its computation will be too slow with little difference in result.
 *    In the third step algorithm finds maximum values for every pixel across all laplacians of stack and save
 *    a depth map - structure which contains number of image, where the maximum was found, in the cell with
 *    pixel coordinates. Then the result is created. The value of pixel (x, y) of it is taken from (x, y) pixel
 *    of image with number depthMap(x, y).
 **/

class LaplacianStacking : public FSAlgorithm
{
public:
    LaplacianStacking(){}
    void doStacking(vector<corecvs::RGB24Buffer *> & imageStack, corecvs::RGB24Buffer * result) override;
    ~LaplacianStacking() override {}
};

#endif // LAPLACIANSTACKING_H
