#ifndef LENSDISTORTIOMMODELPARAMETERSCONTOLWIDGET_H
#define LENSDISTORTIOMMODELPARAMETERSCONTOLWIDGET_H

#include <QWidget>

#include "parametersControlWidgetBase.h"
#include "radialCorrection.h"
#include "advancedImageWidget.h"
#include "graphPlotDialog.h"
#include "rgb24Buffer.h"

namespace Ui {
class LensDistortionModelParametersContolWidget;
}

class LensDistortionModelParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit LensDistortionModelParametersControlWidget(QWidget *parent = 0);
    virtual ~LensDistortionModelParametersControlWidget();

    LensDistortionModelParameters* createParameters() const;
    LensDistortionModelParameters getParameters() const;
    void getParameters(LensDistortionModelParameters &params) const;


    void setParameters(const LensDistortionModelParameters &input);
    virtual void setParametersVirtual(void *input);


    virtual void loadParamWidget(WidgetLoader &loader);
    virtual void saveParamWidget(WidgetSaver  &saver);

    enum {
        COLUMN_CAPTION,
        COLUMN_EDIT
    };

public slots:
    void changeParameters()
    {
        // emit paramsChanged();
    }

    void addPower();
    void delPower();
    void resetCx();
    void resetCy();
    void resetP1();
    void resetP2();
    void resetAspect();
    void resetScale();
    void resetNormalize();
    void invertCorrection();
    void compareCorrection();


    void updateAdditionalDataNeeded();
    void updateAdditionalData();

    void exampleShow();
    void loadExample();

    void showGraphDialog();

    void loadParams();
    void saveParams();

signals:
    void valueChanged();
    void paramsChanged();

private:
    Ui::LensDistortionModelParametersContolWidget *ui;
    bool autoInit;
    QString rootPath;

/* Some extended data */

    GraphPlotDialog     mGraphDialog;
    AdvancedImageWidget mDemoDialog;

    QImage *mExample;

    RGB24Buffer *mInput;
    RGB24Buffer *mCorrected;
    RGB24Buffer *mInverse;
    RGB24Buffer *mBackproject;
    RGB24Buffer *mDiff;
    RGB24Buffer *mIsolines;




};

#endif // LENSDISTORTIOMMODELPARAMETERSCONTOLWIDGET_H
