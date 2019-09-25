#ifndef RADIOCONTROLWIDGET_H
#define RADIOCONTROLWIDGET_H

#include "multimoduleController.h"

#include <QWidget>

namespace Ui {
class RadioControlWidget;
}

class RadioControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RadioControlWidget(QWidget *parent = 0);
    ~RadioControlWidget();

    MultimoduleController controller;

public slots:
    void rereadDevices();
    void connect();
    void disconnect();


private:
    Ui::RadioControlWidget *ui;
};

#endif // RADIOCONTROLWIDGET_H
