#ifndef CALIBRATIONWIDGET_H
#define CALIBRATIONWIDGET_H

#include <QWidget>

namespace Ui {
class CalibrationWidget;
}

class CalibrationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CalibrationWidget(QWidget *parent = nullptr);
    ~CalibrationWidget();

private:
    Ui::CalibrationWidget *ui;
};

#endif // CALIBRATIONWIDGET_H
