#pragma once
#include <QWidget>
#include "core/xml/generated/chessBoardAssemblerParamsBase.h"
#include "ui_chessBoardAssemblerParamsBaseControlWidget.h"
#include "corestructs/parametersControlWidgetBase.h"


namespace Ui {
    class ChessBoardAssemblerParamsBaseControlWidget;
}

class ChessBoardAssemblerParamsBaseControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit ChessBoardAssemblerParamsBaseControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~ChessBoardAssemblerParamsBaseControlWidget();

    ChessBoardAssemblerParamsBase* createParameters() const;
    void getParameters(ChessBoardAssemblerParamsBase &param) const;
    void setParameters(const ChessBoardAssemblerParamsBase &input);
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
    Ui_ChessBoardAssemblerParamsBaseControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

