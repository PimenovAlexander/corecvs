#ifndef PROTOAUTOPILOT_H
#define PROTOAUTOPILOT_H

#include <list>
#include <iostream>
#include <queue>
#include <mutex>

#include <QSharedPointer>
#include <QtGui/QImage>

#include <opencv2/features2d/features2d.hpp>

#include "vertexsquare.h"
#include "copterInputs.h"
#include "simulation/mainObject.h"                               //Vector3dd here, and i dont understand where exactly
#include "copter/droneObject.h"

class ProtoAutoPilot
{
public:
    ProtoAutoPilot();
    void makeStrategy(QSharedPointer<QImage> im);
    QSharedPointer<QImage> outputImage;

    /*CopterInputs output()
    {
        CopterInputs result=outputs.front();
        outputs.pop();
        return result;
    }*/
    bool active=false;

    void stabilise();
    enum Calibration {
        NO_CALIBRATION,
        BABYHAWK_01
    };

    Vector3dd getCurreentPos(QSharedPointer<QImage> inputImage);
    int debugCounter=0;
    void changeImage(QSharedPointer<QImage> inputImage);
    void start();
    void testImageVoid();
    void odinOdometryTest();
    CopterInputs getOutput();
    QSharedPointer<QImage> mat2QImage(const cv::Mat &src);
    cv::Mat QImage2Mat(const QImage &srcc);
    VertexSquare getVertexSquareFromMat(const cv::Mat input);

    void setCalibration(Calibration c);
    ProtoAutoPilot::Calibration getCalibration();

private:
    CopterInputs output;
    std::mutex outputMutex;
    void setOutput(CopterInputs copterinputs);
    std::chrono::high_resolution_clock::time_point lastTime;

    double minPidValue = 900;
    double maxPidValue = 1750;

    Vector3dd targetPoint{0,0,0};

    PID throttlePID { minPidValue , maxPidValue,0.7, 0.45, 0.3};
    PID pitchPID    { minPidValue , maxPidValue,0.7, 0.35, 0.35};
    PID rollPID     { minPidValue , maxPidValue ,0.7, 0.35, 0.35};
    PID yawPID      { minPidValue , maxPidValue ,0.7, 0.35, 0.35};
                                                               //// testModes (i do not want crash the copter)
    bool testMode = false;
    bool outputTestMode = true;
    //put false to use drone
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
    void calculateOutput(double throttle, double pitch, double roll, double yaw);
};

#endif // PROTOAUTOPILOT_H
