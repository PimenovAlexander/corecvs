#pragma once
#include <QWidget>
#include "core/xml/generated/bitcodeBoardParamsBase.h"
#include "ui_bitcodeBoardParamsBaseControlWidget.h"
#include "parametersControlWidgetBase.h"


namespace Ui {
    class BitcodeBoardParamsBaseControlWidget;
}

class BitcodeBoardParamsBaseControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit BitcodeBoardParamsBaseControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~BitcodeBoardParamsBaseControlWidget();

    BitcodeBoardParamsBase* createParameters() const;
    void getParameters(BitcodeBoardParamsBase &param) const;
    void setParameters(const BitcodeBoardParamsBase &input);
    virtual void setParametersVirtual(void *input);
    
    virtual void loadParamWidget(WidgetLoader &loader);
    virtual void saveParamWidget(WidgetSaver  &saver);
    
public slots:
    void changeParameters()
    {
        // emit paramsChanged();
    }

signals:
    void valueChanged();
    void paramsChanged();

private:
    Ui_BitcodeBoardParamsBaseControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

