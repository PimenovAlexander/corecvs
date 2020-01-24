#include <stdio.h>
#include <qtablewidget.h>
#include "filterSelector.h"

#include "core/filters/legacy/gainOffsetFilter.h"
#include "core/filters/legacy/bitSelectorFilter.h"
#include "core/filters/legacy/sobelFilter.h"
#include "core/filters/legacy/cannyFilter.h"
#include "openCVFilter.h"
#include "core/filters/legacy/backgroundFilter.h"

#include "ui/gainOffsetParametersControlWidget.h"
#include "ui/bitSelectorParametersControlWidget.h"
#include "ui/sobelParametersControlWidget.h"
#include "ui/cannyParametersControlWidget.h"
#include "ui/openCVFilterParametersControlWidget.h"
#include "ui/backgroundFilterParametersControlWidget.h"

FilterSelector::FilterSelector(QWidget *parent, QString name)
    : QWidget(parent)
{
    ui.setupUi(this);
    connect(ui.useButton, SIGNAL(released()), this, SLOT(addFilter()));
    connect(ui.deleteButton, SIGNAL(released()), this, SLOT(deleteFilter()));
    connect(ui.filtersWidget, SIGNAL(itemActivated(QListWidgetItem *)), this, SLOT(activateFilter(QListWidgetItem *)));

    connect(ui.upButton, SIGNAL(released()), this, SLOT(upFilter()));
    connect(ui.downButton, SIGNAL(released()), this, SLOT(downFilter()));


    currentWidget = NULL;
//    input = NULL;

    mAllFilters = getFilterNames();
    QVector<const char *>::iterator iterator;
    int i = 0;
    for (iterator = mAllFilters.begin(); iterator != mAllFilters.end(); ++iterator, ++i)
    {
        ui.allFiltersWidget->addItem(new FilterListIdWidgetItem(*iterator));
    }

    this->setWindowTitle(QString("Filter Selector:") + name);
}

void FilterSelector::parametersChangedSlot( void )
{
    qDebug("FilterSelector::Child widget changed. Emitting params changed...\n");
    emit parametersChanged();
}

void FilterSelector::addFilter(void)
{
    qDebug("FilterSelector::addFilter() called\n");
    QList<QListWidgetItem *> items = ui.allFiltersWidget->selectedItems();
    QList<QListWidgetItem *>::iterator addItems;

    for (addItems = items.begin(); addItems != items.end(); ++addItems)
    {
        const FilterListIdWidgetItem *item = static_cast<const FilterListIdWidgetItem *>(*addItems);
        const char* filterName = item->name;
        qDebug("Adding filter:%s\n", filterName );


        FilterParametersControlWidgetBase *filter = widgetFromName(filterName);
        if (filter == NULL) {
            continue;
        }

        mFilters.append(filter);
        QListWidgetItem *newItem = new FilterListWidgetItem(filter, filterName);
        ui.filtersWidget->addItem(newItem);

        connect(filter, SIGNAL(paramsChanged()), this, SLOT(parametersChangedSlot()));
    }

    if (items.size() != 0)
    {
        qDebug("FilterAdded: Emitting params changed...\n");
        ui.filtersWidget->update();
        emit parametersChanged();
    }
}

void FilterSelector::deleteFilter(void)
{
    QList<QListWidgetItem *> items = ui.filtersWidget->selectedItems();
    QList<QListWidgetItem *>::iterator itemIterator;

    QList<QListWidgetItem *> toDelete;

    for (itemIterator = items.begin(); itemIterator != items.end(); ++itemIterator)
    {
        QListWidgetItem *item = (*itemIterator);
        FilterParametersControlWidgetBase *filter = ((FilterListWidgetItem *) item)->parametersWidget;

        if (filter == currentWidget)
        {
            ui.gridLayout->removeWidget(currentWidget);
            currentWidget->setParent(NULL);
            currentWidget  = NULL;
        }
        int index = mFilters.indexOf(filter);
        mFilters.remove(index);
        toDelete.append(item);
        disconnect(filter, SIGNAL(paramsChanged()), this, SLOT(parametersChangedSlot()));
    }

    for (itemIterator = toDelete.begin(); itemIterator != toDelete.end(); ++itemIterator)
    {
        ui.filtersWidget->removeItemWidget(*itemIterator);
        delete_safe(*itemIterator);
    }

    if (items.size() != 0)
    {
        qDebug("Filters Deleted: Emitting params changed...\n");
        emit parametersChanged();
    }
}

void FilterSelector::upFilter(void)
{
    int index = ui.filtersWidget->currentIndex().row();
    if (index == 0)
        return;
    QListWidgetItem *toSwap = ui.filtersWidget->takeItem(index - 1);
    ui.filtersWidget->insertItem(index, toSwap);

    FilterParametersControlWidgetBase *currentFilter = mFilters.at(index);
    mFilters.remove(index);
    mFilters.insert(index - 1, currentFilter);

    printf("Filter Moved Up: Emitting params changed...\n");
    emit parametersChanged();
}

void FilterSelector::downFilter(void)
{
    int index = ui.filtersWidget->currentIndex().row();
    if (index == ui.filtersWidget->count() - 1)
        return;
    QListWidgetItem *toSwap = ui.filtersWidget->takeItem(index + 1);
    ui.filtersWidget->insertItem(index, toSwap);

    FilterParametersControlWidgetBase *currentFilter = mFilters.at(index);
    mFilters.remove(index);
    mFilters.insert(index + 1, currentFilter);

    printf("Filter Moved Down: Emitting params changed...\n");
    emit parametersChanged();
}

void FilterSelector::activateFilter(QListWidgetItem *item)
{
    if (currentWidget != NULL)
    {
        //printf("Removing current widget: %s\n", currentWidget->getInstanceName().toLatin1().constData());
        ui.gridLayout->removeWidget(currentWidget);
        currentWidget->setParent(NULL);
        currentWidget = NULL;
    } else {
        printf("Not removing current widget:\n");
    }

    currentWidget = ((FilterListWidgetItem *)item)->parametersWidget;
    //printf("Adding current widget: %s\n", currentWidget->getInstanceName().toLatin1().constData());
    ui.gridLayout->addWidget(currentWidget, 4, 0, 1, 2);
    this->update();
}


//FilterSelector::~FilterSelector()
//{
////    QVector<FilterParametersControlWidgetBase *>::iterator items;
////    for (items = mFilters.begin(); items != mFilters.end(); ++items)
////    {
////        delete_safe(*items);
////    }
//}

FilterParametersControlWidgetBase *FilterSelector::widgetFromName(const char *name)
{
    (void)name;
//    if (name == QString(GainOffsetFilter::getName()))
//       return new GainOffsetParametersControlWidget();
//    if (name == QString(BitSelectorFilter::getName()))
//       return new BitSelectorParametersControlWidget();
//    if (name == QString(SobelFilter::getTypeName()))
//       return new SobelParametersControlWidget();
//    if (name == QString(CannyFilter::getName()))
//       return new CannyParametersControlWidget();
//#ifdef WITH_OPENCV
//    if (name == QString(OpenCVFilter::getName()))
//       return new OpenCVFilterParametersControlWidget();
//#endif
//    if (name == QString(BackgroundFilter::getName()))
//        return new BackgroundFilterParametersControlWidget();
   return NULL;
}

AbstractFilter *FilterSelector::executerFromName(const char *name)
{
    (void)name;
//    if (name == QString(GainOffsetFilter::getName()))
//       return new GainOffsetFilter();
//    if (name == QString(BitSelectorFilter::getName()))
//       return new BitSelectorFilter();
//    if (name == QString(SobelFilter::getTypeName()))
//       return new SobelFilter();
//    if (name == QString(CannyFilter::getName()))
//       return new CannyFilter();
//#ifdef WITH_OPENCV
//    if (name == QString(OpenCVFilter::getName()))
//       return new OpenCVFilter();
//#endif
//    if (name == QString(BackgroundFilter::getName()))
//        return new BackgroundFilter();
    return NULL;
}

QVector<const char *> FilterSelector::getFilterNames()
{
    QVector<const char *> list;
//    list.append(GainOffsetFilter::getName());
    //list.append(BitSelectorFilter::getTypeName());
//    list.append(SobelFilter::getTypeName());
//    list.append(CannyFilter::getName());
//#ifdef WITH_OPENCV
//    list.append(OpenCVFilter::getName());
//#endif
//    list.append(BackgroundFilter::getName());
    return list;
}

FilterSelectorParameters *FilterSelector::getParameters()
{
    qDebug("FilterSelector::getParameters(): called\n");
    FilterSelectorParameters *toReturn = new FilterSelectorParameters();

    for (int i = 0; i < mFilters.size(); i++)
    {
        FilterParametersControlWidgetBase *filter = mFilters[i];
        BaseReflectionStatic *params = filter->createParametersVirtual();
        FilterListWidgetItem *item = static_cast<FilterListWidgetItem *>(ui.filtersWidget->item(i));
        toReturn->addFilter(item->name, params);

        if (params == NULL)
        {
            qDebug() << "We have recieved NULL parameters from " << item->name << endl;
        }
        //toReturn->filters.push_back(filter->create);
    }
    return toReturn;
}
