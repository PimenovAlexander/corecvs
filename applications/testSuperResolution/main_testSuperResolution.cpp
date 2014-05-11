/**
 * \file main_testSuperResolution.cpp
 *
 * \date  Nov 28, 2013
 **/

#include <string>
#include <iostream>

#include <QtGui/QtGui>
#include <QtGui/QApplication>

#include <QtGui/QPolygon>


#include "global.h"
#include "utils.h"

#include "testSuperResolutionMainWindow.h"
#include "qtFileLoader.h"



using namespace std;
using namespace corecvs;

void testPolygon()
{
    QPolygonF pol1;
    QPolygonF pol2;

    pol1 << QPointF(0, 0) << QPointF(0, 2) << QPointF(2, 2) << QPointF(2, 0);
    pol2 << QPointF(1, 1) << QPointF(1, 3) << QPointF(3, 3) << QPointF(3, 1);
    QPolygonF inter = pol1.intersected(pol2);

    qDebug() << inter;

    double res = 0;
    for(int i = 0; i < inter.count() - 1; i++)
    {
        res += ((double)inter.at(i).x() + (double)inter.at(i + 1).x()) * ((double)inter.at(i).y() - (double)inter.at(i + 1).y());
    }
    qDebug() <<  "Area:" << std::abs(res) / 2.0;

}

int main(int argc, char *argv[])
{
    QTFileLoader::registerMyself();
    setSegVHandler();
    setStdTerminateHandler();

    printf("Starting testSuperResolution...\n");

    testPolygon();

    QApplication app(argc, argv);
    TestSuperResolutionMainWindow mainWindow;

    app.exec();

    printf("Exiting testSuperResolution\n");
    return 0;

}


