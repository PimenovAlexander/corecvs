#pragma once
/**
 * \file AviTGListBase.h
 * \brief Add Comment Here
 *
 * \date Jan 9, 2015
 * \author alexander
 */

#include "aviListBase.h"

#include <QDateTime>

class AviTGListBase : public AviListBase
{
    static bool avCodecInited;
    QString path;
    QString prefix;
    /*
    int year;
    int month;
    int day;
    int hour;
    int minute;*/
    QDateTime timemark;

    int frameCount;
public:
    AviTGListBase(QString const &params);

    void parseFileName(QString const &fileName);
    virtual ~AviTGListBase();

    virtual int getRawFrameFromAVI();
    virtual string getNextFile();
};
