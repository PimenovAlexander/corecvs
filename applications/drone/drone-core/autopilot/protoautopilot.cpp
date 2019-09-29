#include "protoautopilot.h"
#include "core/utils/global.h"
#include "iostream"
#include "bits/stdc++.h"
#include <QtGui/qimage.h>
#include "vertexsquare.h"
#include <mutex>
#include <chrono>

#include <QFileInfo>
#include <QSharedPointer>
#include "droneObject.h"
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

    if (!testMode)
    {
        std::cout<<"death to leather bastards"<<std::endl;
        failSafe = CopterInputs();
        failSafe.axis[CopterInputs::CHANNEL_THROTTLE]=1300;
        //v.testVertex(230,230,45,45,230,45,45,230);
    }
    else
    {
        if(QFileInfo::exists("test1.jpg"))
        {
            QImage testImage = QImage("test1.jpg");
            QSharedPointer<QImage> testPointer = QSharedPointer<QImage> (new QImage(testImage));
            changeImage(testPointer);
        }
        else
        {
            std::cout<<"test images not found"<<std::endl;
        }
    }
}


void ProtoAutoPilot::start()   //
{
    debugCounter=0;
    //testImageVoid();
    //odinOdometryTest();
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
    //getVertexSquareFromMat(QImageToMat2(QImage("test1.jpg")));
}

VertexSquare ProtoAutoPilot::getVertexSquareFromMat(cv::Mat const inputtt)
{
    cv::Mat sqr = QImage2Mat(QImage("test1.jpg"));
    cv::Mat input = QImage2Mat(QImage("sqr2.jpg"));

    #ifdef OPENCV_ENABLE_NONFREE
    vector<cv::KeyPoint> keypointsOfInput;
    vector<cv::KeyPoint> keypointsOfSQR;
    int treshold = 9;

    int minHessian = 800;

    cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create( minHessian);

    detector->setHessianThreshold(400);
    cv::Mat descriptors1, descriptors2;
    cv::imshow("input",input);
    cv::imshow("sqr",sqr);

    detector->detectAndCompute( input, cv::noArray(), keypointsOfInput, descriptors1 );
    detector->detectAndCompute( sqr, cv::noArray(), keypointsOfSQR, descriptors2 );

    std::vector<std::vector<cv::DMatch>> matches;
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    matcher->knnMatch(descriptors1,descriptors2,matches,2);
    const float ratio_thresh = 0.7f;

    std::vector<cv::DMatch> goodMatches;
    for (size_t i = 0; i < matches.size(); i++)
        {
            //if (matches[i][0].distance < ratio_thresh * matches[i][1].distance)
            //{
                goodMatches.push_back(matches[i][0]);
            //}
        }
    cv::Mat imgMatches;
    cv::drawMatches(input,keypointsOfInput,sqr,keypointsOfSQR,goodMatches,imgMatches,cv::Scalar::all(-1),cv::Scalar::all(-1)
                   ,std::vector<char>(),cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
    cv::imshow("matches",imgMatches);

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

void ProtoAutoPilot::setCalibration(Calibration c)
{
    calib = c;
}

ProtoAutoPilot::Calibration ProtoAutoPilot::getCalibration()
{
    return calib;
}

void ProtoAutoPilot::changeImage(QSharedPointer<QImage> inputImage)
{

    inputQImage = inputImage->copy();
    debugCounter++;
    QString n = "inputImage___"+ QString::number(debugCounter);
    inputImage->save("inputImage___1");

    cv::Mat mat = QImage2Mat(inputQImage);
    //cv::Mat mat1;

    if (calib == BABYHAWK_01){
    //undistort(mat, mat1, intrinsic, distCoeffs);
    }
    cv::Mat mat2;
    if (testMode)
    {
        cv::resize(mat,mat2,mat.size());
    }
    else
    {
        cv::resize(mat,mat2,mat.size()*2);
    }
    std::vector<std::vector<cv::Point>> squares;
    findSquares(mat2,squares);
    VertexSquare vertexSquare;
    if (drawSquares(squares,mat2,&vertexSquare))
    {
        int rollDelta = vertexSquare.edges[0] - vertexSquare.edges[2];
        int pitchDelta = squarePerimeter - vertexSquare.perimetr;
        int throttleDelta = vertexSquare.edges[1] - vertexSquare.edges[3];
        int yawDelta = frameHeight - vertexSquare.centre[1];           //image was resized, 2 decreased                                              //here comes the PID

        std::chrono::high_resolution_clock::time_point newTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(newTime-lastTime);
        lastTime = newTime;
        double throttleCommand = throttlePID.calculate(timeSpan.count(),throttleDelta,targetPoint.z());
        double pitchCommand = throttlePID.calculate(timeSpan.count(),pitchDelta,targetPoint.x());
        double rollCommand = throttlePID.calculate(timeSpan.count(),rollDelta,targetPoint.y());
        double yawCommand = throttlePID.calculate(timeSpan.count(),yawDelta,0);         //we want the square to be centered

        if (throttleCommand > 1700){throttleCommand = 1700;}
        if (pitchCommand > 1700){pitchCommand = 1700;}
        if (rollCommand > 1700){pitchCommand = 1700;}
        if (yawCommand > 1700){pitchCommand = 1700;}
        calculateOutput(throttleCommand,pitchCommand,rollCommand,yawCommand);
    }
    else
    {
       //setOutput(failSafe);
    }

    //cv::circle(mat2,cv::Point(300,300),40,cv::Scalar(255,255,255));
    cv::imshow("mat",mat);
    if (testMode)
    {
        cv::imshow("test",mat2);
    }
    outputQImage = mat2RealQImage(mat2);
    outputImage = QSharedPointer<QImage> (new QImage(outputQImage));
    outputImage->save("output___1.jpg");
}

void ProtoAutoPilot::calculateOutput(double throttle, double pitch,double roll,double yaw )
{
    CopterInputs copterInputs;
    if (!outputTestMode)
    {
        copterInputs.axis[0] = throttle;
        copterInputs.axis[1] = roll;
        copterInputs.axis[2] = pitch;
        copterInputs.axis[3] = yaw;
    }
    else
    {
        copterInputs.axis[0] = 1350;
        copterInputs.axis[1] = 1500;
        copterInputs.axis[2] = 1500;
        copterInputs.axis[3] = 1700;
    }
    setOutput(copterInputs);
}

void ProtoAutoPilot::setOutput(CopterInputs copterinputs)
{
    outputMutex.lock();
    output = copterinputs;
    outputMutex.unlock();
}

CopterInputs ProtoAutoPilot::getOutput()
{
    outputMutex.lock();
    CopterInputs result = output;
    outputMutex.unlock();
    return result;
}

QSharedPointer<QImage> ProtoAutoPilot::mat2QImage(cv::Mat const &src)     // B<->R
{
    cv::Mat input(src.rows,src.cols,src.type()); // make the same cv::Mat
    //cv::cvtColor(src,input,CV_BGR2RGB);
    cv::imshow("mat",src);
    QImage img = QImage((uchar*) input.data, input.cols, input.rows, input.step, QImage::Format_RGB888);
    QImage res = img.convertToFormat(QImage::Format_RGB32);
    QSharedPointer<QImage> result ( new  QImage(res));
    return result;
}

QImage ProtoAutoPilot::mat2RealQImage(cv::Mat const &src)     // B<->R
{
    //cv::Mat input(src.rows,src.cols,src.type()); // here we have problem
    //cv::Mat input = src.copyTo();
    //cv::imshow("mat",src);
    //QImage img = QImage((uchar*) input.data, input.cols, input.rows, input.step, QImage::Format_RGB888);
    QImage img = QImage((uchar*) src.data, src.cols, src.rows, src.step, QImage::Format_RGB888);
    QImage res = img.convertToFormat(QImage::Format_RGB32);
    return res.copy();
}

bool ProtoAutoPilot::findSquares(const cv::Mat& image, vector<vector<cv::Point>>& squares)
{
    /**/
    return false;
}

bool ProtoAutoPilot::drawSquares( std::vector<std::vector<cv::Point> > squares, cv::Mat image, VertexSquare  *vertexSquare)
{
    bool result = false;
    std::cout<<"new frame"<<std::endl;
    for ( int i = 0; i< squares.size(); i++ ) {

        cv::Vec3b centralPixel = image.at<cv::Vec3b>((squares[i].at(0).x+squares[i].at(3).x)/2,(squares[i].at(0).y+squares[i].at(3).y)/2);
        int sumOfColors = centralPixel[0]+centralPixel[1]+centralPixel[2];
        if (sumOfColors>500)
        {
            result = true;
            // draw contour
            cv::drawContours(image, squares, i, cv::Scalar(255,0,0), 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
            *vertexSquare = VertexSquare(squares.at(0).at(0).x,squares.at(0).at(0).y,squares.at(0).at(1).x,squares.at(0).at(1).y,squares.at(0).at(2).x
                                      ,squares.at(0).at(2).y,squares.at(0).at(3).x,squares.at(0).at(3).y);
            /*
            // draw bounding rect
            cv::Rect rect = boundingRect(cv::Mat(squares[i]));
            cv::rectangle(image, rect.tl(), rect.br(), cv::Scalar(0,255,0), 2, 8, 0);

            // draw rotated rect
            cv::RotatedRect minRect = minAreaRect(cv::Mat(squares[i]));
            cv::Point2f rect_points[4];
            minRect.points( rect_points );
            std::cout<<"square "<<i;
            for ( int j = 0; j < 4; j++ )
            {
                std::cout<<rect_points[j].x<<" "<<rect_points[j].y<<"-_-";
            }
            //std::cout<<"central pixel"<<image.at<cv::Vec3b>((rect_points[0].x+rect_points[3].x)/2,(rect_points[0].y+rect_points[3].y)/2);

            std::cout<<""<<std::endl;
            for ( int j = 0; j < 4; j++ ) {
                cv::line( image, rect_points[j], rect_points[(j+1)%4], cv::Scalar(0,0,255), 1, 8 ); // blue
            }
            */
        }
    }
    std::cout<<"end frame"<<std::endl;
    return result;
}

cv::Mat ProtoAutoPilot::QImage2Mat(QImage const &srcc)
{
    QImage src = srcc.convertToFormat(QImage::Format_RGB888);
    cv::Mat matOutput = cv::Mat( src.height(),src.width(),CV_8UC3,const_cast<uchar*>(src.bits()),src.bytesPerLine());
    cv::Mat result;
    matOutput.copyTo(result);
    return result;
}

void ProtoAutoPilot::testImageVoid()
{

    QImage im ("test1.jpg");
    cv::Mat mat = QImage2Mat(im);
    std::vector<std::vector<cv::Point>> squares;
    cv::Mat mat1;
    cv::resize(mat,mat1,mat.size()*2);
    findSquares(mat1,squares);
    //drawSquares(squares,mat1);
    cv::imshow("result",mat1);
 }


