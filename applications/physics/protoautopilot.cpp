#include "protoautopilot.h"
#include "core/utils/global.h"
#include "iostream"
#include "bits/stdc++.h"
#include <QtGui/qimage.h>

#include <QSharedPointer>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

using namespace std;
ProtoAutoPilot::ProtoAutoPilot()
{
    cout<<"death to leather bastards"<<endl;
    failSafe= CopterInputs();
    failSafe.axis[CopterInputs::CHANNEL_THROTTLE]=1300;
}


void ProtoAutoPilot::start()
{
    active=true;
}

void ProtoAutoPilot::makeStrategy(QSharedPointer<QImage> inputImage)
{
    changeImage(inputImage);
    if (posFlyTo.size()==0)         //for example first frame
    {
        posFlyTo.push(Vector3dd(0.0,0.0,0.0));               // it will hold on first position
    }
    else
    {
        Vector3dd diff = posFlyTo.front() - getCurreentPos(inputImage);
        outputs = getOutputs(diff);
    }
}

Vector3dd  ProtoAutoPilot::getCurreentPos(QSharedPointer<QImage> inputImage)
{
    Vector3dd result;

    return result;
}



std::queue<CopterInputs> ProtoAutoPilot::getOutputs(Vector3dd diff)
{
     std::queue<CopterInputs> result;

     return result;
}

void  ProtoAutoPilot::stabilise()
{
}

void ProtoAutoPilot::changeImage(QSharedPointer<QImage> inputImage)  // here we will add some data to image
{
    //cv::Mat img = cv::imread("/home/test.jpg");
    //cv::circle(img,cv::Point(30,30),20,cv::Scalar(0,0,0));
    cv::Mat canvas = qImageToMat(*inputImage);
    //cv::imshow("Display window",canvas);
    //cv::circle(canvas,cv::Point(30,30),20,cv::Scalar(0,0,0));
    outputImage = matToQImage(canvas);
}

cv::Mat ProtoAutoPilot::qImageToMat(const QImage &inputIm)
{
    cv::Mat result(inputIm.height(),inputIm.width(),CV_8UC3);
    for (int i=0;i<inputIm.height();i++)
    {
        memcpy(result.ptr(i),inputIm.scanLine(i),inputIm.bytesPerLine());   //memory allocated by cv::mat may not have the same bytesPerine as the QImage, but maybe it will work
    }
    cout<<"for dot"<<endl;
    return result;
}

QSharedPointer<QImage> ProtoAutoPilot::matToQImage(cv::Mat input)
{
    QImage img = QImage((uchar*) input.data,input.cols,input.step, QImage::Format_RGB888);
    QSharedPointer<QImage> result (new QImage(img));
    return result;
}





