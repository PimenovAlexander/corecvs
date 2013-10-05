#ifndef AFFINEFILTER_H
#define AFFINEFILTER_H

#include <QtGui/QWidget>
#include "ui_affinefilter.h"

class AffineFilter : public QWidget
{
    Q_OBJECT

public:
    AffineFilter(QWidget *parent = 0);
    ~AffineFilter();

private:
    Ui_AffineFilterClass ui;
};

#endif // AFFINEFILTER_H
