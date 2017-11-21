#ifndef CHANGERECIEVER_H
#define CHANGERECIEVER_H

#include <QObject>
#include "reflectionWidget.h"

#ifdef INCLUDE_EXAMPLE
#include "testClass.h"
#endif


class ChangeReceiver : public QObject {
    Q_OBJECT

public:
    ReflectionWidget *refWidget = NULL;

    ChangeReceiver(ReflectionWidget *widget) :
        refWidget(widget)
    {}

public slots:
  void processChange()
  {
      qDebug() << "ChangeReceiver::processChange(): called";

#ifdef INCLUDE_EXAMPLE
      TestClass example;
      refWidget->getParameters((void *)&example);

      PrinterVisitor printer(3,3);
      example.accept(printer);
#endif
  }
};

#endif // CHANGERECIEVER_H

