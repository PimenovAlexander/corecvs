/**
 * \file KLTFlow.cpp
 * \brief Add Comment Here
 *
 * \date Feb 23, 2011
 * \author alexander
 */

/* GCC 4.6 needs this to compile*/
#include <cstddef>

#include <opencv2/core/core_c.h>        // cvCreateImage
#include <opencv2/imgproc/imgproc_c.h>  // cvGoodFeaturesToTrack
#ifdef WITH_OPENCV_3x
    #include <opencv2/video/tracking_c.h>   // cvCalcOpticalFlowPyrLK
#else
    #include <opencv2/video/tracking.hpp>   // cvCalcOpticalFlowPyrLK
#endif


#include "core/math/vector/vector2d.h"
#include "KLTFlow.h"
#include "core/math/mathUtils.h"
#include "OpenCVTools.h"

using namespace corecvs;

KLTFlow::KLTFlow()
{
    // TODO Auto-generated constructor stub

}

KLTFlow::~KLTFlow()
{
    // TODO Auto-generated destructor stub
}


#define MAX_CORNERS 4000

/**
 *   This function wraps  OpenCV call
 *
 **/
std::vector<FloatFlowVector> *KLTFlow::getOpenCVKLT(
        G12Buffer *first
      , G12Buffer *second
      , double mSelectorQuality
      , double mSelectorDistance
      , int    mSelectorSize
      , int    mUseHarris
      , double mHarrisK
      , int    mKLTSize)
{
    std::vector<FloatFlowVector> *result = new std::vector<FloatFlowVector>();

    CORE_ASSERT_TRUE_P(first->hasSameSize(second), ("Inputs to getOpenCVKLT must have same size\n"));

    Vector2d<int32_t> size = first->getSize();
    CvSize sizeCV  = cvSize(size.x(), size.y());

    IplImage *algo_image_A_p = OpenCVTools::getCVImageFromG12Buffer(first);
    IplImage *algo_image_B_p = OpenCVTools::getCVImageFromG12Buffer(second);


    /* If only we new size form the start we could cache this*/
    /*Temporary structures*/
    IplImage *eig_image       = cvCreateImage( sizeCV, IPL_DEPTH_32F, 1);
    IplImage *temp_image      = cvCreateImage( sizeCV, IPL_DEPTH_32F, 1);

    /*Search piramid structures */
    CvSize pyr_sz   = cvSize( sizeCV.width+8, sizeCV.height/3 );
    IplImage *pyrA            = cvCreateImage( pyr_sz,  IPL_DEPTH_32F, 1 );
    IplImage *pyrB            = cvCreateImage( pyr_sz,  IPL_DEPTH_32F, 1 );


    /* Arrays that will hold features */
    /* Make C++ style*/
    CvPoint2D32f* featuresA = new CvPoint2D32f[MAX_CORNERS];
    CvPoint2D32f* featuresB = new CvPoint2D32f[MAX_CORNERS];
    float *feature_errors = new float[MAX_CORNERS];
    char  *features_found = new char[MAX_CORNERS];

    int corner_count = MAX_CORNERS;

    cvGoodFeaturesToTrack(
                algo_image_A_p, /**< */
                eig_image,      /**< */
                temp_image,     /**< */
                featuresA,
                &corner_count,
                mSelectorQuality,
                mSelectorDistance,
                NULL,
                mSelectorSize,
                mUseHarris,
                mHarrisK);

    cvFindCornerSubPix(
                algo_image_A_p,
                featuresA,
                corner_count,
                cvSize( mSelectorSize, mSelectorSize ),
                cvSize( -1, -1 ),
                cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03 ) );

    // Call Lucas Kanade algorithm
    cvCalcOpticalFlowPyrLK(
                algo_image_A_p,
                algo_image_B_p,
                pyrA,
                pyrB,
                featuresA,
                featuresB,
                corner_count,
                cvSize( mKLTSize, mKLTSize ),
                3,
                features_found,
                feature_errors,
                cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.3 ), 0 );

    for (int i = 0; i < corner_count; i++)
    {
        if (!features_found[i])
            continue;

        Vector2dd start = Vector2dd(featuresA[i].x, featuresA[i].y);
        Vector2dd end   = Vector2dd(featuresB[i].x, featuresB[i].y);

        result->push_back(FloatFlowVector(start, end));
    }

    cvReleaseImage(&algo_image_A_p);
    cvReleaseImage(&algo_image_B_p);
    cvReleaseImage(&eig_image);
    cvReleaseImage(&temp_image);
    cvReleaseImage(&pyrA);
    cvReleaseImage(&pyrB);


     /* Arrays that will hold features */
     /* Make C++ style*/
     delete[] featuresA;
     delete[] featuresB;
     delete[] feature_errors;
     delete[] features_found;

    return result;
}
