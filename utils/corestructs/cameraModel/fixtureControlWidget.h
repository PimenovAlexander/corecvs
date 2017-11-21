#ifndef FIXTURECONTROLWIDGET_H
#define FIXTURECONTROLWIDGET_H

#include <QWidget>
#include <QLineEdit>

#include "affine3dControlWidget.h"

namespace Ui {
class FixtureControlWidget;
}


/* This needs to be reworked */
class FixtureControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FixtureControlWidget(QWidget *parent = 0);
    ~FixtureControlWidget();

#if 0
    CameraLocationData* createParameters() const;
    void getParameters(CameraLocationData &params) const;
    void setParameters(const CameraLocationData &input);
    virtual void setParametersVirtual(void *input);

#endif
signals:
    void valueChanged();
    void paramsChanged();


public slots:
    /** We should consider who is responsible for what **/
    void loadPressed();
    void savePressed();

signals:
    void loadRequest(QString filename);
    void saveRequest(QString filename);

public:
    Ui::FixtureControlWidget *ui;
    Affine3dControlWidget *getLocationWidget();
    QLineEdit *getNameWidget();
private:

};

#endif // FIXTURECONTROLWIDGET_H
