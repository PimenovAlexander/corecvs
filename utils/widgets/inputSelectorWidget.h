#ifndef INPUTSELECTORWIDGET_H
#define INPUTSELECTORWIDGET_H

#include <QWidget>
#include "ui_inputSelectorWidget.h"

class InputSelectorWidget : public QWidget
{
    Q_OBJECT

public:
    InputSelectorWidget(QWidget *parent = 0);
    ~InputSelectorWidget();

    QString getInputString();
    void setInputString(const QString &str);

signals:
    void newInputString();

private:
    Ui::InputSelectorWidgetClass ui;
};

#endif // INPUTSELECTORWIDGET_H
