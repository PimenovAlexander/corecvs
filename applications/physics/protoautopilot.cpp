#include "protoautopilot.h"
#include "core/utils/global.h"
#include "iostream"
#include "bits/stdc++.h"
#include <QtGui/qimage.h>

#include <QSharedPointer>
#include <opencv2/features2d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>

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
    //cv::Mat canvas = qImageToMat(*inputImage);
    //cv::imshow("Display window",canvas);
    //cv::circle(canvas,cv::Point(30,30),20,cv::Scalar(0,0,0));
    //cv::cvtColor(canvas,canvas,CV_BGR2GRAY);
   // cv::imwrite("test.jpg",canvas);
    cv::Mat mat1= QImageToMat2(*inputImage);
    outputImage = matToQImage(mat1);
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

QImage Mat2QImage(cv::Mat const& src)
{

     cv::Mat temp(src.cols,src.rows,src.type()); // make the same cv::Mat
     cvtColor(src, temp,CV_BGR2RGB); // cvtColor Makes a copt, that what i need
     //cv::imwrite("111",src);
     //cv::imshow("name",temp);
     QImage dest(temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits();
     return dest.copy();
}

cv::Mat ProtoAutoPilot::QImageToMat2(QImage const &srcc)
{
    debuggerSucks++;
    std::string s = "before-_"+std::to_string(debuggerSucks)+"_.jpg";
    char * tab2 = new char [s.length()+1];
    strcpy (tab2, s.c_str());
    srcc.save(tab2);

    QImage src = srcc.convertToFormat(QImage::Format_RGB888);
    cv::Mat temp( src.width(),src.height(),CV_8UC3,const_cast<uchar*>(src.bits()),src.bytesPerLine());
    cv::Mat result;
    //cv::cvtColor(temp,result,CV_BGR2RGB);
    return temp.clone();

}


QSharedPointer<QImage> ProtoAutoPilot::matToQImage(cv::Mat const& src)
{

    cv::Mat input(src.cols,src.rows,src.type()); // make the same cv::Mat
    QImage img= QImage((uchar*) input.data, input.cols, input.rows, input.step, QImage::Format_RGB888).copy();
    QImage res = img.convertToFormat(QImage::Format_RGB32);
    QSharedPointer<QImage> result ( new  QImage(res));
    std::string s = "after-_"+std::to_string(debuggerSucks)+"_.jpg";
    char * tab2 = new char [s.length()+1];
    strcpy (tab2, s.c_str());
    result->save(tab2);
    return result;
}





