#pragma once
/**
 * \file featureDetectorCV.h
 * \brief Add Comment Here
 *
 * \ingroup OpenCV
 * \date Nov 26, 2012
 */

#ifndef WITH_OPENCV_3X

#include "core/buffers/g12Buffer.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/flow/flowBuffer.h"
#include "core/math/vector/vector2d.h"
#include "core/buffers/correspondenceList.h"
#include "openCVTools.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/legacy/legacy.hpp> // BruteForceMatcher

#ifdef WITH_OPENCV_CONTRIB
#if ((CV_MAJOR_VERSION * 10 + CV_MINOR_VERSION) * 10 + CV_SUBMINOR_VERSION) > 220
# include <opencv2/nonfree/features2d.hpp>    /* for new openCV */
#else
# include <opencv2/features2d/features2d.hpp> /* for old openCV */
# define CV_OLD
#endif
#else
# include <opencv2/features2d/features2d.hpp> /* for old openCV */
# define CV_OLD
#endif

using namespace cv;
using namespace corecvs;

template <class DetectorType> /* Detector type may be SIFT or SURF */
class FeatureDetectorCV
{
public:
    // default constructor
    FeatureDetectorCV() :
        mDetector(),
        mDescriptor()
    {}

    // constructor for SURF
    FeatureDetectorCV(
       double  hessianThreshold   // = 400.
     , int     octaves            // = 3
     , int     octaveLayers       // = 4
     , bool    extended           // = false
    ) : mDetector(
            hessianThreshold
          , octaves
          , octaveLayers
    )
    , mDescriptor(
            hessianThreshold
          , octaves
          , octaveLayers
          , extended
    )
    {}

    // constructor for SIFT
    FeatureDetectorCV( int    features           // = 0
                     , int    octaveLayers       // = 3
                     , double contrastThreshold  // = 0.04
                     , double edgeThreshold      // = 10
                     , double sigma              // = 1.6
    ) : mDetector(
            features
          , octaveLayers
          , contrastThreshold
          , edgeThreshold
          , sigma
    )
      , mDescriptor(
           features
         , octaveLayers
         , contrastThreshold
         , edgeThreshold
         , sigma
    )
    {}

    void calculate( G12Buffer *image
                  , G12Buffer *pattern
    )
    {
        // convert images to IplImage type
        IplImage* iplImage   = OpenCVTools::getCVImageFromG12Buffer(image);
        IplImage* iplPattern = OpenCVTools::getCVImageFromG12Buffer(pattern);

        process(iplImage, iplPattern);
    }

    void calculate( RGB24Buffer *image
                  , RGB24Buffer *pattern
    )
    {
        // convert images to IplImage type
        IplImage* iplImage   = OpenCVTools::getCVImageFromRGB24Buffer(image);
        IplImage* iplPattern = OpenCVTools::getCVImageFromRGB24Buffer(pattern);

        process(iplImage, iplPattern);
    }

    CorrespondenceList getCorrespondenceList()
    {
        return mCorList;
    }

    vector<cv::KeyPoint> getKeypointsImage()
    {
        return mKeypointsImage;
    }

    vector<cv::KeyPoint> getKeypointsPattern()
    {
        return mKeypointsPattern;
    }

private:
    void process(IplImage* image, IplImage* pattern)
    {
        Mat matImage(image, false);
        Mat matPattern(pattern, false);
#ifdef CV_OLD
#pragma warning("Please implement detect() for old OpenCV version to support FeatureDetectorCV!")
#else
        //-- Step 1: Detect the keypoints using SURF Detector
        mDetector.detect( matPattern, mKeypointsPattern );
        mDetector.detect( matImage,   mKeypointsImage );

        //-- Step 2: Calculate descriptors (feature vectors)
        mDescriptor.compute( matPattern, mKeypointsPattern, mDescriptorsPattern );
        mDescriptor.compute( matImage,   mKeypointsImage,   mDescriptorsImage );
#endif
        //-- Step 3: Matching descriptor vectors using FLANN matcher
        mMatcher.match( mDescriptorsPattern, mDescriptorsImage, mMatches );

        // fill correspondences
        Correspondence tmpCorr;
        for (uint i = 0; i < mMatches.size(); i++)
        {
            tmpCorr.end   = Vector2dd(mKeypointsImage  [ mMatches[i].trainIdx ].pt.x, mKeypointsImage  [ mMatches[i].trainIdx ].pt.y);
            tmpCorr.start = Vector2dd(mKeypointsPattern[ mMatches[i].queryIdx ].pt.x, mKeypointsPattern[ mMatches[i].queryIdx ].pt.y);
            mCorList.push_back(tmpCorr);
        }
    }

    CorrespondenceList              mCorList; // output of algorithm

    DetectorType                    mDetector;
    vector<cv::KeyPoint>            mKeypointsImage, mKeypointsPattern;

    DetectorType                    mDescriptor;
    Mat                             mDescriptorsImage;
    Mat                             mDescriptorsPattern;

    BruteForceMatcher< L2<float> >  mMatcher;
    vector<DMatch>                  mMatches;
};

#endif

/* EOF */
