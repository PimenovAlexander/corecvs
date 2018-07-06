#ifndef PARAMETEREDITORWIDGET_H_
#define PARAMETEREDITORWIDGET_H_
/*
 * parameterEditorWidget.h
 *
 *  Created on: Oct 12, 2012
 *      Author: alexander
 */
#include <QString>
#include <QWidget>

class ParameterEditorWidget: public QWidget
{
    Q_OBJECT
public:
    ParameterEditorWidget(QWidget *parent = NULL);
    virtual ~ParameterEditorWidget();

    virtual double minimum (void) = 0;
    virtual double maximum (void) = 0;
    virtual double value   (void) = 0;

    virtual void setValue  (double value) = 0;
    /*virtual void setMinimum(double value) = 0;
    virtual void setMaximum(double value) = 0;
    virtual void setStep   (double value) = 0;*/

    virtual void setAutoSupported(bool value) = 0;

    virtual void stepUp  () = 0;
    virtual void stepDown() = 0;

    virtual void setName (QString name) = 0;
};

#endif /* PARAMETEREDITORWIDGET_H_ */
