#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#include "openCVCheckerBoardDetector.h"
#include "../openCVTools.h"

using namespace corecvs;
using std::vector;
using std::string;

OpenCVCheckerBoardDetector::OpenCVCheckerBoardDetector()
{

}

OpenCVCheckerBoardDetector::~OpenCVCheckerBoardDetector()
{
    delete_safe(debugBuffer);
}

int OpenCVCheckerBoardDetector::operator ()(){
    SYNC_PRINT(("OpenCVCheckerBoardDetector::operator(): called\n"));

    bool result = false;
    corners.clear();

    Statistics::startInterval(stats);
    cv::Size board_sz = cv::Size(params.width(), params.height());
    Statistics::resetInterval(stats, "Conversion");

    bool found = findChessboardCorners(input, board_sz, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
    Statistics::resetInterval(stats, "Corner Search");

    if(found)
    {
        SYNC_PRINT(("Found a board\n"));
        cv::Mat gray_image;
        cvtColor(input, gray_image, CV_RGB2GRAY);
        cornerSubPix( gray_image, corners, cv::Size(11,11),
                                    cv::Size(-1,-1), cv::TermCriteria( cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 30, 0.1 ));
        cv::drawChessboardCorners(gray_image, board_sz, corners, found);
    } else {
        SYNC_PRINT(("Board not found\n"));
    }

    Statistics::endInterval(stats, "Detection");
    return result;
}

void OpenCVCheckerBoardDetector::setInputImage(corecvs::RGB24Buffer *input) {
    Statistics::startInterval(stats);
    this->input = OpenCVTools::getCVMatFromRGB24Buffer(input);
    Statistics::endInterval(stats, "Setting input");
}

std::vector<std::string> OpenCVCheckerBoardDetector::debugBuffers() const
{
    if (debugBuffer != NULL)
        return std::vector<std::string>({"debug"});

    return std::vector<std::string>();
}

RGB24Buffer *OpenCVCheckerBoardDetector::getDebugBuffer(const std::string &name) const
{
    if (name == "debug")
    {
        return new RGB24Buffer(debugBuffer);
    }
    return NULL;
}

std::map<std::string, DynamicObject> OpenCVCheckerBoardDetector::getParameters()
{
    std::map<std::string, DynamicObject> dparams;
    dparams.emplace("params", DynamicObject(&params));
    return dparams;
}

bool OpenCVCheckerBoardDetector::setParameters(std::string name, const DynamicObject &param)
{
    if (name == "params") {
        param.copyTo(&params);
        return true;
    }
    return false;
}

void OpenCVCheckerBoardDetector::getOutput(vector<PatternDetectorResult> &patterns)
{
    Statistics::startInterval(stats);
    if (corners.size() >= params.width() * (params.height() - 1) + params.width() - 1)
    {
        PatternDetectorResult result;
        cv::Point2f p1 = corners[0];
        cv::Point2f p2 = corners[params.width() - 1];
        cv::Point2f p3 = corners[params.width() * (params.height() - 1) + params.width() - 1];
        cv::Point2f p4 = corners[params.width() * (params.height() - 1)];


        result.mPosition   = Vector2dParameters(p1.x, p1.y);
        result.mOrtX       = Vector2dParameters(p2.x, p2.y);
        result.mOrtY       = Vector2dParameters(p3.x, p3.y);
        result.mUnityPoint = Vector2dParameters(p4.x, p4.y);
        result.mId = 0;
        patterns.push_back(result);
    } else {
        cout << "corner size is" << corners.size() << endl;
    }


    Statistics::endInterval(stats, "Getting output");
}

