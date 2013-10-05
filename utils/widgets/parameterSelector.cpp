
#include "parameterSelector.h"
#include "log.h"


ParameterSelector::ParameterSelector(QWidget *parent)
    : ParameterEditorWidget(parent)
{
	ui.setupUi(this);
    connect(ui.resetButton, SIGNAL(released()), this, SIGNAL(resetPressed()));
}

void ParameterSelector::setName (QString name)
{
    ui.label->setText(name);
}

void ParameterSelector::setAutoSupported(bool value)
{
    ui.autoBox->setEnabled(value);
}

double ParameterSelector::minimum (void)
{
    return mIndexToValue[0];
}

double ParameterSelector::maximum (void)
{
    return mIndexToValue[mIndexToValue.size() - 1];
}

double ParameterSelector::value()
{
    int index = ui.comboBox->currentIndex();
    if (index < (int)mIndexToValue.size())
        return mIndexToValue[index];
    else
        return 0;
}


void ParameterSelector::setValue(double value)
{
    for (unsigned i = 0; i < mIndexToValue.size(); i++ )
    {
        if (mIndexToValue[i] == value)
        {
            ui.comboBox->setCurrentIndex(i);
            return;
        }
    }
    ui.comboBox->setCurrentIndex(0);
}

void ParameterSelector::pushOption(QString name, int value)
{
    ui.comboBox->addItem(name);
    mIndexToValue.push_back(value);
}

void ParameterSelector::stepUp  ()
{
    ui.comboBox->setCurrentIndex(ui.comboBox->currentIndex() + 1);
}

void ParameterSelector::stepDown()
{
    ui.comboBox->setCurrentIndex(ui.comboBox->currentIndex() - 1);
}

void ParameterSelector::currentIndexChanged (int /*value*/)
{
    emit valueChanged(value());
}
/*
void ParameterSelector::regenComboBox()
{
    while (ui.comboBox->count() != 0) {
        ui.comboBox->removeItem(0);
    }
    for (int i = mMin; i <= mMax; i+= mStep)
    {
        ui.comboBox->addItem(QString::number(i));
    }
}
*/
ParameterSelector::~ParameterSelector()
{

}
