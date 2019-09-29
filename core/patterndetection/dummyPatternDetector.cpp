#include "dummyPatternDetector.h"

namespace corecvs {

DummyPatternDetector::DummyPatternDetector() :
    point(-1, -1)
{
}

DummyPatternDetector::~DummyPatternDetector()
{
    delete_safe(debug);
}


std::vector<std::string> DummyPatternDetector::debugBuffers() const
{
    return std::vector<std::string>({"marks"});
}

RGB24Buffer *DummyPatternDetector::getDebugBuffer(const std::string &name) const
{
    if (name == "marks")
    {
        return new RGB24Buffer(debug);
    }
    return NULL;
}

std::map<std::string, DynamicObject> DummyPatternDetector::getParameters()
{
    std::map<std::string, DynamicObject> params;
    params.emplace("point", DynamicObject(&point));
    params.emplace("color", DynamicObject(&color));
    return params;
}

bool DummyPatternDetector::setParameters(std::string name, const DynamicObject &param)
{
    if (name == "point") {
        param.copyTo(&point);
        return true;
    }
    if (name == "color") {
        param.copyTo(&color);
        return true;
    }
    return false;
}

void DummyPatternDetector::getOutput(vector<Vector2dd> &patterns)
{
    Statistics::startInterval(stats);
    if (input != NULL) {
        if (input->isValidCoordBl(point))
        {
            patterns.push_back(point);
            return;
        }

        Vector2dd result = Vector2dd::Zero();
        double minDist = numeric_limits<int>::max();
        for (int i = 0; i < input->h; i++)
        {
            for (int j = 0; j < input->w; j++)
            {
                int dist = RGBColor::diff(input->element(i,j), color).y();
                if (dist < minDist)
                {
                    minDist = dist;
                    result = Vector2dd(j, i);
                }
            }
        }
        patterns.push_back(result);
        delete_safe(debug);
        debug = new RGB24Buffer(input);
        for (size_t i = 0; i < patterns.size(); i++)
        {
            debug->drawCrosshare3(patterns[i], RGBColor::Red());
        }
    }    

    Statistics::endInterval(stats, "Getting output");
}

} // namespace corecvs
