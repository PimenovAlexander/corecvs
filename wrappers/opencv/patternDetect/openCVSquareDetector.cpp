#include <opencv2/imgproc.hpp>

#include "openCVSquareDetector.h"
#include "../openCVTools.h"

using namespace corecvs;
using std::vector;
using std::string;

OpenCVSquareDetector::OpenCVSquareDetector()
{

}

OpenCVSquareDetector::~OpenCVSquareDetector()
{
    delete_safe(debugBuffer);
}

/**
 * WTF? You ment
 *
 * double angle( const Vector2dd &a, const Vector2dd &b, const Vector2dd &c ) {
 *    Vector2dd v1 = (b - a);
 *    Vector2dd v2 = (c - a);
 *    return (v1 & v2) / (v1.l2Metric() * v2.l2Metric() + 1e-10);
 * }
 **/

double angle( cv::Point pt1, cv::Point pt2, cv::Point pt0 ) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

int OpenCVSquareDetector::operator ()(){
    SYNC_PRINT(("OpenCVSquareDetector::operator(): called\n"));

    bool result = false;
    squares.clear();

    Statistics::startInterval(stats);
    // blur will enhance edge detection
    cv::Mat timg(input);
    medianBlur(input, timg, 3);
    Statistics::resetInterval(stats, "blur");

    cv::Mat gray0(timg.size(), CV_8U);
    cv::Mat gray;

    vector<vector<cv::Point> > contours;

    // find squares in every color plane of the image
    // Do we need it?
    for( int c = 0; c < 3; c++ )
    {
        const char *channels[3] = {"R", "G", "B"};
        Statistics::enterContext(stats, std::string("channel") + channels[c] + "-> ");
        Statistics::startInterval(stats);

        int ch[] = {c, 0};
        mixChannels(&timg, 1, &gray0, 1, ch, 1);
        Statistics::resetInterval(stats, "Mixing channels");

        const int treshholdLevel = params.treshholdLevel();
        // try several threshold levels
        for( int l = 0; l < treshholdLevel; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                Canny(gray0, gray, params.cannyThreshold1(), params.cannyThreshold2(), params.cannyApperture());
                // dilate canny output to remove potential
                // holes between edge segments
                dilate(gray, gray, cv::Mat(), cv::Point(-1,-1));
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l+1)*255/treshholdLevel;
            }

            // find contours and store them all as a list
            findContours(gray, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

            vector<cv::Point> approx;

            // test each contour
            for( size_t i = 0; i < contours.size(); i++ )
            {
                 // approximate contour with accuracy proportional
                 // to the contour perimeter
                 approxPolyDP(cv::Mat(contours[i]), approx, arcLength(cv::Mat(contours[i]), true)*0.02, true);

                 // square contours should have 4 vertices after approximation
                 // relatively large area (to filter out noisy contours)
                 // and be convex.
                 // Note: absolute value of an area is used because
                 // area may be positive or negative - in accordance with the
                 // contour orientation
                 if( approx.size() == 4 &&
                      fabs(contourArea(cv::Mat(approx))) > 1000 &&
                      isContourConvex(cv::Mat(approx)) )
                 {
                     double maxCosine = 0;

                     for( int j = 2; j < 5; j++ )
                     {
                         // find the maximum cosine of the angle between joint edges
                         double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                         maxCosine = MAX(maxCosine, cosine);
                     }

                     // if cosines of all angles are small
                     // (all angles are ~90 degree) then write quandrange
                     // vertices to resultant sequence
                     if( maxCosine < params.maxCosineThreshold() )
                         squares.push_back(approx);
                 }
             }
        }
        Statistics::endInterval(stats, "Detecting squares");
        Statistics::leaveContext(stats);
    }

    Statistics::startInterval(stats);


    Statistics::endInterval(stats, "Finalize");
    return result;
}

void OpenCVSquareDetector::setInputImage(corecvs::RGB24Buffer *input) {
    Statistics::startInterval(stats);
    this->input = OpenCVTools::getCVMatFromRGB24Buffer(input);
    Statistics::endInterval(stats, "Setting input");
}

std::vector<std::string> OpenCVSquareDetector::debugBuffers() const
{
    if (debugBuffer != NULL)
        return std::vector<std::string>({"debug"});

    return std::vector<std::string>();
}

RGB24Buffer *OpenCVSquareDetector::getDebugBuffer(const std::string &name) const
{
    if (name == "debug")
    {
        return new RGB24Buffer(debugBuffer);
    }
    return NULL;
}

std::map<std::string, DynamicObject> OpenCVSquareDetector::getParameters()
{
    std::map<std::string, DynamicObject> dparams;
    dparams.emplace("params", DynamicObject(&params));
    return dparams;
}

bool OpenCVSquareDetector::setParameters(std::string name, const DynamicObject &param)
{
    if (name == "params") {
        param.copyTo(&params);
        return true;
    }
    return false;
}

void OpenCVSquareDetector::getOutput(vector<Vector2dd> &patterns)
{
    Statistics::startInterval(stats);
    for (int i = 0; i < squares.size(); i++)
    {
        for (int j = 0; j < squares[i].size(); j++)
        {
            cv::Point p = squares[i][j];
            patterns.push_back(Vector2dd(p.x, p.y));
        }
    }

    if (params.debug())
    {
        delete_safe(debugBuffer);
        debugBuffer = OpenCVTools::getRGB24BufferFromCVMat(input);
        for (int i = 0; i < squares.size(); i++)
        {
            Vector2dd a(squares[i][0].x, squares[i][0].y);
            Vector2dd b(squares[i][1].x, squares[i][1].y);
            Vector2dd c(squares[i][2].x, squares[i][2].y);
            Vector2dd d(squares[i][3].x, squares[i][3].y);

            RGBColor color = RGBColor::parula((double)i / (squares.size()));
            debugBuffer->drawLine(a, b, color);
            debugBuffer->drawLine(b, c, color);
            debugBuffer->drawLine(c, d, color);
            debugBuffer->drawLine(d, a, color);
        }
    }

    Statistics::endInterval(stats, "Getting output");
}

