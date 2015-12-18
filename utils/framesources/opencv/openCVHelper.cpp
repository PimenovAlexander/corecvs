#include "openCVHelper.h"

bool OpenCvHelper::captureImageCopyToBuffer(cv::VideoCapture &capture, G12Buffer* buf)
{
    cv::Mat image;
    if (!capture.grab())
        return false;

    capture.retrieve(image);

    IplImage frame = image;
    return captureImageCopyToBuffer(&frame, buf);
}

bool OpenCvHelper::captureImageCopyToBuffer(CvCapture* capHandle, G12Buffer* buf)
{
    IplImage* frame = cvQueryFrame(capHandle);
    return captureImageCopyToBuffer(frame, buf);
}

bool OpenCvHelper::captureImageCopyToBuffer(IplImage* frame, G12Buffer* buf)
{
    if (buf == NULL) {
        throw "No buffer allocated for a frame";
    }

    const uchar *imageData = (const uchar *)frame->imageData;

    /*
     * Note here that OpenCV image is stored so that each lined is 32-bits aligned thus
     * explaining the necessity to "skip" the few last bytes of each line of OpenCV image buffer.
     */
    int widthStep = frame->widthStep;
    int height    = frame->height;
    int width     = frame->width;
    int gap       = widthStep - frame->nChannels * width;

    if ((frame->depth != IPL_DEPTH_8U) || (frame->nChannels != 3))
    {
        printf("Unsupported IplImage format: depth=%d, channels = %d\n", frame->depth, frame->nChannels);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            buf->element(i, j) = (11 * imageData[0] + 16 * imageData[1] + 5 * imageData[2]) >> 1;  // b,g,r => y12
            imageData += 3;
        }
        imageData += gap;
    }
    return true;
}
