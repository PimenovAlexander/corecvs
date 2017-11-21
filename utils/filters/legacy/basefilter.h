#ifndef BASEFILTER_H
#define BASEFILTER_H

#include <QtGui/QWidget>
#include "ui_basefilter.h"
#include "core/buffers/g12Buffer.h"

using namespace corecvs;

class BaseFilter : public QWidget
{
    Q_OBJECT

signals:
    void parametersChanged( void );

public:
    BaseFilter(QWidget *parent = 0);
    ~BaseFilter();

    static QString getName()
    {
        return QString("Base");
    };

    virtual QString getInstanceName()
    {
        return QString("Base");
    };

    virtual G12Buffer *filter (G12Buffer *input);

    /*BaseFilter *instance(QString Name);
    QList<QString> *possibleFilters();*/

private:
    Ui_BaseFilterClass ui;
};

class FilterFabric {

public:
    static BaseFilter *createFilterInstance(QString name);
    static QVector<QString> getFilterNames();
};

#endif // BASEFILTER_H
