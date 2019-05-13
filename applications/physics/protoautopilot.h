#ifndef PROTOAUTOPILOT_H
#define PROTOAUTOPILOT_H

#include <QSharedPointer>
#include "bits/stdc++.h"
#include "copterInputs.h"
#include "list"
#include "iostream"
#include "mainObject.h"                               //Vector3dd here
#include <bits/stl_list.h>
#include <bits/stl_queue.h>
#include <queue>

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
    int debuggerSucks=0;
    cv::Mat qImageToMat(const QImage &inputImage);
    void changeImage(QSharedPointer<QImage> inputImage);
    void start();
    QSharedPointer<QImage> matToQImage(const cv::Mat &src);
     cv::Mat QImageToMat2(const QImage &src);
private:
    vector<MainObject> mainObjects;
    CopterInputs failSafe;
    std::queue<CopterInputs> outputs ;
    Vector3dd currentPos=Vector3dd(0.0,0.0,0.0);
    std::queue<Vector3dd> posFlyTo;

    std::queue<CopterInputs> getOutputs(Vector3dd diff);
};

#endif // PROTOAUTOPILOT_H
