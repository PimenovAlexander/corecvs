#pragma once

/**
 * \file robodetectImageWidget.h
 *
 * \date Dec 2, 2013
 **/

#include "advancedImageWidget.h"

class RobodetectImageWidget: public AdvancedImageWidget
{
public:
    RobodetectImageWidget(QWidget *parent = 0, bool showHeader = true);
    virtual ~RobodetectImageWidget(){}

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
