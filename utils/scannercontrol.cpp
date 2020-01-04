#include "scannercontrol.h"
#include <QDebug>
#include <QThread>
#include <fstream>

//std::ofstream a("arduinooutput.txt");

ScannerControl::ScannerControl()
{
    port = new QSerialPort();
    lock = false;
    //pos = 1000;
    connect(port, SIGNAL(readyRead()), this, SLOT(getMessage()));
    connect(this, SIGNAL(endOfMove()), this, SLOT(setLock()));
}
bool ScannerControl::openDevice(QString name)
{

    port->setPortName(name);
    if (port->open(QIODevice::OpenModeFlag::ReadWrite))
    {
        port->setBaudRate(QSerialPort::Baud9600);
        port->setDataBits(QSerialPort::Data8);
        port->setParity(QSerialPort::NoParity);
        port->setStopBits(QSerialPort::OneStop);
        port->setFlowControl(QSerialPort::NoFlowControl);
        port->setDataTerminalReady(false);
        port->setRequestToSend(false);
        port->flush();
        //QThread::msleep(1000);
        //sendCommand("HOME\n");
        return true;
    }
    return false;
}


void ScannerControl::close()
{
    if(port->isOpen())
        port->close();
}
bool ScannerControl::home()
{
    lock = true;
    //while (pos);
    return sendCommand("HOME\n");
}

bool ScannerControl::laserOn()
{
    return sendCommand("LASER_ON\n");
}

bool ScannerControl::laserOff()
{
    return sendCommand("LASER_OFF\n");
}

void ScannerControl::setLock()
{
    lock = false;
}

/*
 * negative argument moves back, positive - forward
 */
bool ScannerControl::step(qint64 dist)
{
    lock = true;
    QString command;
    command.sprintf("MOVE %i\n",int(quint64(abs(dist))) );
    qDebug()<<command;
    if(!sendCommand(dist < 0 ? "BACK\n" : "FORWARD\n"))
        return false;
    if(!sendCommand(command))
        return false;
    //pos += dist;
    return true;
}

bool ScannerControl::sendCommand(QString command)
{
    if(port->isOpen())
       if(port->write(command.toLatin1().data()))
       {
           port->flush();
           return true;
       }
    return false;
}

/*int ScannerControl::getPos()
{
    return pos;
}*/

void ScannerControl::getMessage()
{
    SYNC_PRINT(("dada"));
    QString message = port->readAll();
    port->clear();
    qDebug() << message;
    //a << message.toLatin1().data();
    if(message.contains("STOP", Qt::CaseInsensitive))
    {
        SYNC_PRINT(("FUCK"));
        emit endOfMove();
        lock = false;
    }
}
