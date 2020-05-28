#pragma once
#include <QWidget>
#include "core/xml/generated/chessBoardCornerDetectorParamsBase.h"
#include "ui_chessBoardCornerDetectorParamsBaseControlWidget.h"
#include "corestructs/parametersControlWidgetBase.h"


namespace Ui {
    class ChessBoardCornerDetectorParamsBaseControlWidget;
}

class ChessBoardCornerDetectorParamsBaseControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit ChessBoardCornerDetectorParamsBaseControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~ChessBoardCornerDetectorParamsBaseControlWidget();

    ChessBoardCornerDetectorParamsBase* createParameters() const;
    void getParameters(ChessBoardCornerDetectorParamsBase &param) const;
    void setParameters(const ChessBoardCornerDetectorParamsBase &input);
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
    Ui_ChessBoardCornerDetectorParamsBaseControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

