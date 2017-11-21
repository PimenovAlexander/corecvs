#ifndef SCANNERCONTROL_H
#define SCANNERCONTROL_H

#include "QObject"
#include "QString"
#include <QtSerialPort/QSerialPort>
#include "core/utils/global.h"

class ScannerControl : public QObject
{
    Q_OBJECT
public:
    ScannerControl();

private:
    bool sendCommand(QString command);

public slots:
    bool openDevice(QString name);
    bool laserOn();
    bool laserOff();
    bool step(qint64 dist);
    //int getPos();
    //int getRealPos();
    void close();
    bool  home();
    //bool end();

private slots:
    void getMessage();
    void setLock();

signals:
    void endOfMove();
    //void homeStop(qint64 pointsLeft);
    //void endStop(qint64 pointsLeft);


private:
    QSerialPort *port;
    qreal pos;
    int dir;
public:
    const qint32 stepsOneRound = 200;
    bool lock;

};


#endif // SCANNERCONTROL_H
