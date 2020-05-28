#pragma once
#include <QtCore/QDebug>
#include <QWidget>

#include "generatedParameters/presentationParameters.h"
#include "ui_presentationParametersControlWidget.h"
#include "corestructs/parametersControlWidgetBase.h"

namespace Ui {
    class PresentationParametersControlWidget;
}

class PresentationParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit PresentationParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~PresentationParametersControlWidget();

    PresentationParameters* createParameters() const;
    void getParameters(PresentationParameters &params) const;
    void setParameters(const PresentationParameters &input);
    virtual void setParametersVirtual(void *input);


    virtual void loadParamWidget(WidgetLoader &loader);
    virtual void saveParamWidget(WidgetSaver  &saver);

    Ui::PresentationParametersControlWidget *ui()
    {
        return mUi;
    }

public slots:
    void changeParameters()
    {
        // emit paramsChanged();
    }

signals:
    void valueChanged();
    void paramsChanged();



private:
    Ui::PresentationParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};


