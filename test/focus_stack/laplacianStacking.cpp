#include "laplacianStacking.h"
#include "imageStack.h"
#include "core/buffers/kernels/gaussian.h"
#include "core/buffers/kernels/laplace.h"
#include "core/buffers/convolver/convolver.h"
#include <float.h>
#include <core/utils/preciseTimer.h>
#include <core/stats/calculationStats.h>


void LaplacianStacking::doStacking(vector<RGB24Buffer*> & imageStack, RGB24Buffer * result)
{
    Gaussian5x5<FpKernel> gaussian5x5(true);
    Laplace5x5 <FpKernel> laplace5x5;
    Vector2d<int> imageSize = imageStack[0]->getSize();
    AbstractBuffer<float> maxValue(imageSize);
    AbstractBuffer<int> depthMap(imageSize);

    FpImage outputG(imageSize);
    FpImage outputL(imageSize);

    //PreciseTimer timer = PreciseTimer::currentTime();
    Statistics stats;
    stats.startInterval();

    for (size_t i = 0; i < imageStack.size(); i++) {
        stats.enterContext(std::string("Level") + to_string(i) + " ->");
        stats.startInterval();
        FpImage * input = imageStack[i]->getChannelFp(ImageChannel::GRAY);
        stats.resetInterval("Conversion");
        Convolver().convolve(*input, gaussian5x5, outputG);
        Convolver().convolve(outputG, laplace5x5, outputL);
        stats.resetInterval("Convoluion");

        for (int h = 0; h < imageSize.y(); h++) {
            for (int w = 0; w < imageSize.x(); w++) {
                float value = fabs(outputL.element(h, w));

                if (maxValue.element(h, w) < value) {
                    maxValue.element(h, w) = value;
                    depthMap.element(h, w) = i;
                }
            }
        }
        delete_safe(input);
        stats.resetInterval("Comparing");
        stats.leaveContext();
    }

    stats.resetInterval("Computing choice");
    for (int h = 0; h < imageSize.y(); h++) {
        for (int w = 0; w < imageSize.x(); w++) {
            result->element(h, w) = imageStack[depthMap.element(h, w)]->element(h, w);
        }
    }
    stats.endInterval("Merging");

    //cout << "Elapsed ms:" << (timer.usecsToNow() / 1000.0) << endl;
    BaseTimeStatisticsCollector(stats).printAdvanced();
}
