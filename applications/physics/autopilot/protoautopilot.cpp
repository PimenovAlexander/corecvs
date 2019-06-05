#include "protoautopilot.h"
#include "core/utils/global.h"
#include "iostream"
#include "bits/stdc++.h"
#include <QtGui/qimage.h>
#include "vertexsquare.h"

#include <QSharedPointer>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

#include <opencv2/features2d.hpp>


ProtoAutoPilot::ProtoAutoPilot()
{

    std::cout<<"death to leather bastards"<<std::endl;
    failSafe= CopterInputs();
    failSafe.axis[CopterInputs::CHANNEL_THROTTLE]=1300;
}


void ProtoAutoPilot::start()   //first QImage2Mat will be broken?!, other - good
{
    debugCounter=0;
    //testImageVoid();
    //odinOdometryTest();
    //active=true;
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
    //getVertexSquareFromMat(QImageToMat2(QImage("test1.jpg")));
}

VertexSquare ProtoAutoPilot::getVertexSquareFromMat(cv::Mat const input)
{
    cv::imshow("input",input);
    cv::Mat sqr = QImage2Mat(QImage("sqr.png"));
    cv::imshow("sqr",sqr);

    #ifdef OPENCV_ENABLE_NONFREE
    vector<cv::KeyPoint> keypointsOfInput;
    vector<cv::KeyPoint> keypointsOfSQR;
    int treshold = 9;

    int minHessian = 800;

    cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create( minHessian);

    detector->setHessianThreshold(400);
    cv::Mat descriptors1, descriptors2;
    if (debugCounter>2)                // why does it works only after 2 click???????????????
    {
        detector->detectAndCompute( input, cv::noArray(), keypointsOfInput, descriptors1 );
        detector->detectAndCompute( sqr, cv::noArray(), keypointsOfSQR, descriptors2 );

        std::vector<std::vector<cv::DMatch>> matches;
        cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
        matcher->knnMatch(descriptors1,descriptors2,matches,2);
        const float ratio_thresh = 0.7f;

        std::vector<cv::DMatch> goodMatches;
        for (size_t i = 0; i < matches.size(); i++)
            {
                if (matches[i][0].distance < ratio_thresh * matches[i][1].distance)
                {
                    goodMatches.push_back(matches[i][0]);
                }
            }

        cv::Mat imgMatches;

        cv::drawMatches(input,keypointsOfInput,sqr,keypointsOfSQR,goodMatches,imgMatches,cv::Scalar::all(-1),cv::Scalar::all(-1)
                       ,std::vector<char>(),cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
        cv::imshow("matches",imgMatches);
    }
    //cv::namedWindow("test");
    #else
    std::cout<<"you need OPENCV_ENABLE_NONFREE"<<std::endl;
    #endif
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
    cv::Mat mat1 = QImage2Mat(*inputImage);
    //cv::circle(mat1,cv::Point(200,200),50,cv::Scalar(255,255,255),CV_FILLED);
    QSharedPointer<QImage> r = mat2QImage(temp);
}

QSharedPointer<QImage> ProtoAutoPilot::mat2QImage(cv::Mat const &src)     // B<->R
{
    cv::Mat input(src.rows,src.cols,src.type()); // make the same cv::Mat
    QImage img = QImage((uchar*) input.data, input.cols, input.rows, input.step, QImage::Format_RGB888).copy();
    QImage res = img.convertToFormat(QImage::Format_RGB32);
    QSharedPointer<QImage> result ( new  QImage(outputQImage));
    return result;
}

cv::Mat ProtoAutoPilot::QImage2Mat(QImage const &srcc)
{
    QImage src = srcc.convertToFormat(QImage::Format_RGB888);
    cv::Mat matOutput = cv::Mat( src.height(),src.width(),CV_8UC3,const_cast<uchar*>(src.bits()),src.bytesPerLine());
    return matOutput;
}

void ProtoAutoPilot::testImageVoid()
{
    QImage imm ("test1.jpg");
    QImage2Mat(imm);
    QImage2Mat(imm);
    QImage im ("test1.jpg");
    cv::Mat mat = QImage2Mat(im);
    //cv::imshow("Before FAST",mat);
    //cv::circle(mat,cv::Point(30,30),20,cv::Scalar(0,0,0));
    if (debugCounter>0)                          // after 1 click we have trash in the image ?!
    {
        VertexSquare vertex = getVertexSquareFromMat(mat);
    }
    //cv::imshow("result Mat",mat);
    QString name="result1.jpg";
    QSharedPointer<QImage> result = mat2QImage(mat);
    result->save(name);

    debugCounter++;
 }


