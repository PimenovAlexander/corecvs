#pragma once
#include <QtGui/QWidget>
#include "generated/rgbColorParameters.h"
#include "ui_rgbColorParametersControlWidget.h"
#include "parametersControlWidgetBase.h"
#include "rgbColor.h"


namespace Ui {
    class RgbColorParametersControlWidget;
}

class RgbColorParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT
    QColorDialog mColorPicker;
    QColor color;

public:
    explicit RgbColorParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~RgbColorParametersControlWidget();

    RgbColorParameters* createParameters() const;
    void setParameters(const RgbColorParameters &input);
    virtual void setParametersVirtual(void *input);

    virtual void loadParamWidget(WidgetLoader &loader);
    virtual void saveParamWidget(WidgetSaver  &saver);

    RGBColor getColor()
    {
        return RGBColor(
          mUi->rSpinBox->value()
        , mUi->gSpinBox->value()
        , mUi->bSpinBox->value());
    }

public slots:
    void changeParameters()
    {
        // emit paramsChanged();
    }

    void updateIcon()
    {
        //mUi->selectorPushButton->setS;
    }

    void pickerClicked(QMouseEvent * /*event*/)
    {
        mColorPicker.show();

        bool signalState = mColorPicker.blockSignals(true);
        mColorPicker.setCurrentColor(QColor(
                mUi->rSpinBox->value(),
                mUi->gSpinBox->value(),
                mUi->bSpinBox->value()
        ));
        mColorPicker.blockSignals(signalState);

        connect(&mColorPicker, SIGNAL(currentColorChanged (QColor)),
                this, SLOT(colorSelected(QColor)), Qt::UniqueConnection);
    }

    void colorSelected(const QColor &color)
    {
        mUi->rSpinBox->setValue(color.red());
        mUi->gSpinBox->setValue(color.green());
        mUi->bSpinBox->setValue(color.blue());
        mUi->pickerWidget->update();
    }

    void repaintPicker(QPaintEvent * /*event*/, QWidget *who)
    {
        color = QColor(
            mUi->rSpinBox->value(),
            mUi->gSpinBox->value(),
            mUi->bSpinBox->value()
        );

        QPainter painter(who);
        //painter.setPen(color);

        painter.fillRect(who->rect(),color);
    }

signals:
    void valueChanged();
    void paramsChanged();

private:
    Ui_RgbColorParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

