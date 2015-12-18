#ifndef VECTORWIDGET_H
#define VECTORWIDGET_H

#include <QWidget>
#include "parametersControlWidgetBase.h"


namespace Ui {
class VectorWidget;
}

class VectorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VectorWidget(QWidget *parent = 0);
    ~VectorWidget();

    ParametersControlWidgetBaseFabric *mFabric;

public slots:
    void addEntry();
    void removeEntry();

signals:

private:
    Ui::VectorWidget *ui;


};

#endif // VECTORWIDGET_H
