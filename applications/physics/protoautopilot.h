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

class ProtoAutoPilot
{
public:
    ProtoAutoPilot();
    void makeStrategy(QSharedPointer<QImage> im);
    CopterInputs output()
    {
        CopterInputs result=outputs.front();
        outputs.pop();
        return result;
    }
    bool active=false;
    void stabilise();
    Vector3dd getCurreentPos(QSharedPointer<QImage> inputImage);
private:
    vector<MainObject> mainObjects;
    CopterInputs failSafe;
    std::queue<CopterInputs> outputs ;
    Vector3dd currentPos=Vector3dd(0.0,0.0,0.0);
    std::queue<Vector3dd> posFlyTo;

    std::queue<CopterInputs> getOutputs(Vector3dd diff);
};

#endif // PROTOAUTOPILOT_H
