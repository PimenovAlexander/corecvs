#include "semiGlobalBlockMatching.h"
#include "openCVTools.h"

#include <opencv2/core/core_c.h> // cvReleaseImage

using namespace cv;

/*
FlowBuffer* BMOpenCV::getStereoBM(G12Buffer* InputBufferLeft, G12Buffer* InputBufferRight, const OpenCVBMParameters &params)
{
    int32_t h = InputBufferLeft->getH();
    int32_t w = InputBufferLeft->getW();

    FlowBuffer* stereo = new FlowBuffer(h,w,true);

    int dSearch = params.disparitySearch() & -16;
    int bSize   = (params.blockSize() & -2) + 1;
    if(bSize < 5 ) bSize = 5;

    bm = StereoBM::create( dSearch, bSize );

    bm->setPreFilterCap     ( params.preFilterCap() );
    bm->setMinDisparity     ( params.minDisparity() );
    bm->setTextureThreshold ( params.textureThreshold() );
    bm->setUniquenessRatio  ( params.uniquenessRatio() );
    bm->setSpeckleWindowSize( params.speckleWindowSize() );
    bm->setSpeckleRange     ( params.speckleRange() );
    bm->setDisp12MaxDiff    ( params.disp12MaxDiff() );

    IplImage* leftIpl  = OpenCVTools::getCVImageFromG12Buffer(InputBufferLeft);
    IplImage* rightIpl = OpenCVTools::getCVImageFromG12Buffer(InputBufferRight);

    CVMAT_FROM_IPLIMAGE( imgLeft, leftIpl, false );
    CVMAT_FROM_IPLIMAGE( imgRight, rightIpl, false );

    Mat disp, disp8;
    bm->compute( imgLeft, imgRight, disp );

    disp.convertTo(disp8, CV_8U, 1/16.);

    for(int i = 0; i < h; i++)
    {
        for(int j = 0; j < w; j++)
        {
            int tmp = j-(int16_t)disp8.at<uint8_t>(i, j);
            if(tmp < w && tmp > 0)
            {
                stereo->element(i,tmp).x() = (int16_t)disp8.at<uint8_t>(i, j);//*255/(bm.state->numberOfDisparities);
            }
            stereo->element(i,j).y() = 0;
        }
    }

    cvReleaseImage(&leftIpl);
    cvReleaseImage(&rightIpl);

    return stereo;
}

FlowBuffer* SGBMOpenCV::getStereoSGBM(G12Buffer* InputBufferLeft, G12Buffer* InputBufferRight, const OpenCVSGMParameters &params)
{
    int32_t h = InputBufferLeft->getH();
    int32_t w = InputBufferLeft->getW();

    FlowBuffer* stereo = new FlowBuffer(h,w,true);

    const int sADWindowSize = params.sADWindowSize();

    sgbm = StereoSGBM::create( params.minDisparity(),
        */
/* num disparities *//*
 ( ( w / 5 ) + 15 ) & -16,
        sADWindowSize,
        params.p1Multiplier() * sADWindowSize * sADWindowSize,
        params.p2Multiplier() * sADWindowSize * sADWindowSize,
        params.disp12MaxDiff(),
        params.preFilterCap(),
        params.uniquenessRatio(),
        params.speckleWindowSize(),
        params.speckleRange(),
        params.fullDP() ? StereoSGBM::MODE_SGBM : StereoSGBM::MODE_HH );


#define DIRECT_CONVERSION
#ifndef DIRECT_CONVERSION
    G8Buffer* InputBufferLeft8  = new G8Buffer(h, w, false);
    G8Buffer* InputBufferRight8 = new G8Buffer(h, w, false);

    for(int i = 0; i < h; i++)
        for(int j = 0; j < w; j++)
        {
            InputBufferLeft8->element(i,j)  = (uint8_t)(InputBufferLeft->element(i,j) / 16.0);
            InputBufferRight8->element(i,j) = (uint8_t)(InputBufferRight->element(i,j) / 16.0);
        }

    IplImage* leftIpl  = OpenCVTools::getCVImageFromG8Buffer(InputBufferLeft8);
    IplImage* rightIpl = OpenCVTools::getCVImageFromG8Buffer(InputBufferRight8);
#else
    IplImage* leftIpl  = OpenCVTools::getCVImageFromG12Buffer(InputBufferLeft);
    IplImage* rightIpl = OpenCVTools::getCVImageFromG12Buffer(InputBufferRight);
#endif

    CVMAT_FROM_IPLIMAGE( imgLeft, leftIpl, false );
    CVMAT_FROM_IPLIMAGE( imgRight, rightIpl, false );

    Mat disp, disp8;

    sgbm->compute( imgLeft, imgRight, disp );

    disp.convertTo(disp8, CV_8U,1/16.);

    for(int i = 0; i < h; i++)
        for(int j = 0; j < w; j++)
        {
            int tmp = j-(int16_t)disp8.at<uint8_t>(i, j);
            if(tmp < w && tmp > 0)
            {
                stereo->element(i,tmp).x() = (int16_t)disp8.at<uint8_t>(i, j);//*255/(sgbm.numberOfDisparities);
            }
            stereo->element(i,j).y() = 0;
        }

    cvReleaseImage(&leftIpl);
    cvReleaseImage(&rightIpl);
#ifndef DIRECT_CONVERSION
    delete InputBufferLeft8;
    delete InputBufferRight8;
#endif
    return stereo;
}
*/
