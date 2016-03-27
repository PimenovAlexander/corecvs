#include "openCvCheckerboardDetector.h"
#include "selectableGeometryFeatures.h"  // ObservationList
#include "checkerboardDetectionParameters.h"
#include "preciseTimer.h"
/// OpenCV wrapper
#include "OpenCVTools.h"

#include <opencv2/imgproc/imgproc.hpp>  // cornerSubPix
#include <opencv2/highgui/highgui.hpp>  // imwrite
#include <opencv2/calib3d/calib3d.hpp>  // findChessboardCorners

#if defined(_MSC_VER)
#pragma warning (disable:4482)
#endif

OpenCvCheckerboardDetector::OpenCvCheckerboardDetector(const CheckerboardDetectionParameters &params, BoardAlignerParams boardAlignerParams)
    : CheckerboardDetectionParameters(params), BoardAligner(boardAlignerParams)
{
}

bool OpenCvCheckerboardDetector::detectPattern(corecvs::G8Buffer &buffer)
{
    result.clear();
    points.clear();

    if (!detectChessBoardOpenCv(buffer))
        return false;

    assignPointCoordinates(buffer);
    return true;
}

bool OpenCvCheckerboardDetector::detectChessBoardOpenCv(corecvs::G8Buffer &buffer)
{
    IplImage *iplImage = OpenCVTools::getCVImageFromG8Buffer(&buffer);
    cv::Mat view = cv::Mat(iplImage);

    // TODO: check if we can add full support of aligner (seems to be impossible)
    int hbegin = mPartialBoard ? 3 : idealHeight, hend = idealHeight;
    int wbegin = idealWidth, wend = idealWidth;

    bool found = false;
    for (int h = hend; h >= hbegin && !found; --h)
    {
        for (int w = wend; w >= wbegin && !found; --w)
        {
            found = cv::findChessboardCorners(view, cv::Size(w, h), points, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK);
            if (found)
            {
                bw = w; bh = h;
                cv::cornerSubPix(view, points, cv::Size(mPreciseDiameter, mPreciseDiameter), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, mIterationCount, mMinAccuracy));
            }
        }
    }
    cvReleaseImage(&iplImage);
    return found;
}

void OpenCvCheckerboardDetector::assignPointCoordinates(corecvs::G8Buffer &buffer)
{
    int id = 0;
    bestBoard.resize(bh);
    for (int i = 0; i < bh; ++i)
    {
        bestBoard[i].resize(bw);
        for (int j = 0; j < bw; ++j)
        {
            bestBoard[i][j] = corecvs::Vector2dd(points[id].x, points[id].y);
            id++;
        }
    }

    DpImage dpImage(buffer.h, buffer.w);
    dpImage.binaryOperationInPlace(buffer, [](const double & /*a*/, const unsigned char &b) {
        return ((double)b) / 255.0;
    });

    align(dpImage);
    result = observationList;
}


void OpenCvCheckerboardDetector::getPointData(corecvs::ObservationList &observations)
{
    observations = result;
}

