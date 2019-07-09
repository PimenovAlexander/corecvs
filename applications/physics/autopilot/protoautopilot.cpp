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

void ProtoAutoPilot::changeImage(QSharedPointer<QImage> inputImage)  // here we will add some data to image
{

    inputQImage = inputImage->copy();
    cv::Mat mat = QImage2Mat(inputQImage);
    cv::Mat mat1;

    if (calib == BABYHAWK_01){
    //undistort(mat, mat1, intrinsic, distCoeffs);
    }
    cv::Mat mat2;
    cv::resize(mat1,mat2,mat.size()*2);
    std::vector<std::vector<cv::Point>> squares;
    findSquares(mat2,squares);
    drawSquares(squares,mat1);
    cv::circle(mat2,cv::Point(300,300),40,cv::Scalar(255,255,255));
    //cv::imshow("mat",mat);
    outputQImage = mat2RealQImage(mat2);
    outputImage = QSharedPointer<QImage> (new QImage(outputQImage));
    outputImage->save("output2.jpg");
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

void ProtoAutoPilot::findSquares(const cv::Mat& image, vector<vector<cv::Point>>& squares)
{
    squares.clear();

   //s    Mat pyr, timg, gray0(image.size(), CV_8U), gray;

       // down-scale and upscale the image to filter out the noise
       //pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
       //pyrUp(pyr, timg, image.size());


       // blur will enhance edge detection
       cv::Mat timg(image);
       medianBlur(image, timg, 3);
       cv::Mat gray0(timg.size(), CV_8U), gray;

       vector<vector<cv::Point> > contours;

       // find squares in every color plane of the image
       for( int c = 0; c < 3; c++ )
       {
           int ch[] = {c, 0};
           mixChannels(&timg, 1, &gray0, 1, ch, 1);

           const int treshholdLevel = 4;
           // try several threshold levels
           for( int l = 0; l < treshholdLevel; l++ )
           {
               // hack: use Canny instead of zero threshold level.
               // Canny helps to catch squares with gradient shading
               if( l == 0 )
               {
                   // apply Canny. Take the upper threshold from slider
                   // and set the lower to 0 (which forces edges merging)
                   Canny(gray0, gray, 5, 50, 5);
                   // dilate canny output to remove potential
                   // holes between edge segments
                   dilate(gray, gray, cv::Mat(), cv::Point(-1,-1));
               }
               else
               {
                   // apply threshold if l!=0:
                   //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                   gray = gray0 >= (l+1)*255/treshholdLevel;
               }

               // find contours and store them all as a list
               findContours(gray, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

               vector<cv::Point> approx;

               // test each contour
               for( size_t i = 0; i < contours.size(); i++ )
               {
                   // approximate contour with accuracy proportional
                   // to the contour perimeter
                   approxPolyDP(cv::Mat(contours[i]), approx, arcLength(cv::Mat(contours[i]), true)*0.02, true);

                   // square contours should have 4 vertices after approximation
                   // relatively large area (to filter out noisy contours)
                   // and be convex.
                   // Note: absolute value of an area is used because
                   // area may be positive or negative - in accordance with the
                   // contour orientation
                   if( approx.size() == 4 &&
                       fabs(contourArea(cv::Mat(approx))) > 1000 &&
                       isContourConvex(cv::Mat(approx))

                           )
                   {
                       double maxCosine = 0;

                       for( int j = 2; j < 5; j++ )
                       {
                           // find the maximum cosine of the angle between joint edges
                           double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                           maxCosine = MAX(maxCosine, cosine);
                       }

                       // if cosines of all angles are small
                       // (all angles are ~90 degree) then write quandrange
                       // vertices to resultant sequence
                       if( maxCosine < 0.3 )
                           squares.push_back(approx);
                   }
               }
           }
       }
   }

cv::Mat ProtoAutoPilot::drawSquares( std::vector<std::vector<cv::Point> > squares, cv::Mat image )
{
    std::cout<<"new frame"<<std::endl;
    for ( int i = 0; i< squares.size(); i++ ) {

        cv::Vec3b centralPixel = image.at<cv::Vec3b>((squares[i].at(0).x+squares[i].at(3).x)/2,(squares[i].at(0).y+squares[i].at(3).y)/2);
        int sumOfColors = centralPixel[0]+centralPixel[1]+centralPixel[2];
        if (sumOfColors>500)
        {

            // draw contour
            cv::drawContours(image, squares, i, cv::Scalar(255,0,0), 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());

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
    return image;
}

double ProtoAutoPilot::angle( cv::Point pt1, cv::Point pt2, cv::Point pt0 ) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
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
    drawSquares(squares,mat1);
    cv::imshow("result",mat1);
 }


