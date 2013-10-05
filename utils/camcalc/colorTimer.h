#pragma once

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtGui/QDoubleSpinBox>

class ColorTimer : public QTimer
{
    Q_OBJECT
public:
    ColorTimer(QDoubleSpinBox *box);
    ~ColorTimer();

signals:
    void colorTimeout(QDoubleSpinBox *box, int timeoutNum);

public slots:
    
private:
    /* Not an owner here */
    QDoubleSpinBox *mBox;
    int mTimeoutNum;

private slots:
    void emitSignal();
};

/*EOF*/
