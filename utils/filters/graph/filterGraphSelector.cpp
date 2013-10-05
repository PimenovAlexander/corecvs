#include <stdio.h>
#include <QtGui/qtablewidget.h>
#include "filterGraphSelector.h"

#include "filterBlockDirectory.h"
#include <QtGui/qpainter.h>

FilterGraphSelector::FilterGraphSelector(QString name, QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.useButton, SIGNAL(released()), this, SLOT(addFilter()));
    connect(ui.deleteButton, SIGNAL(released()), this, SLOT(deleteFilter()));

    /* Fill all widget list with widgets */
    filtersClasses = FilterBlockDirectory::getFilterNames();
    QVector<const char *>::iterator iterator;
    for (iterator = filtersClasses.begin(); iterator != filtersClasses.end(); ++iterator)
    {
        ui.allFiltersWidget->addItem(new FilterListIdWidgetItem(*iterator));
    }

    this->setWindowTitle(QString("Filter Selector:") + name);

    /* Connet drawing functions */
    connect(ui.widget, SIGNAL(askParentRepaint(QPaintEvent *, QWidget *)), this, SLOT(repaintGraphArea(QPaintEvent *, QWidget *)));
}

void FilterGraphSelector::parametersChangedSlot( void )
{
    printf("FilterGraphSelector::Child widget changed. Emitting params changed.\n");
    emit parametersChanged();
}

void FilterGraphSelector::addFilter(void)
{
    printf("FilterGraphSelector::addFilter called\n");
    QList<QListWidgetItem *> items = ui.allFiltersWidget->selectedItems();
    QList<QListWidgetItem *>::iterator addItems;

    for (addItems = items.begin(); addItems != items.end(); ++addItems)
    {
        const FilterListIdWidgetItem *item = static_cast<const FilterListIdWidgetItem *>(*addItems);
        const char* filterName = item->name;
        printf("FilterGraphSelector::Adding filter:%s\n", filterName );

        FilterBlock *filter = FilterBlockDirectory::executerFromName(filterName);
        FilterBlockPresentation *filterPresentation
            = new FilterBlockPresentation(filter, 40, 40 + 100 * filters.size());

        if (filterPresentation == NULL)
            continue;

        filters.append(filterPresentation);
        selectedFilterId = filters.size() - 1;
/*        QListWidgetItem *newItem = new FilterListWidgetItem(filter, filterName);*/
//        ui.filtersWidget->addItem(newItem);

       // connect(filter, SIGNAL(paramsChanged()), this, SLOT(parametersChangedSlot()));
    }

    ui.widget->update();

    if (items.size() != 0)
    {
        printf("FilterAdded: Emitting params changed...\n");
//        ui.filtersWidget->update();
        emit parametersChanged();
    }
}


void FilterGraphSelector::repaintGraphArea(QPaintEvent * /*event*/, QWidget *who)
{
    printf("Executing repaint\n");
    QPainter painter(who);
    painter.setPen(Qt::red);
    painter.drawText(QPoint(20,20), QString("Filters:%1").arg(filters.size()));
    for (int i = 0; i < filters.size(); i++)
    {
        filters[i]->draw(painter, selectedFilterId == i);
    }
}


void FilterGraphSelector::deleteFilter(void)
{
#if 0
    QList<QListWidgetItem *> items; //= ui.filtersWidget->selectedItems();
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
        int index = filters.indexOf(filter);
        filters.remove(index);
        toDelete.append(item);
        disconnect(filter, SIGNAL(paramsChanged()), this, SLOT(parametersChangedSlot()));
    }

    for (itemIterator = toDelete.begin(); itemIterator != toDelete.end(); ++itemIterator)
    {
//        ui.filtersWidget->removeItemWidget(*itemIterator);
        delete_safe(*itemIterator);
    }

    if (items.size() != 0)
    {
        printf("Filters Deleted: Emitting params changed...\n");
        emit parametersChanged();
    }
#endif
}

void FilterGraphSelector::activateFilter(QListWidgetItem * /*item*/)
{
#if 0
    if (currentWidget != NULL)
    {
        //printf("Removing current widget: %s\n", currentWidget->getInstanceName().toAscii().constData());
        ui.gridLayout->removeWidget(currentWidget);
        currentWidget->setParent(NULL);
        currentWidget = NULL;
    } else {
        printf("Not removing current widget:\n");
    }

    currentWidget = ((FilterListWidgetItem *)item)->parametersWidget;
    //printf("Adding current widget: %s\n", currentWidget->getInstanceName().toAscii().constData());
    ui.gridLayout->addWidget(currentWidget, 4, 0, 1, 2);
    this->update();
#endif
}

/*
void FilterGraphSelector::bufferChangedSlot(G12Buffer *buffer)
{
    input = buffer;
    if (isContinious)
    {
        emit bufferChanged(filter(input));
    }
}
*/
/*
G12Buffer *FilterGraphSelector::filter (G12Buffer *input)
{
    G12Buffer *toReturn = new G12Buffer(input);

    QVector<QWidget *>::iterator iterator;
    for (iterator = filters.begin(); iterator != filters.end(); ++iterator)
    {
        QWidget *filter = *iterator;
        G12Buffer *oldFilter = toReturn;
        toReturn = filter->filter(toReturn);
        delete oldFilter;
    }
    return toReturn;
}
*/


FilterGraphSelector::~FilterGraphSelector()
{
    QVector<FilterBlockPresentation *>::iterator items;
    for (items = filters.begin(); items != filters.end(); ++items)
    {
        delete_safe(*items);
    }
}



FilterGraphSelectorParameters *FilterGraphSelector::getParameters()
{
    qDebug("FilterGraphSelector::getParameters(): called\n");
    FilterGraphSelectorParameters *toReturn = new FilterGraphSelectorParameters();

    for (int i = 0; i < filters.size(); i++)
    {
     /*   FilterParametersControlWidgetBase *filter = filters[i];
        BaseReflectionStatic *params = filter->createParametersVirtual();
        FilterListWidgetItem *item = NULL; //static_cast<FilterListWidgetItem *>(ui.filtersWidget->item(i));
        toReturn->addFilter(item->name, params);

        if (params == NULL)
        {
            cout << "We have recieved NULL parameters from " << item->name << endl;
        }
        //toReturn->filters.push_back(filter->create);

         */
    }
    return toReturn;
}
