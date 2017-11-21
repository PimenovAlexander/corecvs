#pragma once

/**
 * \file contentStatistics.h
 *
 * \date Mar 30, 2013
 **/

#include <QtCore/QObject>
#include <QtCore/QtDebug>

#include "core/utils/preciseTimer.h"
#include "core/stats/calculationStats.h"

using corecvs::PreciseTimer;
using corecvs::SingleStat;
using corecvs::BaseTimeStatisticsCollector;

class ContentStatistics : public QObject, public BaseTimeStatisticsCollector
{
    Q_OBJECT
public:

    static const int EMPTY_ID = -1;
    int currentId;
    QString name;

    PreciseTimer timer;

    ContentStatistics() :
        currentId(EMPTY_ID)
    {

    }

    QString oldStyleNameResolve()
    {
        QString name("");
        QString postfix("");
        if (currentId != EMPTY_ID)
        {
            if (currentId >= 1000 && currentId < 2000)
            {
                postfix = " (details)";
                currentId = currentId - 1000;
            }

            switch(currentId) {
            case    0: name = "Dress 999 RUR";                    break;
            case    1: name = "Trousers (dark red) 1499 RUR";     break;
            case    2: name = "Shoes (dark red) 1999 RUR";        break;
            case    3: name = "Sweater (light red) 1299 RUR";     break;
            case    4: name = "Dress (violet & white) 1499 RUR";  break;
            case    5: name = "Boots (grey) 1499 RUR";            break;
            case    6: name = "Leggins (violet & white) 499 RUR"; break;
            case    7: name = "Boots (brown) 1999 RUR";           break;
            case    8: name = "Trousers (black) 1499 RUR";        break;
            case    9: name = "Jeans Skinny";                     break;
            case   10: name = "Earrings 399 RUR";                 break;
            case   11: name = "Shoes (black) 1499 RUR";           break;
            case   12: name = "Gloves (black)";                   break;
            case   13: name = "Collar 2499 RUR";                  break;
            case 2000: name = "Where to buy map";                 break;
            case 1000000: name = "Intro Movie";                   break;

            default: name = "OTHER";                            break;
            }

            name += postfix;
        }
        return name;
    }

public slots:

    void transition(int newId, QString name)
    {
        //long timestamp = 0;
        qDebug() << "New content: " << newId  << " " << name;
        /* Could be moved comewhere */
        //QString postfix = "";

        /* If no name use old style H&M*/
        if (name.isEmpty())
        {
            name = oldStyleNameResolve();
        }
        PreciseTimer currentTimer = PreciseTimer::currentTime();
        //long timestamp = currentTimer.usecs(); //timer.usecsTo();
        addSingleStat(name.toStdString(), SingleStat(timer.isCorrect() ? timer.usecsTo(currentTimer) : 0));
        emit contentTransitionRegistered(currentTimer.usec(), newId, name);

        currentId = newId;
        timer = currentTimer;

    }

signals:
    void contentTransitionRegistered(long timestamp, int contentId, QString contentName);

};

/* EOF */
