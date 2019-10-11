#include "laplacianStacking.h"
#include "imageStack.h"
#include "core/buffers/kernels/gaussian.h"
#include "core/buffers/kernels/laplace.h"
#include "core/buffers/convolver/convolver.h"
#include <float.h>
#include <core/utils/preciseTimer.h>

LaplacianStacking::LaplacianStacking(){}

LaplacianStacking::~LaplacianStacking(){}

void LaplacianStacking::doStacking(vector<RGB24Buffer*> & imageStack, RGB24Buffer * result)
{
    Gaussian5x5<DpKernel> gaussian5x5(true);
    Laplace5x5 <DpKernel> laplace5x5;
    Vector2d<int> imageSize = imageStack[0]->getSize();
    AbstractBuffer<double> maxValue(imageSize);
    AbstractBuffer<int> depthMap(imageSize);

    DpImage outputG(imageSize);
    DpImage outputL(imageSize);

    PreciseTimer timer = PreciseTimer::currentTime();

    for (size_t i = 0; i < imageStack.size(); i++) {

        DpImage * input = imageStack[i]->getChannelDp(ImageChannel::GRAY);
        Convolver().convolve(*input, gaussian5x5, outputG);
        Convolver().convolve(outputG, laplace5x5, outputL);

        for (int h = 0; h < imageSize.y(); h++) {
            for (int w = 0; w < imageSize.x(); w++) {
                double value = fabs(outputL.element(h, w));

                if (maxValue.element(h, w) < value) {
                    maxValue.element(h, w) = value;
                    depthMap.element(h, w) = i;
                }
            }
        }
        delete_safe(input);
    }

    for (int h = 0; h < imageSize.y(); h++) {
        for (int w = 0; w < imageSize.x(); w++) {
            result->element(h, w) = imageStack[depthMap.element(h, w)]->element(h, w);
        }
    }

    cout << "Elapsed ms:" << (timer.usecsToNow() / 1000.0) << endl;
}
