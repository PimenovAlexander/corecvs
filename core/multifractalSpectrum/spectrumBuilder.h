#include <map>
#include <cmath>
#include "core/fileformats/bmpLoader.h"
#include "layerBuilder.h"
#include "filesystem"

#ifndef CORECVS_SPECTRUMBUILDER_H
#define CORECVS_SPECTRUMBUILDER_H

namespace corecvs {
    class SpectrumBuilder{
    public:
        SpectrumBuilder(vector<int>, const string&);
        std::map<double, double> calculateSpectrum(
                RGB24Buffer *image,
                const std::vector<Layer>& layers,
                Interval singularityBounds,
                double singularityStep);

    private:
        const int maxWindowSize = 7;
        string layersDirectoryPath;
        vector<int> windows;
        double createAndMeasureLayer(RGB24Buffer *image, const Layer& layer);

        double calculateMeasure(RGB24Buffer *image);

        void saveLayerImage(Layer layer, RGB24Buffer *layerImage);

        double calculateBlackWindows(RGB24Buffer *image, int window);

        bool hasBlackPixel(RGB24Buffer *image, int start_x, int start_y, int window);
    };
}

#endif //CORECVS_SPECTRUMBUILDER_H
