#pragma once
#include "../../qt4/host-soft/stateMachine/stateMachine.h"
#include <QTimer>

class Test : public QObject
{
    Q_OBJECT

public:
    Test(QObject *parent = 0);

    //virtual void test() = 0;
    virtual ~Test();

    void setInterval(int interval);

public slots:
    void runTest();

signals:
    void testFinished();

protected:
    virtual void setMachineEvent() = 0;

    QList<QPair<MachineEvent, int> > mMachineEventFlow;
    States::Id mFinalState;
    States::Id mInitState;

private slots:
    void setState();

private:
    QTimer *mTimer;
    int mInterval;
    StateMachine mMachine;

};
