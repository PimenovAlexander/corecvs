#include "laplacianStacking.h"
#include "imageStack.h"
#include "core/buffers/kernels/gaussian.h"
#include "core/buffers/kernels/laplace.h"
#include "core/buffers/convolver/convolver.h"
#include <float.h>

LaplacianStacking::LaplacianStacking(){}

LaplacianStacking::~LaplacianStacking(){}

void LaplacianStacking::doStacking(vector<RGB24Buffer*> & imageStack, RGB24Buffer * result)
{
    Gaussian5x5<FpKernel> gaussian5x5f(true);
    Laplace3x3<FpKernel> laplace3x3f(128.0f);
    Vector2d<int> imageSize = imageStack[0]->getSize();
    double ** maxValue = new double*[imageSize.x()];
    int ** depthMap = new int*[imageSize.x()];
    for (int i = 0; i < imageSize.x(); i++) {
        maxValue[i] = new double[imageSize.y()]{-DBL_MAX};
        depthMap[i] = new int[imageSize.y()]{0};
    }
    for (int i = 0; i < imageStack.size(); i++) {
        DpImage * outputG = new DpImage(imageSize);
        DpImage * outputL = new DpImage(imageSize);
        Convolver().convolve(*imageStack[i], gaussian5x5f, *outputG);
        Convolver().convolve(*outputG, laplace3x3f, *outputL);
        for (int w = 0; w < imageSize.x(); w++) {
            for (int h = 0; h < imageSize.y(); h++) {
                if (maxValue[w][h] > outputG->elementBl(w, h)) {
                    maxValue[w][h] = outputG->elementBl(w, h);
                    depthMap[w][h] = i;
                }
            }
        }
        delete_safe(outputG);
    }

    for (int w = 0; w < imageSize.x(); w++) {
        for (int h = 0; h < imageSize.y(); h++) {
            result->drawPixel(w, h, imageStack[depthMap[w][h]]->elementBl(w, h));
        }
    }
}
