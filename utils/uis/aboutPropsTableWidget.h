#ifndef ABOUTPROPSTABLEWIDGET_H
#define ABOUTPROPSTABLEWIDGET_H

#include <QTableWidget>
#include <QWidget>

class AboutPropsTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit AboutPropsTableWidget(QWidget *parent = 0);
    ~AboutPropsTableWidget();

public slots:
     void addParameter(QString name, QString value);

};

#endif // ABOUTPROPSTABLEWIDGET_H
