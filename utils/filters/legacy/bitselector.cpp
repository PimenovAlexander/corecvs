#include "bitselector.h"
#include "core/buffers/commonMappers.h"

QString BitSelector::name = QString("Bit Selector");

#define SET_BOX_ARRAY(X) bitBoxes[X] = ui.bit##X

BitSelector::BitSelector(QWidget *parent)
    : BaseFilter(parent)
{
    ui.setupUi(this);

    SET_BOX_ARRAY(0);  SET_BOX_ARRAY(1);   SET_BOX_ARRAY(2);  SET_BOX_ARRAY(3);
    SET_BOX_ARRAY(4);  SET_BOX_ARRAY(5);   SET_BOX_ARRAY(6);  SET_BOX_ARRAY(7);
    SET_BOX_ARRAY(8);  SET_BOX_ARRAY(9);   SET_BOX_ARRAY(10); SET_BOX_ARRAY(11);

    mask = 0;
    for (int i = 0; i < 12; i++)
    {
        bitBoxes[i]->setChecked(true);
        mask |= (1 << i);
        connect(bitBoxes[i], SIGNAL(stateChanged(int)), this, SLOT(maskChanged()));
    }
    shift = 0;
    connect(ui.shiftBox, SIGNAL(valueChanged(int)), this, SLOT(shiftChanged()));

}

void BitSelector::maskChanged( void )
{
    mask = 0;
    for (int i = 0; i < 12; i++)
    {
        mask |= bitBoxes[i]->isChecked() ? (1 << i) : 0;
    }
    emit parametersChanged();
}

void BitSelector::shiftChanged( void )
{
    shift = ui.shiftBox->value();
    emit parametersChanged();
}


G12Buffer *BitSelector::filter (G12Buffer *input)
{
    G12Buffer *toReturn = new G12Buffer(input);
    ShiftMaskMapper smm(mask, shift);
    toReturn->mapOperationElementwize<ShiftMaskMapper>(smm);
    return toReturn;
}


BitSelector::~BitSelector()
{

}
