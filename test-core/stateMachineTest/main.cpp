#include <iostream>
#include <QCoreApplication>

#include "../../qt4/host-soft/stateMachine/stateMachine.h"
#include "test.h"


using namespace std;

const int frameNumber = 15;

class TestIdleToGaper : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::idle;
        MachineEvent initMachineEvent;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(initMachineEvent, frameNumber));
        MachineEvent data;
        data.objectInCZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int> (data, frameNumber));
        mFinalState = States::gaper;
    }
};

class TestControlToControl : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::control;
        MachineEvent data;
        data.handInCW = true;
        data.objectInCZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
        mFinalState = States::control;
    }
};

class TestControlToGapZone : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::control;
        MachineEvent initMachineEvent;
        initMachineEvent.faceInCZ = true;
        initMachineEvent.faceInGZ = true;
        initMachineEvent.objectInCZ = true;
        initMachineEvent.handInCW = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int> (initMachineEvent, frameNumber));
        MachineEvent data;
        data.objectInCZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
        mFinalState = States::gaper;
    }
};

class TestControlToIdle : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::control;
        mFinalState = States::idle;
        MachineEvent initMachineEvent;
        initMachineEvent.objectInCZ = true;
        initMachineEvent.handInCW = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(initMachineEvent, frameNumber));
        MachineEvent data;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
    }
};

class TestControlToReady : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::control;
        MachineEvent initMachineEvent;
        initMachineEvent.objectInCZ = true;
        initMachineEvent.handInCW = true;
        initMachineEvent.faceInGZ = true;
        initMachineEvent.faceInCZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(initMachineEvent, frameNumber));
        MachineEvent data;
        data.faceInGZ = true;
        data.faceInCZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
        mFinalState = States::ready;
    }
};

class TestControlToFace : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::control;
        MachineEvent initMachineEvent;
        initMachineEvent.objectInCZ = true;
        initMachineEvent.faceInGZ = true;
        initMachineEvent.handInCW = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(initMachineEvent, frameNumber));
        MachineEvent data;
        data.faceInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber * 2));
        mFinalState = States::face;
    }
};

class TestIdleToGaper2 : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::idle;
        MachineEvent initMachineEvent;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(initMachineEvent, frameNumber));
        MachineEvent data;
        data.movementInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
        mFinalState = States::gaper;
    }
};

class TestIdleToFace : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::idle;
        MachineEvent initMachineEvent;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(initMachineEvent, frameNumber));
        MachineEvent data;
        data.faceInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
        mFinalState = States::face;
    }
};

class TestFaceToReady : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::face;
        MachineEvent initMachineEvent;
        initMachineEvent.faceInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(initMachineEvent, frameNumber));
        MachineEvent data;
        data.faceInGZ = true;
        data.faceInCZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
        mFinalState = States::ready;
    }
};

class TestFaceToGaper: public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::face;
        MachineEvent initMachineEvent;
        initMachineEvent.faceInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(initMachineEvent, frameNumber));
        MachineEvent data;
        data.movementInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
        mFinalState = States::gaper;
    }
};

class TestFaceToGaper2 : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::face;
        MachineEvent initMachineEvent;
        initMachineEvent.faceInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(initMachineEvent, frameNumber));
        MachineEvent data;
        data.objectInCZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
        mFinalState = States::gaper;
    }
};

class TestFaceToIdle : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::face;
        MachineEvent initMachineEvent;
        initMachineEvent.faceInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int> (initMachineEvent, frameNumber));
        mFinalState = States::idle;
        MachineEvent data;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
    }
};

class TestFaceToControl : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::face;
        MachineEvent initMachineEvent;
        initMachineEvent.faceInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(initMachineEvent, frameNumber));
        MachineEvent data;
        data.faceInCZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
        mFinalState = States::ready;
    }
};

class TestGaperToIdle : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::gaper;
        mFinalState = States::idle;
        MachineEvent initMachineEvent;
        initMachineEvent.movementInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(initMachineEvent, frameNumber));
        MachineEvent data;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
    }
};

class TestGaperToFace : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::gaper;
        MachineEvent initMachineEvent;
        initMachineEvent.movementInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(initMachineEvent, frameNumber));
        MachineEvent data;
        data.faceInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
        mFinalState = States::face;
    }
};

class TestReadyToFace : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::ready;
        MachineEvent data;
        data.faceInGZ = true;
        data.handInCW = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
        mFinalState = States::face;
    }
};

class TestReadyToControl : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::ready;
        MachineEvent initMachineEvent;
        initMachineEvent.faceInCZ = true;
        initMachineEvent.faceInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int> (initMachineEvent, frameNumber));
        MachineEvent data;
        data.handInCW = true;
        data.faceInCZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
        mFinalState = States::control;
    }
};

class TestReadyToGaper : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::ready;
        MachineEvent initMachineEvent;
        initMachineEvent.faceInCZ = true;
        initMachineEvent.faceInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int> (initMachineEvent, frameNumber));
        MachineEvent data;
        data.handInCW = true;
        data.movementInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
        mFinalState = States::gaper;

    }
};

class TestReadyToIdle : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::ready;
        MachineEvent initMachineEvent;
        initMachineEvent.faceInCZ = true;
        initMachineEvent.faceInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int> (initMachineEvent, frameNumber));
        mFinalState = States::idle;
        MachineEvent data;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
    }
};

class TestReadyToReady : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::ready;
        MachineEvent data;
        data.faceInCZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
        mFinalState = States::ready;
    }
};

class TestFaceToFace : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::face;
        MachineEvent data;
        data.faceInGZ = true;
        data.handInCW = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
        mFinalState = States::face;
    }
};

class TestGaperToGaper : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::gaper;
        MachineEvent initMachineEvent;
        initMachineEvent.movementInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(initMachineEvent, frameNumber));
        MachineEvent data;
        data.objectInCZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
        mFinalState = States::gaper;
    }
};

class TestGaperToReady : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::gaper;
        MachineEvent initMachineEvent;
        initMachineEvent.movementInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(initMachineEvent, frameNumber));
        MachineEvent data;
        data.faceInCZ = true;
        data.faceInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
        MachineEvent data1;
        data1.faceInCZ = true;
        data1.faceInGZ = true;
        data1.handInCW = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data1, frameNumber));
        mFinalState = States::ready;
    }
};

class TestReadyToControl2 : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::ready;
        MachineEvent data;
        data.faceInCZ = true;
        data.handInCW = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
        MachineEvent newMachineEvent;
        newMachineEvent.objectInCZ = true;
        newMachineEvent.handInCW = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(newMachineEvent, frameNumber));
        MachineEvent data2;
        data2.objectInCZ = true;
        data2.handInCW = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data2, frameNumber));
        mFinalState = States::control;
    }
};

class TestControlToGaper2 : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::control;
        mFinalState = States::gaper;
        MachineEvent initMachineEvent;
        initMachineEvent.faceInGZ = true;
        initMachineEvent.objectInCZ = true;
        initMachineEvent.handInCW = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(initMachineEvent, frameNumber));
        MachineEvent data;
        data.handInCW = true;
        data.faceInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int> (data, frameNumber));
    }
};

class TestControlToFace2 : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::control;
        MachineEvent data;
        MachineEvent initMachineEvent;
        initMachineEvent.faceInGZ = true;
        initMachineEvent.objectInCZ = true;
        initMachineEvent.handInCW = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(initMachineEvent, frameNumber));
        data.handInCW = true;
        data.faceInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int> (data, frameNumber));
        MachineEvent data1;
        data1.faceInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int> (data1, frameNumber));
        mFinalState = States::face;
    }
};

class TestFaceToFace2 : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::face;
        MachineEvent data;
        data.faceInCZ = true;
        data.faceInGZ = true;
        data.handInCW = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int> (data, frameNumber));
        MachineEvent data1;
        data1.faceInGZ = true;
        data1.handInCW = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int> (data1, frameNumber));
        mFinalState = States::face;
    }
};

class TestGaperToGaper2 : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::gaper;
        mFinalState = States::gaper;
        MachineEvent data1;
        data1.faceInCZ = true;
        data1.faceInGZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data1, frameNumber));
        MachineEvent data2;
        data2.faceInCZ = true;
        data2.faceInGZ = true;
        data2.handInCW = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data2, frameNumber));
        MachineEvent data3;
        data3.objectInCZ = true;
        data3.handInCW = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data3, frameNumber));
    }
};

class TestFalsePosControl : public Test
{
public:
    TestFalsePosControl() : Test() {}

protected:
    virtual void setMachineEvent()
    {

        mInitState = States::control;
        mFinalState = States::face; //FIXME:: decide something about timeouts. Maybe state shoul be control
        for (int i = 0; i < 40; i ++)
        {
            MachineEvent data1;
            data1.objectInCZ = true;
            data1.handInCW = true;
            MachineEvent data2;
            data2.faceInGZ = true;
            data2.objectInCZ = true;
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(data1, 1));
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(data2, 1));
        }
    }
};

class TestGaperToFaceToGaper : public Test
{
public:
    TestGaperToFaceToGaper() : Test() {}

protected:
    virtual void setMachineEvent()
    {
        mInitState = States::gaper;
        mFinalState = States::gaper;

        MachineEvent data;
        data.movementInGZ = true;

        MachineEvent incorrectMachineEvent;
        incorrectMachineEvent.faceInGZ = true;

        for (int i = 0; i < 5; i++)
        {
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, 4));
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(incorrectMachineEvent, 1));
        }

        for (int i = 0; i < 5; i++)
        {
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, 2));
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(incorrectMachineEvent, 10));
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, 1));
        }

        for (int i = 0; i < 5; i++)
        {
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(incorrectMachineEvent, 2));
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, 10));
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(incorrectMachineEvent, 1));
        }
    }
};


class TestGaperToFace2 : public Test
{
public:
    TestGaperToFace2() : Test() {}

protected:
    virtual void setMachineEvent()
    {
        mInitState = States::gaper;
        mFinalState = States::face;
        for (int i = 0; i < 40; i ++)
        {
            MachineEvent data1;
            data1.faceInGZ = true;
            MachineEvent data2;
            data2.movementInGZ = true;
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(data1, 1));
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(data2, 1));
        }
    }
};

class TestFaceToFace3 : public Test
{
public:
    TestFaceToFace3() : Test() {}

protected:
    virtual void setMachineEvent()
    {
        mInitState = States::face;
        mFinalState = States::face;
        for (int i = 0; i < 40; i ++)
        {
            MachineEvent data1;
            data1.faceInGZ = true;
            MachineEvent data2;
            data2.movementInGZ = true;
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(data1, 1));
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(data2, 1));
        }
    }
};

class TestReadyToFace2 : public Test
{
public:
    TestReadyToFace2() : Test() {}

protected:
    virtual void setMachineEvent()
    {
        mInitState = States::ready;
        mFinalState = States::face;
        for (int i = 0; i < 40; i ++)
        {
            MachineEvent data1;
            data1.faceInGZ = true;
            MachineEvent data2;
            data2.faceInCZ = true;
            data2.faceInGZ = true;
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(data1, 1));
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(data2, 1));
        }
    }
};

class TestFaceToFace4 : public Test
{
public:
    TestFaceToFace4() : Test() {}

protected:
    virtual void setMachineEvent()
    {
        mInitState = States::face;
        mFinalState = States::face;
        for (int i = 0; i < 40; i ++)
        {
            MachineEvent data1;
            data1.faceInGZ = true;
            MachineEvent data2;
            data2.faceInCZ = true;
            data2.faceInGZ = true;
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(data1, 1));
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(data2, 1));
        }
    }
};

class TestReadyToReady2 : public Test
{
public:
    TestReadyToReady2() : Test() {}

protected:
    virtual  void setMachineEvent()
    {
        mInitState = States::ready;
        mFinalState = States::ready;
        for (int i = 0; i < 40; i ++)
        {
            MachineEvent data1;
            data1.faceInCZ = true;
            data1.faceInGZ = true;
            MachineEvent data2;
            data2.faceInCZ = true;
            data2.faceInGZ = true;
            data2.handInCW = true;
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(data1, 1));
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(data2, 1));
        }
    }
};

class TestReadyToGaper2 : public Test
{
public:
    TestReadyToGaper2() : Test() {}

protected:
    virtual void setMachineEvent()
    {
        mInitState = States::ready;
        mFinalState = States::gaper; // FIXME:: maybe stay at ready or go to gaper???
        for (int i = 0; i < 60; i ++)
        {
            MachineEvent data1;
            data1.faceInCZ = true;
            data1.faceInGZ = true;
            MachineEvent data2;
            data2.objectInCZ = true;
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(data1, 1));
            mMachineEventFlow.push_back(QPair<MachineEvent, int>(data2, 1));
        }
    }
};

class TestControlFalsePosistive : public Test
{
public:
    TestControlFalsePosistive() : Test() { }

protected:
    virtual void setMachineEvent()
    {
        mInitState = States::control;
        mFinalState = States::control;
        MachineEvent data;
        data.objectInCZ = true;
        data.handInCW = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
        MachineEvent incorrectMachineEvent;
        incorrectMachineEvent.handInCW = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(incorrectMachineEvent, 3));
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(data, frameNumber));
    }
};

class TestIdleFalsePos : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::idle;
        mFinalState = States::idle;
        MachineEvent initMachineEvent;
        mMachineEventFlow.push_back(QPair<MachineEvent, int> (initMachineEvent, frameNumber));
        MachineEvent data;
        data.objectInCZ = true;
        data.faceInGZ = true;
        data.faceInCZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int> (data, frameNumber));
    }
};

class TestReadyToGaper3 : public Test
{
protected:
    virtual void setMachineEvent()
    {
        mInitState = States::ready;
        mFinalState = States::gaper;
        MachineEvent initMachineEvent;
        initMachineEvent.faceInCZ = true;
        initMachineEvent.faceInGZ = true;
        MachineEvent falseMachineEvent;
        falseMachineEvent.faceInCZ = true;
        falseMachineEvent.faceInGZ = true;
        falseMachineEvent.handInCW = true;
        MachineEvent curMachineEvent;
        curMachineEvent.handInCW = true;
        curMachineEvent.objectInCZ = true;
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(initMachineEvent, frameNumber));
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(falseMachineEvent, 2));
        mMachineEventFlow.push_back(QPair<MachineEvent, int>(curMachineEvent, frameNumber));
    }
};


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    vector<Test *> tests;
    tests.push_back(new TestIdleFalsePos());
    tests.push_back(new TestReadyToGaper3());
    tests.push_back(new TestReadyToGaper2());
    tests.push_back(new TestReadyToReady2());
    tests.push_back(new TestFaceToFace4());
    tests.push_back(new TestReadyToFace2());
    tests.push_back(new TestGaperToFace2());
    tests.push_back(new TestFaceToFace3());
    tests.push_back(new TestFalsePosControl());
    tests.push_back(new TestControlFalsePosistive());
    tests.push_back(new TestGaperToGaper2());
    tests.push_back(new TestFaceToFace2());
    tests.push_back(new TestControlToFace2());
    tests.push_back(new TestReadyToControl2());
    tests.push_back(new TestControlToGaper2());
    tests.push_back(new TestReadyToFace());
    tests.push_back(new TestReadyToControl());
    tests.push_back(new TestReadyToGaper());
    tests.push_back(new TestReadyToIdle());
    tests.push_back(new TestFaceToReady());
    tests.push_back(new TestFaceToGaper());
    tests.push_back(new TestFaceToGaper2());
    tests.push_back(new TestFaceToIdle());
    tests.push_back(new TestFaceToControl());
    tests.push_back(new TestControlToControl());
    tests.push_back(new TestControlToGapZone());
    tests.push_back(new TestControlToIdle());
    tests.push_back(new TestControlToReady());
    tests.push_back(new TestControlToFace());
    tests.push_back(new TestGaperToIdle());
    tests.push_back(new TestGaperToFace());
    tests.push_back(new TestIdleToGaper());
    tests.push_back(new TestIdleToGaper2());
    tests.push_back(new TestIdleToFace());
    tests.push_back(new TestGaperToReady());
    for (unsigned i = 0; i < tests.size() - 1; i ++)
    {
        QObject::connect(tests.at(i), SIGNAL(testFinished()), tests.at(i + 1), SLOT(runTest()));
    }
    if (!tests.empty())
    {
        tests.at(0)->runTest();
    }

    cout << "PASSED\n";

    return a.exec();
}
