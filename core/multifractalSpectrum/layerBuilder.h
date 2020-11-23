#include <vector>
#include <cmath>
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "leastSquares.h"
#include "structures.h"
#include "cfloat"

#ifndef CORECVS_LAYERBUILDER_H
#define CORECVS_LAYERBUILDER_H
namespace corecvs {

    class Layer {
    public:
        const vector<Point> &getPoints() const;

        Interval getSingularityBounds();

        Layer(std::vector<Point> points, Interval singularityBounds);

    private:
        std::vector<Point> points;
        Interval singularityBounds;
    };

    class LayersBuilder {
    public:
        LayersBuilder(RGB24Buffer *image, ImageChannel::ImageChannel channel, std::vector<int> windows);
        Interval getSingularityBounds();
        std::vector<Layer> splitByLayers(Interval singularityBounds, double singularityStep);

    private:
        vector<int> windows;
        const int maxWindowSize = 7;
        G16Buffer converterImage;
        int height;
        int width;
        std::vector<std::vector<double>> densities;
        std::vector<std::vector<double>> intensities;
        double maxDensity = -DBL_MIN;
        double minDensity = -DBL_MAX;

        void calculateDensity();
        double calculateDensityInPoint(Point *point);
        double calculateIntensity(Point *point, int windowSize);



    };

}

#endif //CORECVS_LAYERBUILDER_H