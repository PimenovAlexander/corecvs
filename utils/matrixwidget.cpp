#include "matrixwidget.h"

#define ADD_TO_BOX_ARRAY(X) boxes[X] = ui.box##X


MatrixWidget::MatrixWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    ADD_TO_BOX_ARRAY(0);  ADD_TO_BOX_ARRAY(1);  ADD_TO_BOX_ARRAY(2);
    ADD_TO_BOX_ARRAY(3);  ADD_TO_BOX_ARRAY(4);  ADD_TO_BOX_ARRAY(5);
    ADD_TO_BOX_ARRAY(6);  ADD_TO_BOX_ARRAY(7);    ADD_TO_BOX_ARRAY(8);

    int i;
    for (i = 0; i < 9; i++)
    {
        boxes[i]->setMaximum(1e100);
        boxes[i]->setMinimum(-1e100);
        boxes[i]->setEnabled(false);
    }
}

void MatrixWidget::setProjectiveTransform(ProjectiveTransform *F)
{
    int i;
    for (i = 0; i < 9; i++)
    {
        boxes[i]->setValue(F->element[i]);
    }

    this->update();
}


MatrixWidget::~MatrixWidget()
{

}
