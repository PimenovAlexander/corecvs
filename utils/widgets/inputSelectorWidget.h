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

public slots:
    void updateInputString();


    void browseAvi();
    void browseFile();

    void v4l2Refresh();
signals:
    void newInputString();

private:
    Ui::InputSelectorWidgetClass ui;
};

#endif // INPUTSELECTORWIDGET_H
