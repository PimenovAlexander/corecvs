#include "protoautopilot.h"
#include "core/utils/global.h"
#include "iostream"
#include "bits/stdc++.h"
#include <QtGui/qimage.h>
#include "vertexsquare.h"

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
    //testImageVoid();
    odinOdometryTest();
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

void ProtoAutoPilot::odinOdometryTest()
{
    getVertexSquareFromMat(QImageToMat2(QImage("test1.jpg")));
}

VertexSquare ProtoAutoPilot::getVertexSquareFromMat(cv::Mat input)
{
    QSharedPointer<QImage> q;

    cv::Mat grayInput;
    cv::cvtColor(input,grayInput,CV_RGB2GRAY);

    cv::Mat res1;
    cv::cvtColor(grayInput,res1,CV_GRAY2RGB);
    q = matToQImage(res1);
    q->save("result1.jpg");

    cv::Mat sqr = QImageToMat2(QImage("sqr.png"));
    cv::Mat graySQR;
    cv::cvtColor(sqr,graySQR,CV_RGB2GRAY);

    vector<cv::KeyPoint> keypointsOfInput;
    vector<cv::KeyPoint> keypointsOfSQR;
    int treshold = 9;

    cv::FAST(input,keypointsOfInput,treshold);
    cv::FAST(sqr,keypointsOfSQR,treshold);

    for  (int i=0;i<keypointsOfInput.size();i++)
    {
        //cv::circle(grayInput,keypointsOfInput.at(i).pt,5,cv::Scalar(0),2,8,0);
    }
    QString name = "result2";
    cv::Mat output;
    cv::cvtColor(grayInput,output,CV_GRAY2RGB);

    q = matToQImage(output);
    q->save("result.jpg");
    std::cout<<"result1"<<std::endl;
    //cv::namedWindow("test");
    //cv::imshow("test",grayInput);




    return VertexSquare();
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
    //cv::imwrite("test.jpg",canvas);
    //cv::Mat mat1 = QImageToMat2(*inputImage);
    // QImage *image = new QImage(*inputImage);
    cv::Mat mat1 = QImageToMat2(*inputImage);
    //cv::circle(mat1,cv::Point(200,200),50,cv::Scalar(255,255,255),CV_FILLED);
    QSharedPointer<QImage> r = matToQImage(temp);
}


cv::Mat ProtoAutoPilot::QImageToMat2(QImage const &srcc)
{
    debugCounter++;
    std::string s = "before-_"+std::to_string(debugCounter)+"_.jpg";
    char * tab2 = new char [s.length()+1];
    strcpy (tab2, s.c_str());
    //srcc.save(tab2);
    QImage src = srcc.convertToFormat(QImage::Format_RGB888);
    temp = cv::Mat( src.width(),src.height(),CV_8UC3,const_cast<uchar*>(src.bits()),src.bytesPerLine());
    /*cv::FileStorage storage("Mat.yml",cv::FileStorage::WRITE);
    storage<<"img"<<temp;
    storage.release();*/
    //cv::cvtColor(temp,result,CV_BGR2RGB);
    return temp;

}



QSharedPointer<QImage> ProtoAutoPilot::matToQImage(cv::Mat const &src, char* nameToSave)
{
    cv::Mat input(src.cols,src.rows,src.type()); // make the same cv::Mat
    QImage img = QImage((uchar*) input.data, input.cols, input.rows, input.step, QImage::Format_RGB888).copy();
    //QImage res = img.convertToFormat(QImage::Format_RGB32);
    outputQImage = img.convertToFormat(QImage::Format_RGB32);;
    QSharedPointer<QImage> result ( new  QImage(outputQImage));
    outputImage = QSharedPointer<QImage>(new QImage(outputQImage));
    std::string s = "after-_"+std::to_string(debugCounter)+"_.jpg";
    char * tab2 = new char [s.length()+1];
    strcpy (tab2, s.c_str());
    //res.save(tab2);

    s = "LastAfter-_"+std::to_string(debugCounter)+"_.jpg";
    tab2 = new char [s.length()+1];
    strcpy (tab2, s.c_str());

    result->save(nameToSave);
    //img.save(tab2);
    //result->save(tab2);
    return result;
}


QSharedPointer<QImage> ProtoAutoPilot::matToQImage(cv::Mat const &src)
{
    cv::Mat input(src.cols,src.rows,src.type()); // make the same cv::Mat
    QImage img = QImage((uchar*) input.data, input.cols, input.rows, input.step, QImage::Format_RGB888).copy();
    //QImage res = img.convertToFormat(QImage::Format_RGB32);
    outputQImage = img.convertToFormat(QImage::Format_RGB32);;
    QSharedPointer<QImage> result ( new  QImage(outputQImage));
    outputImage = QSharedPointer<QImage>(new QImage(outputQImage));
    std::string s = "after-_"+std::to_string(debugCounter)+"_.jpg";
    char * tab2 = new char [s.length()+1];
    strcpy (tab2, s.c_str());
    //res.save(tab2);

    s = "LastAfter-_"+std::to_string(debugCounter)+"_.jpg";
    tab2 = new char [s.length()+1];
    strcpy (tab2, s.c_str());

    //img.save(tab2);
    //result->save(tab2);
    return result;
}


void ProtoAutoPilot::testImageVoid()
{
    QImage im ("test.jpg");
    matToQImage(QImageToMat2(im))->save("resultOfTest.jpg");
}


