#ifndef MATRIXWIDGET_H
#define MATRIXWIDGET_H

#include <QWidget>
#include "core/math/matrix/matrix.h"
#include "ui_matrixwidget.h"
#include "core/math/projectiveTransform.h"

using namespace corecvs;

class MatrixWidget : public QWidget
{
    Q_OBJECT

public:
    MatrixWidget(QWidget *parent = 0);
    ~MatrixWidget();

    void setProjectiveTransform(ProjectiveTransform *F);
    void setMatrix33(Matrix33 *F);


private:
    Ui_MatrixWidgetClass ui;
    Matrix *M;
    QDoubleSpinBox* boxes[9];
};

#endif // MATRIXWIDGET_H
