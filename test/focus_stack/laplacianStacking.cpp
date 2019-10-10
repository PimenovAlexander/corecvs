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
    Laplace5x5 <DpKernel> laplace5x5(128.0);
    Vector2d<int> imageSize = imageStack[0]->getSize();
    AbstractBuffer<double> maxValue(imageSize);
    AbstractBuffer<int> depthMap(imageSize);
    for (int i = 0; i < imageStack.size(); i++) {
        DpImage * outputG = new DpImage(imageSize);
        DpImage * outputL = new DpImage(imageSize);
        DpImage * input = imageStack[i]->getChannelDp(ImageChannel::GRAY);
        Convolver().convolve(*input, gaussian5x5, *outputG);
        Convolver().convolve(*outputG, laplace5x5, *outputL);
        for (int w = 0; w < imageSize.x() - 1; w++) {
            for (int h = 0; h < imageSize.y() - 1; h++) {
                if (maxValue.element(h, w) < outputG->element(h, w)) {
                    maxValue.element(h, w) = outputG->element(h, w);
                    depthMap.element(h, w) = i;
                }
            }
        }
        delete_safe(outputG);
        delete_safe(outputL);
        delete_safe(input);
    }

    for (int w = 0; w < imageSize.x(); w++) {
        for (int h = 0; h < imageSize.y(); h++) {
            result->element(h, w) = imageStack[depthMap.element(h, w)]->element(h, w);
        }
    }
}
