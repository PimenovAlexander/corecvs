#include "core/patterndetection/patternDetector.h"

namespace corecvs {


bool PatternGeometryDetector::detectPattern(RGB24Buffer &rgbBuffer)
{
    G8Buffer grayscale(rgbBuffer.h, rgbBuffer.w);
    grayscale.binaryOperationInPlace(rgbBuffer, [](const unsigned char & /*a*/, const RGBColor &b) { return b.y(); });
    return detectPattern(grayscale);
}

void PatternGeometryDetector::getPointData(SelectableGeometryFeatures &features)
{
    corecvs::ObservationList list;
    getPointData(list);
    features.addAllLinesFromObservationList(list);
}


size_t PatternGeometryDetector::detectPatterns(RGB24Buffer &/*buffer*/, std::vector<ObservationList> &patterns)
{
    patterns.clear();
    return 0;
}

PatternDetectorFabric *PatternDetectorFabric::getInstance()
{
    static PatternDetectorFabric instance;
    return &instance;
}

} // namespace corecvs
