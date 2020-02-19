#ifndef INPUTSELECTORWIDGET_H
#define INPUTSELECTORWIDGET_H

#include <QWidget>
#include <corestructs/parametersControlWidgetBase.h>
#include "ui_inputSelectorWidget.h"

class InputSelectorWidget : public QWidget, public SaveableWidget
{
    Q_OBJECT

public:
    int autoInit;
    QString rootPath;

    InputSelectorWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~InputSelectorWidget();

    QString getInputString();
    void setInputString(const QString &str);

    virtual void loadParamWidget(WidgetLoader &loader);
    virtual void saveParamWidget(WidgetSaver  &saver);

    virtual void loadFromQSettings(const QString &fileName, const QString &_root);
    virtual void saveToQSettings  (const QString &fileName, const QString &_root);

public slots:
    void fpsIndexChanged (int i);
    void sizeIndexChanged(int i);
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
