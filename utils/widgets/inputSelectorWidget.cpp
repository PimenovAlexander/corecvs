#include "inputSelectorWidget.h"

InputSelectorWidget::InputSelectorWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    connect(ui.okPushButton, SIGNAL(pressed()), this, SIGNAL(newInputString()));
}

QString InputSelectorWidget::getInputString()
{
    return ui.inputLineEdit->text();
}

void InputSelectorWidget::setInputString(const QString &str)
{
    ui.inputLineEdit->setText(str);
}


InputSelectorWidget::~InputSelectorWidget()
{

}

