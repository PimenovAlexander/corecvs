//
// Created by gleb on 19.11.2020.
//
#include <iostream>
#include "gtest/gtest.h"

#include "core/multifractalSpectrum/layerBuilder.h"
#include "core/multifractalSpectrum/spectrumBuilder.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "../../core/fileformats/bmpLoader.h"
#include "core/buffers/bufferFactory.h"

using namespace corecvs;

TEST(MULTIFRACTAL_SPECTRUM, LAYERS_BULDER){
    auto image = BMPLoader().loadRGB("2.bmp");
    const double singularityStep = 0.2;
    const std::vector<int> windows = {3, 4, 5, 6, 7, 8};
    LayersBuilder l(image, static_cast<ImageChannel::ImageChannel>(10), windows);

    Interval singularityBounds = l.getSingularityBounds();

    auto layers = l.splitByLayers(singularityBounds, singularityStep);
    SpectrumBuilder spectrum(windows, "");

    std::map<double, double> res = spectrum.calculateSpectrum(image, layers, singularityBounds, singularityStep);

    delete_safe(image);
    ASSERT_TRUE(true);
}
