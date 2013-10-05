#include <matrix33.h>
#include <projectiveTransform.h>
#include <g12Buffer.h>

#include "scalefilter.h"


QString ScaleFilter::name = QString("Scale");

ScaleFilter::ScaleFilter(QWidget *parent)
    : BaseFilter(parent)
{
    ui.setupUi(this);

    connect(ui.heightBox, SIGNAL(valueChanged(int)), this, SLOT(updateHeight(int)));
    connect(ui.widthBox,  SIGNAL(valueChanged(int)), this, SLOT(updateWidth(int)));

    connect(ui.scaleXBox, SIGNAL(valueChanged(double)), this, SLOT(updateXScale(double)));
    connect(ui.scaleYBox, SIGNAL(valueChanged(double)), this, SLOT(updateYScale(double)));

    connect(ui.shiftXBox, SIGNAL(valueChanged(int)), this, SLOT(updateXShift(int)));
    connect(ui.shiftXBox, SIGNAL(valueChanged(int)), this, SLOT(updateYShift(int)));

}

void ScaleFilter::updateXScale (double /*value*/)
{
    emit parametersChanged();
}

void ScaleFilter::updateYScale (double /*value*/)
{
    emit parametersChanged();
}

void ScaleFilter::updateHeight (int /*val*/)
{
    emit parametersChanged();
}

void ScaleFilter::updateWidth  (int /*val*/)
{
    emit parametersChanged();
}

void ScaleFilter::updateXShift (int /*val*/)
{
    emit parametersChanged();
}

void ScaleFilter::updateYShift (int /*val*/)
{
    emit parametersChanged();
}



G12Buffer *ScaleFilter::filter (G12Buffer *input)
{
    Matrix33 matrix;
    matrix =
        Matrix33::ShiftProj( ui.shiftXBox->value(), ui.shiftYBox->value()) *
        Matrix33::Scale2(1.0 / ui.scaleXBox->value(), 1.0 / ui.scaleYBox->value());
    ProjectiveTransform transform(matrix);
    int h = ui.heightBox->value();
    int w = ui.widthBox->value();

    if (h == -1) h = input->h * ui.scaleYBox->value();
    if (w == -1) w = input->w * ui.scaleXBox->value();

    G12Buffer *changed = input->doReverseDeformationBl<G12Buffer, ProjectiveTransform> (&transform, h, w);
    return changed;
}

ScaleFilter::~ScaleFilter()
{

}
