#include "harrisPatternDetector.h"

namespace corecvs {

HarrisPatternDetector::HarrisPatternDetector()
{

}


std::vector<std::string> HarrisPatternDetector::debugBuffers() const
{
    std::vector<std::string> toReturn;

    if (highlights != NULL)
        toReturn.push_back("highlights");

    if (gray != NULL)
        toReturn.push_back("gray");


    return toReturn;
}

RGB24Buffer *HarrisPatternDetector::getDebugBuffer(const std::string &name) const
{
    if (name == "highlights")
    {
        if (highlights != NULL) {
            return new RGB24Buffer(highlights);
        } else {
            SYNC_PRINT(("Requested highlights debug buffer, but it is NULL\n"));
        }
    }
    if (name == "gray")
    {
        if (highlights != NULL) {
            return new RGB24Buffer(gray);
        } else {
            SYNC_PRINT(("Requested gray debug buffer, but it is NULL\n"));
        }
    }
    return NULL;
}

int HarrisPatternDetector::operator ()()
{
    Statistics::startInterval(stats);
    gray = input->getChannelG12(ImageChannel::GRAY);
    Statistics::resetInterval(stats, "Convert to grayscale");
    SpatialGradientF gradient(gray);
    Statistics::resetInterval(stats, "Compute spatial gradient");
    highlights = gradient.findCornerPoints(params.scaler(), params.apperture());
    Statistics::endInterval(stats, "Finding corners");
    return 0;
}

std::map<std::string, DynamicObject> HarrisPatternDetector::getParameters()
{
    std::map<std::string, DynamicObject> params;
    params.emplace("params", DynamicObject(&this->params));
    return params;
}

bool HarrisPatternDetector::setParameters(std::string name, const DynamicObject &param)
{
    if (name == "params") {
        param.copyTo(&this->params);
        return true;
    }
    return false;
}

void HarrisPatternDetector::setInputImage(RGB24Buffer *input)
{
    Statistics::startInterval(stats);
    this->input = input;
    Statistics::endInterval(stats, "Setting input");
}

void HarrisPatternDetector::getOutput(vector<PatternDetectorResult> &patterns)
{
    return;
}

void HarrisPatternDetector::setStatistics(Statistics *stats)
{
    this->stats = stats;
}

} // namespace corecvs
