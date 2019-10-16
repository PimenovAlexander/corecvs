#include "dummyPatternDetector.h"

namespace corecvs {

DummyPatternDetector::DummyPatternDetector()
{
    dummyResult.setPosition(Vector2dParameters(-1, -1));
}

DummyPatternDetector::~DummyPatternDetector()
{
    delete_safe(debug);
}


std::vector<std::string> DummyPatternDetector::debugBuffers() const
{
    if (debug == NULL)
        return std::vector<std::string>();

    return std::vector<std::string>({"marks"});
}

RGB24Buffer *DummyPatternDetector::getDebugBuffer(const std::string &name) const
{
    if (name == "marks")
    {
        if (debug != NULL) {
            return new RGB24Buffer(debug);
        } else {
            SYNC_PRINT(("Requested marks debug buffer, but it is NULL\n"));
        }
    }
    return NULL;
}

std::map<std::string, DynamicObject> DummyPatternDetector::getParameters()
{
    std::map<std::string, DynamicObject> params;
    params.emplace("point", DynamicObject(&dummyResult));
    params.emplace("color", DynamicObject(&color));
    return params;
}

bool DummyPatternDetector::setParameters(std::string name, const DynamicObject &param)
{
    if (name == "point") {
        param.copyTo(&dummyResult);
        return true;
    }
    if (name == "color") {
        param.copyTo(&color);
        return true;
    }
    return false;
}

void DummyPatternDetector::getOutput(vector<PatternDetectorResult> &patterns)
{
    Statistics::startInterval(stats);
    if (input != NULL) {
        if (input->isValidCoordBl(Vector2dd(dummyResult.position())))
        {
            patterns.push_back(dummyResult);
            return;
        }

        PatternDetectorResult result;
        result.setPosition(Vector2dParameters(0, 0));

        double minDist = numeric_limits<int>::max();
        for (int i = 0; i < input->h; i++)
        {
            for (int j = 0; j < input->w; j++)
            {
                int dist = RGBColor::diff(input->element(i,j), color).y();
                if (dist < minDist)
                {
                    minDist = dist;
                    result.setPosition(Vector2dParameters(j, i));
                }
            }
        }

        patterns.push_back(result);
        delete_safe(debug);
        debug = new RGB24Buffer(input);
        for (size_t i = 0; i < patterns.size(); i++)
        {
            debug->drawCrosshare3(Vector2dd(patterns[i].position()), RGBColor::Red());
        }
    }    

    Statistics::endInterval(stats, "Getting output");
}

} // namespace corecvs
