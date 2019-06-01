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

class ProtoAutoPilot
{
public:
    ProtoAutoPilot();
    void makeStrategy(QSharedPointer<QImage> im);
    QSharedPointer<QImage> outputImage;
    CopterInputs output()
    {
        CopterInputs result=outputs.front();
        outputs.pop();
        return result;
    }
    bool active=false;
    void stabilise();
    Vector3dd getCurreentPos(QSharedPointer<QImage> inputImage);
    int debugCounter=0;
    void changeImage(QSharedPointer<QImage> inputImage);
    void start();
    //QSharedPointer<QImage> matToQImage(const cv::Mat &src);
    //cv::Mat QImageToMat2(const QImage &src);
    void testImageVoid();
    void odinOdometryTest();
    //QSharedPointer<QImage> matToQImage(const cv::Mat &src, char nameToSave[]);

    QSharedPointer<QImage> mat2QImage(const cv::Mat &src);
    cv::Mat QImage2Mat(const QImage &srcc);
    VertexSquare getVertexSquareFromMat(const cv::Mat input);
private:
     //for memory
     cv::Mat temp;
     QImage outputQImage;


    vector<MainObject> mainObjects;
    CopterInputs failSafe;
    std::queue<CopterInputs> outputs ;
    Vector3dd currentPos=Vector3dd(0.0,0.0,0.0);
    std::queue<Vector3dd> posFlyTo;

    std::queue<CopterInputs> getOutputs(Vector3dd diff);
};

#endif // PROTOAUTOPILOT_H
