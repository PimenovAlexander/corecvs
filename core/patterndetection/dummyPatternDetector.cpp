#include "dummyPatternDetector.h"

namespace corecvs {

DummyPatternDetector::DummyPatternDetector()
{
    //dummyResult.push_back(PointObservation(Vector3dd::Zero(), Vector2dd(-1, -1)));
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
            PatternDetectorResult result;
            result.push_back(PointObservation(Vector3dd(0,0,0), Vector2dd(dummyResult.position())  ));
            result.push_back(PointObservation(Vector3dd(1,0,0), Vector2dd(dummyResult.ortX())      ));
            result.push_back(PointObservation(Vector3dd(0,1,0), Vector2dd(dummyResult.ortY())      ));
            result.push_back(PointObservation(Vector3dd(1,1,0), Vector2dd(dummyResult.unityPoint())));
            return;
        }

        PatternDetectorResult result;
        result.resize(1);
        result[0] = PointObservation(Vector3dd::Zero(), Vector2dd(0, 0));

        double minDist = numeric_limits<int>::max();
        for (int i = 0; i < input->h; i++)
        {
            for (int j = 0; j < input->w; j++)
            {
                int dist = RGBColor::diff(input->element(i,j), color).y();
                if (dist < minDist)
                {
                    minDist = dist;
                    result[0] = PointObservation(Vector3dd::Zero(), Vector2dd(j, i));
                }
            }
        }

        patterns.push_back(result);
        delete_safe(debug);
        debug = new RGB24Buffer(input);
        for (size_t i = 0; i < patterns.size(); i++)
        {
            if (!patterns[i].empty())
                debug->drawCrosshare3(patterns[i][0].projection, RGBColor::Red());
        }
    }    

    Statistics::endInterval(stats, "Getting output");
}

} // namespace corecvs
