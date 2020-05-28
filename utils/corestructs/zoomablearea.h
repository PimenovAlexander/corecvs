#ifndef ZOOMABLEAREA_H
#define ZOOMABLEAREA_H

#include <QWidget>
#include "ui_zoomablearea.h"

class ZoomableArea : public QWidget
{
    Q_OBJECT

public:
    ZoomableArea(QWidget *parent = 0);
    ~ZoomableArea();

private:
    Ui::ZoomableAreaClass ui;
};

#endif // ZOOMABLEAREA_H
