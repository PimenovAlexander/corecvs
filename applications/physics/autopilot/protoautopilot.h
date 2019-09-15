#ifndef PROTOAUTOPILOT_H
#define PROTOAUTOPILOT_H

#include "vertexsquare.h"
#include <QSharedPointer>
#include "bits/stdc++.h"
#include "copterInputs.h"
#include "list"
#include "iostream"
#include "mainObject.h"                               //Vector3dd here, and i dont understand where exactly
#include <bits/stl_list.h>
#include <bits/stl_queue.h>
#include <queue>
#include <QtGui/qimage.h>

#include <opencv2/features2d/features2d.hpp>

#include <droneObject.h>

class ProtoAutoPilot
{
public:
    ProtoAutoPilot();
    void makeStrategy(QSharedPointer<QImage> im);
    QSharedPointer<QImage> outputImage;
    CopterInputs output;
    /*CopterInputs output()
    {
        CopterInputs result=outputs.front();
        outputs.pop();
        return result;
    }*/
    bool active=false;
    void stabilise();
    enum Calibration {NO_CALIBRATION, BABYHAWK_01};
    Vector3dd getCurreentPos(QSharedPointer<QImage> inputImage);
    int debugCounter=0;
    void changeImage(QSharedPointer<QImage> inputImage);
    void start();
    void testImageVoid();
    void odinOdometryTest();

    QSharedPointer<QImage> mat2QImage(const cv::Mat &src);
    cv::Mat QImage2Mat(const QImage &srcc);
    VertexSquare getVertexSquareFromMat(const cv::Mat input);

    void setCalibration(Calibration c);
    ProtoAutoPilot::Calibration getCalibration();
private:
    PID throttlePID{0.7, 0.35, 0.35};
    PID pitchPID{0.7, 0.35, 0.35};
    PID rollPID{0.7, 0.35, 0.35};
    PID yawPID{0.7, 0.35, 0.35};

    bool testMode = false;                              //put false to use drone
    int frameHeight=480;
    int frameWidth=640;
    int squarePerimeter = 120*4;  //perimetr that seen from (0,0)
    //for memory
    CopterInputs failSafe;
    cv::Mat temp;
    QImage outputQImage;
    QImage inputQImage;
    Calibration calib = NO_CALIBRATION;

    vector<MainObject> mainObjects;
    std::queue<CopterInputs> outputs ;
    Vector3dd currentPos=Vector3dd(0.0,0.0,0.0);
    std::queue<Vector3dd> posFlyTo;

    std::queue<CopterInputs> getOutputs(Vector3dd diff);

    double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0);
    bool drawSquares(std::vector<std::vector<cv::Point> > squares, cv::Mat image, VertexSquare *vertexSquare);
    bool findSquares(const cv::Mat &image, vector<vector<cv::Point> > &squares);
    QImage mat2RealQImage(const cv::Mat &src);
};

#endif // PROTOAUTOPILOT_H
