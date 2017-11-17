#include "core/patterndetection/patternDetector.h"

using namespace corecvs;

bool PatternDetector::detectPattern(RGB24Buffer &rgbBuffer)
{
    G8Buffer grayscale(rgbBuffer.h, rgbBuffer.w);
    grayscale.binaryOperationInPlace(rgbBuffer, [](const unsigned char & /*a*/, const RGBColor &b) { return b.y(); });
    return detectPattern(grayscale);
}

void PatternDetector::getPointData(SelectableGeometryFeatures &features)
{
    corecvs::ObservationList list;
    getPointData(list);
    features.addAllLinesFromObservationList(list);
}


size_t PatternDetector::detectPatterns(RGB24Buffer &/*buffer*/, std::vector<ObservationList> &patterns)
{
    patterns.clear();
    return 0;
}
