/**
 * \file OpenCVTools.cpp
 * \brief Add Comment Here
 *
 * \date Apr 6, 2011
 * \author alexander
 */
#include <opencv/cv.h>


#include "OpenCVTools.h"

using namespace corecvs;

IplImage *OpenCVTools::getCVImageFromRGB24Buffer(RGB24Buffer *input)
{
    CvSize size  = getCvSizeFromVector(input->getSize());

    IplImage *result = cvCreateImage(size, IPL_DEPTH_8U, 3);

    for (int i = 0; i < input->h; i++)
    {
        RGBColor *srcData = &(input->element(i,0));
        uint8_t  *dstData = (uint8_t *)&result->imageData[i * result->widthStep];

        /*TODO: Add SSE implementation*/
        for (int j = 0; j < input->w; j++)
        {
            *dstData = srcData->b();
            *(dstData + 1) = srcData->g();
            *(dstData + 2) = srcData->r();
            dstData += 3;
            srcData++;
        }
    }
    return result;
}

IplImage *OpenCVTools::getCVImageFromG12Buffer(G12Buffer *input)
{
    CvSize size  = getCvSizeFromVector(input->getSize());

    IplImage *result = cvCreateImage(size, IPL_DEPTH_8U, 1);
    for (int i = 0; i < size.height; i++)
    {
        uint8_t *dstLineStart = (uint8_t *)&result->imageData[i * result->widthStep];
        G12Buffer::InternalElementType *srcLineStart =  &(input->element(i,0));

        /*TODO: Add SSE implementation*/
        for (int j = 0; j < size.width; j++)
        {
            dstLineStart[j] = (srcLineStart[j] >> 4);
        }
    }

    return result;
}

IplImage *OpenCVTools::getCVImageFromG8Buffer(G8Buffer *input)
{
    CvSize size  = getCvSizeFromVector(input->getSize());

    IplImage *result = cvCreateImage(size, IPL_DEPTH_8U, 1);
    for (int i = 0; i < size.height; i++)
    {
        uint8_t *dstLineStart = (uint8_t *)&result->imageData[i * result->widthStep];
        G8Buffer::InternalElementType *srcLineStart =  &(input->element(i,0));

        /*TODO: Add SSE implementation*/
        for (int j = 0; j < size.width; j++)
        {
            dstLineStart[j] = srcLineStart[j];
        }
    }

    return result;
}

/*TODO: Support other depth and channel numbers*/
G12Buffer *OpenCVTools::getG12BufferFromCVImage(IplImage *input)
{
    G12Buffer *toReturn = new G12Buffer(input->height, input->width);


    ASSERT_TRUE_P((input->depth == IPL_DEPTH_8U) && ((input->nChannels == 3) || input->nChannels == 1),
      ("Unsupported IplImage format: depth=%d, channels = %d\n", input->depth, input->nChannels));


        for (int i = 0; i < input->height; i++)
        {
            uint8_t *srcData = (uint8_t *)&input->imageData[i * input->widthStep];
            G12Buffer::InternalElementType *dstData =  &(toReturn->element(i,0));

            /*TODO: Add SSE implementation*/
            for (int j = 0; j < input->width; j++)
            {
                if (input->nChannels == 3)
                {
                    *dstData = (11 * srcData[0] + 16 * srcData[1] + 5 * srcData[2]) >> 1;
                    dstData++;
                    srcData += 3;
                }
                if (input->nChannels == 1)
                {
                    *dstData = *srcData * 16;
                    dstData++;
                    srcData++;
                }


            }
        }


    return toReturn;
}

/*TODO: Support other depth and channel numbers*/
RGB24Buffer *OpenCVTools::getRGB24BufferFromCVImage(IplImage *input)
{
    RGB24Buffer *toReturn = new RGB24Buffer(input->height, input->width);


    ASSERT_TRUE_P((input->depth == IPL_DEPTH_8U) && (input->nChannels == 3),
      ("Unsupported IplImage format: depth=%d, channels = %d\n", input->depth, input->nChannels));

    for (int i = 0; i < input->height; i++)
    {
        uint8_t *srcData = (uint8_t *)&input->imageData[i * input->widthStep];
        RGB24Buffer::InternalElementType *dstData =  &(toReturn->element(i,0));

        /*TODO: Add SSE implementation*/
        for (int j = 0; j < input->width; j++)
        {
            uint8_t b = *(srcData );
            uint8_t g = *(srcData + 1);
            uint8_t r = *(srcData + 2);
            *dstData = RGBColor(r,g,b);
            dstData++;
            srcData += 3;
        }
    }
    return toReturn;
}


