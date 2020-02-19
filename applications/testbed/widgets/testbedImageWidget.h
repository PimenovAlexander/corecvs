#pragma once

/**
 * \file testbedImageWidget.h
 *
 * \date Dec 2, 2013
 **/

#include "uis/advancedImageWidget.h"

class TestbedImageWidget: public AdvancedImageWidget
{
public:
    TestbedImageWidget(QWidget *parent = 0, bool showHeader = true);
    virtual ~TestbedImageWidget(){};

public slots:
    virtual void childRepaint(QPaintEvent *event, QWidget *who);
    virtual void childMouseMoved   (QMouseEvent  *);

public:
    QPoint mousePoint;
    bool drawDecoration;
    int decorationSize;

    QRect getInputRect()
    {
        return mInputRect;
    }

    int getImageRadius();
};

/* EOF */
