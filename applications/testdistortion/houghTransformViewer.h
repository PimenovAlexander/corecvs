#pragma once

#include <vector>
#include <QtGui/QWidget>
#include <QtGui/QtGui>
#include "advancedImageWidget.h"

using std::vector;

class HoughTransformViewer : public AdvancedImageWidget
{
    Q_OBJECT
public slots:
    virtual void childMouseMoved   (QMouseEvent  *);
signals:
    void pointSelected(QPoint point);
};
