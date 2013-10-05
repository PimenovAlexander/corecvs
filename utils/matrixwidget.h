#ifndef MATRIXWIDGET_H
#define MATRIXWIDGET_H

#include <QtGui/QWidget>
#include "matrix.h"
#include "ui_matrixwidget.h"
#include "projectiveTransform.h"

using namespace corecvs;

class MatrixWidget : public QWidget
{
    Q_OBJECT

public:
    MatrixWidget(QWidget *parent = 0);
    ~MatrixWidget();

    void setProjectiveTransform(ProjectiveTransform *F);


private:
    Ui_MatrixWidgetClass ui;
    Matrix *M;
    QDoubleSpinBox* boxes[9];
};

#endif // MATRIXWIDGET_H
