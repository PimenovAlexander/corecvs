#include "serializedWidget.h"

SerializedWidget::SerializedWidget(QWidget *parent) : QWidget(parent)
{
    mTimer = new QTimer(this);
    QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(deserialize()));
    mTimer->start(500);
}

SerializedWidget::~SerializedWidget()
{
    qDebug() << "serialized";
    Serializer::serialize(* this);
}


