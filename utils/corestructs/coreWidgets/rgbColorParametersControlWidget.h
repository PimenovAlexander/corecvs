#pragma once
/**
    rgbColorParametersControlWidget.h
**/
#include <QWidget>
#include <QColorDialog>
#include <QPainter>

#include "core/buffers/rgb24/rgbColor.h"
#include "core/xml/generated/rgbColorParameters.h"

#include "ui_rgbColorParametersControlWidget.h"
#include "corestructs/parametersControlWidgetBase.h"

//#ifndef WIN32
//EM: said that it crashes on her Linux
//TODO: clarify and fix it
//AP: Uncommented. Need this function. Most probably Xwindow/GTK/QT issue
//#define DISABLE_COLOR_PICKER
//#endif

namespace Ui {
    class RgbColorParametersControlWidget;
}

class RgbColorParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT
    QColor color;
#ifndef DISABLE_COLOR_PICKER
    QColorDialog mColorPicker;
#endif

public:
    explicit RgbColorParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~RgbColorParametersControlWidget();

    RgbColorParameters* createParameters() const;
    void setParameters(const RgbColorParameters &input);
    void getParameters(RgbColorParameters &params) const;
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

    void setRGBColor(const RGBColor &color)
    {
        RgbColorParameters params;
        params.setR(color.r());
        params.setG(color.g());
        params.setB(color.b());
        setParameters(params);
    }


    void setEnabled(bool flag)
    {
        mUi->rSpinBox->setEnabled(flag);
        mUi->gSpinBox->setEnabled(flag);
        mUi->bSpinBox->setEnabled(flag);
        mUi->pickerWidget->setEnabled(flag);
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
#ifndef DISABLE_COLOR_PICKER
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
#endif
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

