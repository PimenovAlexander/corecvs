#include "protoautopilot.h"
#include "core/utils/global.h"
#include "iostream"
#include "bits/stdc++.h"
#include <QtGui/qimage.h>
#include "vertexsquare.h"

#include <QSharedPointer>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d.hpp>


using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;
ProtoAutoPilot::ProtoAutoPilot()
{
    cout<<"death to leather bastards"<<endl;
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
    //QSharedPointer<QImage> q;

    cv::Mat grayInput;
    cv::cvtColor(input,grayInput,CV_RGB2GRAY);
    cv::imshow("input",input);
    //cv::Mat res1;
    //cv::cvtColor(grayInput,res1,CV_GRAY2RGB);
    //q = mat2QImage(res1);
    //q->save("result1.jpg");

    cv::Mat sqr = QImage2Mat(QImage("sqr.png"));
    cv::Mat graySQR;
    cv::cvtColor(sqr,graySQR,CV_RGB2GRAY);

    vector<cv::KeyPoint> keypointsOfInput;
    vector<cv::KeyPoint> keypointsOfSQR;
    int treshold = 9;

    //cv::FAST(grayInput,keypointsOfInput,treshold);
    //cv::FAST(graySQR,keypointsOfSQR,treshold);

    std::cout<<"here2"<<std::endl;
    int minHessian = 800;

    Ptr< xfeatures2d::SURF > detector = xfeatures2d::SURF::create( minHessian );

    //Ptr<SURF> detector = SURF::create( minHessian,4,3,false,false );
    std::cout<<"here3"<<std::endl;

    detector->setHessianThreshold(400);
    cv::Mat descriptors1, descriptors2;
    std::cout<<"here3.5"<<std::endl;
    if (debugCounter>2)                // why does it works only after 2 click???????????????
    {
        detector->detectAndCompute( grayInput, cv::noArray(), keypointsOfInput, descriptors1 );
        std::cout<<"here3.7"<<std::endl;
        detector->detectAndCompute( graySQR, cv::noArray(), keypointsOfSQR, descriptors2 );

        std::cout<<"here4"<<std::endl;
        std::vector<std::vector<cv::DMatch>> matches;
        cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
         matcher->knnMatch(descriptors1,descriptors2,matches,2);
        const float ratio_thresh = 0.7f;

        std::cout<<"here5"<<std::endl;
        std::vector<cv::DMatch> goodMatches;
        for (size_t i = 0; i < matches.size(); i++)
            {
                if (matches[i][0].distance < ratio_thresh * matches[i][1].distance)
                {
                    goodMatches.push_back(matches[i][0]);
                }
            }


        cv::Mat imgMatches;

        cv::drawMatches(grayInput,keypointsOfInput,graySQR,keypointsOfSQR,goodMatches,imgMatches,cv::Scalar::all(-1),cv::Scalar::all(-1)
                       ,std::vector<char>(),cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
    }

    //cv::imshow("matches",imgMatches);

    /*for  (int i=0;i<keypointsOfInput.size();i++)
    {
        cv::circle(grayInput,keypointsOfInput.at(i).pt,5,cv::Scalar(0),2,8,0);
    }
    cv::imshow("FAST 1 test",grayInput);
    */

    //cv::Mat output;
    //cv::cvtColor(grayInput,output,CV_GRAY2RGB);

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
    cv::Mat mat1 = QImage2Mat(*inputImage);
    //cv::circle(mat1,cv::Point(200,200),50,cv::Scalar(255,255,255),CV_FILLED);
    QSharedPointer<QImage> r = mat2QImage(temp);
}

/*
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
    storage.release();
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
    //img.save(tab2);Снимок экрана от 2019-06-01 19-09-44
    //result->save(tab2);
    return result;
}


QSharedPointer<QImage> ProtoAutoPilot::matToQImage(cv::Mat const &src)
{
    cv::Mat input(src.cols,src.rows,src.type()); // make the same cv::Mat
    cv::imshow("Display window",input);
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

*/

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
    cv::Mat output = cv::Mat( src.height(),src.width(),CV_8UC3,const_cast<uchar*>(src.bits()),src.bytesPerLine());
    return output;
}

void ProtoAutoPilot::testImageVoid()
{
    QImage imm ("test1.jpg");
    QImage2Mat(imm);
    QImage2Mat(imm);
    QImage im ("test1.jpg");
    cv::Mat mat = QImage2Mat(im);
    cv::imshow("Before FAST",mat);
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


