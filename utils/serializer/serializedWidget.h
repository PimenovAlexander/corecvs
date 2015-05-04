#pragma once
#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QWidget>

#include "serializer.h"

class SerializedWidget : public QWidget
{
    Q_OBJECT

public slots:
    void deserialize()
    {
        if (objectName().isEmpty())
            return;
        Serializer::deserialize(this);
        mTimer->stop();
    }

public:
    SerializedWidget(QWidget *parent = 0);

    ~SerializedWidget();

private:
    QTimer * mTimer;
};

