#include "protoautopilot.h"
#include "core/utils/global.h"
 #include "iostream"
#include "bits/stdc++.h"

#include <QSharedPointer>

using namespace std;
ProtoAutoPilot::ProtoAutoPilot()
{
    cout<<"death to leather bastards"<<endl;
    failSafe= CopterInputs();
    failSafe.axis[CopterInputs::CHANNEL_THROTTLE]=1300;
}


void ProtoAutoPilot::makeStrategy(QSharedPointer<QImage> inputImage)
{

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
    //count
    return result;
}

std::queue<CopterInputs> ProtoAutoPilot::getOutputs(Vector3dd diff)              // let me think
{                                                                                // we have 1 frame per 1/30 sec
     std::queue<CopterInputs> result;                                            // if i can count we send ~6-7 comands per 1/30 sec
                                                                                 //here we need pid + speed value from copter
     return result;
}

void  ProtoAutoPilot::stabilise()
{

}



