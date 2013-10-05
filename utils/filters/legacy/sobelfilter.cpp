#include "sobelfilter.h"

QString SobelFilter::name = QString("Sobel");

SobelFilter::SobelFilter(QWidget *parent)
    : BaseFilter(parent)
{
    ui.setupUi(this);
    connect(ui.xButton, SIGNAL(toggled(bool)), this, SLOT(parametersChangedSlot()));
    connect(ui.yButton, SIGNAL(toggled(bool)), this, SLOT(parametersChangedSlot()));
    connect(ui.bothButton, SIGNAL(toggled(bool)), this, SLOT(parametersChangedSlot()));

}

void SobelFilter::parametersChangedSlot( void )
{
    emit parametersChanged();
}

G12Buffer *SobelFilter::filter (G12Buffer *input)
{
    //G12Buffer *toReturn = g12BufferClone(input);

/*    if (ui.xButton->isChecked())
        return g12BufferSobelX(input);

    if (ui.yButton->isChecked())
        return g12BufferSobelY(input);

    if (ui.bothButton->isChecked())
        return g12BufferSobelMag(input, 8);*/

//    g12BufferGainOffset(toReturn, gParams.gain, gParams.offset);
    //return toReturn;
    return new G12Buffer(input);
}

SobelFilter::~SobelFilter()
{

}
