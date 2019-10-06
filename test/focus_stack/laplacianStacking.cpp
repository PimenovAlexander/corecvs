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
    Gaussian5x5<DpKernel> gaussian5x5(true);
    Laplace3x3 <DpKernel> laplace3x3(128.0);
    Vector2d<int> imageSize = imageStack[0]->getSize();
    double ** maxValue = new double*[imageSize.y()];
    int ** depthMap = new int*[imageSize.y()];
    for (int i = 0; i < imageSize.y(); i++) {
        maxValue[i] = new double[imageSize.x()]{-DBL_MAX};
        depthMap[i] = new int[imageSize.x()]{0};
    }
    for (int i = 0; i < imageStack.size(); i++) {
        DpImage * outputG = new DpImage(imageSize);
        DpImage * outputL = new DpImage(imageSize);
        DpImage * input = imageStack[i]->getChannelDp(ImageChannel::GRAY);
        Convolver().convolve(*input, gaussian5x5, *outputG);
        Convolver().convolve(*outputG, laplace3x3, *outputL);
        //std::cout << outputG->getH() << " " << outputG->getW() << endl;
        for (int w = 0; w < imageSize.x() - 1; w++) {
            for (int h = 0; h < imageSize.y() - 1; h++) {
                if (maxValue[h][w] < outputG->elementBl(h, w)) {
                    maxValue[h][w] = outputG->elementBl(h, w);
                    depthMap[h][w] = i;
                }
            }
        }
        delete_safe(outputG);
        delete_safe(outputL);
        delete_safe(input);
    }

    for (int w = 0; w < imageSize.x(); w++) {
        for (int h = 0; h < imageSize.y(); h++) {
            result->drawPixel(w, h, imageStack[depthMap[h][w]]->elementBl(h, w));
        }
    }
}
