#include <iostream>
#include <QDebug>

#include "test.h"


using namespace std;

Test::Test(QObject *parent) :
    QObject(parent)
    , mTimer(NULL)
    , mInterval(50)
{
}

void Test::runTest()
{
    mMachineEventFlow.clear();
    setMachineEvent();
    mMachine.changeActiveState(mInitState);
    delete mTimer;
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(setState()));
    mTimer->start(mInterval);
}

Test::~Test()
{
    delete mTimer;
}

void Test::setInterval(int interval)
{
    mInterval = interval;
}

void Test::setState()
{
    if (mMachineEventFlow.isEmpty())
    {
        mTimer->stop();
        if (mMachine.currentState() != mFinalState)
        {
            cout << "State is " << mMachine.toString().toStdString() << ", should be " << mFinalState << endl;
        }
        else
        {
            cout << "test passed" << endl;
        }
        emit testFinished();
        return;
    }

    MachineEvent data = mMachineEventFlow.at(0).first;

    if (mMachineEventFlow.at(0).second <= 1)
        mMachineEventFlow.pop_front();
    else
        mMachineEventFlow[0].second--;

    mMachine.addData(data);
}
