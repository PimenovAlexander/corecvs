#include "scannercontrol.h"
#include <QDebug>
#include <QThread>


ScannerControl::ScannerControl()
{
    port = new QSerialPort();
    pos = 0;
    connect(port,SIGNAL(readyRead()),this,SLOT(getMessage()));
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
        QThread::msleep(1000);
        //sendCommand("HOME\n");
        return true;
    }
    return false;
}


void ScannerControl::close(){
    if(port->isOpen())
        port->close();
}
bool ScannerControl::home(){
    return sendCommand("HOME");
    pos = 0;
}

bool ScannerControl::laserOn(){
    return sendCommand("LASER_ON\n");
}

bool ScannerControl::laserOff(){
    return sendCommand("LASER_OFF\n");
}
/*
 * negative argument moves back, positive - forward
 */
bool ScannerControl::step(qreal dist)
{
    QString command;
    command.sprintf("MOVE %i\n",quint64(dist));
    qDebug()<<command;
    if(!sendCommand(dist <0? "BACK\n" : "FORWARD\n"))
        return false;
    if(!sendCommand(command))
        return false;
    pos += dist;
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
int ScannerControl::getPos(){
    return pos;
}


void ScannerControl::getMessage()
{
    QString message = port->readAll();
    qDebug() << message;
    if(message.contains("STOP")){
        emit endOfMove();
    }
}
