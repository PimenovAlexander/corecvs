/**
 * \file OpenCVTools.cpp
 * \brief Add Comment Here
 *
 * \date Apr 6, 2011
 * \author alexander
 */
#include "openCVTools.h"
#include <opencv2/core/core_c.h> // cvCreateImage

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

cv::Mat OpenCVTools::getCVMatFromRGB24Buffer(RGB24Buffer *input)
{
    CvSize size  = getCvSizeFromVector(input->getSize());

    cv::Mat result(size, CV_8UC3);

    for (int i = 0; i < input->h; i++)
    {
        RGBColor *srcData = &(input->element(i,0));
        uint8_t  *dstData = (uint8_t *)&result.at<cv::Vec3b>(i,0);

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

/*TODO: Support other depth and channel numbers*/
G12Buffer *OpenCVTools::getG12BufferFromCVImage(IplImage *input)
{
    G12Buffer *toReturn = new G12Buffer(input->height, input->width);

    CORE_ASSERT_TRUE_P((input->depth == IPL_DEPTH_8U) && ((input->nChannels == 3) || (input->nChannels == 1)),
        ("Unsupported IplImage format: depth=%d, channels = %d\n", input->depth, input->nChannels));

    if (input->nChannels == 3)
    {
        for (int i = 0; i < input->height; i++)
        {
            uint8_t *srcData = (uint8_t *)&input->imageData[i * input->widthStep];
            G12Buffer::InternalElementType *dstData =  &(toReturn->element(i,0));

            /*TODO: Add SSE implementation*/
            for (int j = 0; j < input->width; j++)
            {
                *dstData = (11 * srcData[0] + 16 * srcData[1] + 5 * srcData[2]) >> 1;
                dstData++;
                srcData += 3;
            }
        }
    }

    if (input->nChannels == 1)
    {
        for (int i = 0; i < input->height; i++)
        {
            uint8_t *srcData = (uint8_t *)&input->imageData[i * input->widthStep];
            G12Buffer::InternalElementType *dstData =  &(toReturn->element(i,0));

            /*TODO: Add SSE implementation*/
            for (int j = 0; j < input->width; j++)
            {
                *dstData = *srcData * 16;
                dstData++;
                srcData++;
            }
        }
    }

    return toReturn;
}

G8Buffer *OpenCVTools::getG8BufferFromCVImage(IplImage *input)
{
    G8Buffer *toReturn = new G8Buffer(input->height, input->width);

    CORE_ASSERT_TRUE_P((input->depth == IPL_DEPTH_8U) && ((input->nChannels == 3) || (input->nChannels == 1)),
        ("Unsupported IplImage format: depth=%d, channels = %d\n", input->depth, input->nChannels));

    if (input->nChannels == 3)
    {
        for (int i = 0; i < input->height; i++)
        {
            uint8_t *srcData = (uint8_t *)&input->imageData[i * input->widthStep];
            G8Buffer::InternalElementType *dstData =  &(toReturn->element(i,0));

            /*TODO: Add SSE implementation*/
            for (int j = 0; j < input->width; j++)
            {
                *dstData = (11 * srcData[0] + 16 * srcData[1] + 5 * srcData[2]) >> 5;
                dstData++;
                srcData += 3;
            }
        }
    }

    if (input->nChannels == 1)
    {
        for (int i = 0; i < input->height; i++)
        {
            uint8_t *srcData = (uint8_t *)&input->imageData[i * input->widthStep];
            G8Buffer::InternalElementType *dstData =  &(toReturn->element(i,0));

            /*TODO: Add SSE implementation*/
            for (int j = 0; j < input->width; j++)
            {
                *dstData = *srcData; // memcpy?
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

    CORE_ASSERT_TRUE_P((input->depth == IPL_DEPTH_8U) && ((input->nChannels == 3) || (input->nChannels == 1)),
        ("Unsupported IplImage format: depth=%d, channels = %d\n", input->depth, input->nChannels));

    if (input->nChannels == 3)
    {
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
    }

    if (input->nChannels == 1)
    {
        for (int i = 0; i < input->height; i++)
        {
            uint8_t *srcData = (uint8_t *)&input->imageData[i * input->widthStep];
            RGB24Buffer::InternalElementType *dstData =  &(toReturn->element(i,0));

            /*TODO: Add SSE implementation*/
            for (int j = 0; j < input->width; j++)
            {
                uint8_t g = *(srcData );
                *dstData = RGBColor::gray(g);
                dstData++;
                srcData ++;
            }
        }
    }

    return toReturn;
}

RGB24Buffer *OpenCVTools::getRGB24BufferFromCVMat(const cv::Mat &input)
{
    RGB24Buffer *toReturn = new RGB24Buffer(input.rows, input.cols);

    CORE_ASSERT_TRUE_P((input.type() == CV_8UC1) || (input.type() == CV_8UC3),
        ("getRGB24BufferFromCVMat(): Unsupported cv::Mat format: depth=%d, type = %d\n", input.depth(), input.type()));

    if (input.type() == CV_8UC3)
    {
        for (int i = 0; i < input.rows; i++)
        {
            uint8_t *srcData = (uint8_t *)&input.at<cv::Vec3b>(i, 0);
            RGB24Buffer::InternalElementType *dstData =  &(toReturn->element(i,0));

            /*TODO: Add SSE implementation*/
            for (int j = 0; j < input.cols; j++)
            {
                uint8_t b = *(srcData );
                uint8_t g = *(srcData + 1);
                uint8_t r = *(srcData + 2);
                *dstData = RGBColor(r,g,b);
                dstData++;
                srcData += 3;
            }
        }
    }

    if (input.type() == CV_8UC1)
    {
        for (int i = 0; i < input.rows; i++)
        {
            uint8_t *srcData = (uint8_t *)&input.at<uint8_t>(i, 0);
            RGB24Buffer::InternalElementType *dstData =  &(toReturn->element(i,0));

            /*TODO: Add SSE implementation*/
            for (int j = 0; j < input.cols; j++)
            {
                uint8_t g = *(srcData );
                *dstData = RGBColor::gray(g);
                dstData++;
                srcData ++;
            }
        }
    }

    return toReturn;
}

G8Buffer *OpenCVTools::getG8BufferFromCVMat(const cv::Mat &input)
{
    G8Buffer *toReturn = new G8Buffer(input.rows, input.cols);

    CORE_ASSERT_TRUE_P((input.type() == CV_8UC1) || (input.type() == CV_8UC3),
        ("getRGB24BufferFromCVMat(): Unsupported cv::Mat format: depth=%d, type = %d\n", input.depth(), input.type()));

    if (input.type() == CV_8UC3)
    {
        for (int i = 0; i < input.rows; i++)
        {
            uint8_t *srcData = (uint8_t *)&input.at<cv::Vec3b>(i, 0);
            G8Buffer::InternalElementType *dstData =  &(toReturn->element(i,0));

            /*TODO: Add SSE implementation*/
            for (int j = 0; j < input.cols; j++)
            {
                uint8_t b = *(srcData );
                uint8_t g = *(srcData + 1);
                uint8_t r = *(srcData + 2);
                *dstData = RGBColor(r,g,b).brightness();
                dstData++;
                srcData += 3;
            }
        }
    }

    if (input.type() == CV_8UC1)
    {
        for (int i = 0; i < input.rows; i++)
        {
            uint8_t *srcData = (uint8_t *)&input.at<uint8_t>(i, 0);
            G8Buffer::InternalElementType *dstData =  &(toReturn->element(i,0));

            /*TODO: Add SSE implementation*/
            for (int j = 0; j < input.cols; j++)
            {
                uint8_t g = *(srcData ); //memcpy
                *dstData = g;
                dstData++;
                srcData ++;
            }
        }
    }

    return toReturn;
}

cv::Mat OpenCVTools::convert(const corecvs::RuntimeTypeBuffer &buffer)
{
    return cv::Mat((int)buffer.getRows()
        , (int)buffer.getCols()
        , (int)buffer.getCvType()
        , buffer.row<void>((size_t)0));
}

corecvs::RuntimeTypeBuffer OpenCVTools::convert(const cv::Mat &mat)
{
    return RuntimeTypeBuffer(mat.rows, mat.cols
        , mat.data
        , (int)RuntimeTypeBuffer::getTypeFromCvType(mat.type()));
}
