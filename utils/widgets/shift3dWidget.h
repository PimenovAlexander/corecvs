#ifndef SHIFT3DWIDGET_H
#define SHIFT3DWIDGET_H

#include <QWidget>
#include "ui_shift3dWidget.h"

class Shift3dWidget : public QWidget
{
    Q_OBJECT

public:
    Shift3dWidget(QWidget *parent = 0);
    ~Shift3dWidget();

public:
    Ui::Shift3dWidgetClass ui;
};

#endif // SHIFT3DWIDGET_H
