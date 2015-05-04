#ifndef FILTER_SELECTOR_H_
#define FILTER_SELECTOR_H_

#include <QWidget>
#include <QtCore/qlist.h>

#include "ui_filterSelector.h"
#include "g12Buffer.h"
#include "abstractFilter.h"
#include "reflection.h"
#include "filterParametersControlWidgetBase.h"

using corecvs::BaseReflectionStatic;
using corecvs::AbstractFilter;

class FilterListIdWidgetItem : public QListWidgetItem
{

public:
    const char * name;

    FilterListIdWidgetItem(const char * _name) :
        QListWidgetItem(_name)
      , name(_name)
    {}

};

class FilterListWidgetItem : public QListWidgetItem
{

public:
    FilterParametersControlWidgetBase *parametersWidget;
    const char * name;

    FilterListWidgetItem(FilterParametersControlWidgetBase *_parametersWidget, const char * _name) :
        QListWidgetItem(_name)
      , parametersWidget(_parametersWidget)
      , name(_name)
    {}

};


class FilterSelectorParameters {
public:
    vector<const char *> filters;
    vector<BaseReflectionStatic *> parameters;

    virtual ~FilterSelectorParameters()
    {
        for (unsigned i = 0; i < parameters.size(); i++)
        {
            delete parameters[i];
            parameters[i] = NULL;
        }
    }

    void addFilter (const char *name, BaseReflectionStatic *params)
    {
        filters.push_back(name);
        parameters.push_back(params);
    }

    void printParams (void)
    {
        for (unsigned i = 0; i < filters.size(); i++)
        {
            printf("%s\n", filters[i]);
        }
    }
};

class FilterSelector : public QWidget
{
    Q_OBJECT


public slots:
    // Filter work
    void addFilter(void);
    void deleteFilter(void);
    void activateFilter(QListWidgetItem *);

    // Filter up/down
    void upFilter(void);
    void downFilter(void);


    void parametersChangedSlot( void );
signals:
    void parametersChanged( void );
public:
    FilterSelector(QString name = QString("FilterSelector"), QWidget *parent = 0);
    ~FilterSelector(){}

    FilterSelectorParameters *getParameters ();

    static FilterParametersControlWidgetBase *widgetFromName(const char *name);
    static AbstractFilter *executerFromName(const char *name);
    static QVector<const char *> getFilterNames();

private:
    Ui_FilterSelectorClass ui;

    QVector<FilterParametersControlWidgetBase *> mFilters;
    QVector<const char *> mAllFilters;

    //G12Buffer *input;
    QWidget *currentWidget;

    bool isContinious;
};

#endif // FILTER_SELECTOR_H_
