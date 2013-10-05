#include <stdio.h>
#include "filterGraphPresentation.h"

FilterGraphPresentation::FilterGraphPresentation(FilterPresentationsCollection* fpcollection,
                                                 CompoundBlockPresentation* compoundfilterpresentation,
                                                 QWidget *parent)
    : QWidget(parent),
      filterPresentations(fpcollection),
      mCompoundFilter(NULL),
      mCompoundBlockPresentation(compoundfilterpresentation)

{
    ui.setupUi(this);

    connect(ui.useButton,       SIGNAL(released()), this, SLOT(addFilter()));
    connect(ui.drawGraphButton, SIGNAL(released()), this, SLOT(drawGraphClicked()));
    connect(ui.serializeButton, SIGNAL(released()), this, SLOT(serializeClicked()));
    connect(ui.saveAsButton,    SIGNAL(released()), this, SLOT(saveAsClicked()));

    connect(ui.allFiltersWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this,               SLOT(addFilter()));
    connect(ui.nameLineEdit,    SIGNAL(textChanged(QString)), this, SLOT(nameChanged()));


    deleteAction = new QAction(parent);
    deleteAction->setShortcut(QKeySequence(Qt::Key_Delete));
    ui.graphicsView->addAction(deleteAction);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));

    currentWidget = NULL;

    stream = new XMLPrinter();
    doc = new tinyxml2::XMLDocument();

    if (mCompoundBlockPresentation) {
        mCompoundFilter = mCompoundBlockPresentation->compoundFilter;
    }

    mFilterType = (mCompoundFilter) ? mCompoundFilter->typeId : -1;
    const char* filename = getGraphFileName();
    if (mCompoundBlockPresentation) {
        setWindowTitle(QString("Filter type: \"")
                      +QString(mCompoundFilter->getTypeName())
                      +QString("\" ")
                      +QString(mCompoundBlockPresentation->blockName)
                      +QString(" ")
                      +QString(filename));
    } else {
        setWindowTitle(QString("Main Graph: ")+QString(filename));
    }

    int len = FiltersCollection::numberOfFilterTypes();

    for (int i = 0; i < len; i++) {
        ui.allFiltersWidget->addItem(new FilterListIdWidgetItem(FiltersCollection::typenames[i]));
    }

    for (unsigned int i = 0; i < filterPresentations->fCollection->compoundFilters.size(); i++)
    {
        bool needRedColor = false;
        string itemName = filterPresentations->fCollection->compoundFilters[i].name;
        if (mCompoundBlockPresentation)
             if ( mCompoundBlockPresentation->compoundFilter->dependsOnMe(i) )
             {
                 itemName = "[ " + itemName + " ]";
                 needRedColor = true;
             }
        ui.allFiltersWidget->addItem(new FilterListIdWidgetItem(itemName.c_str()));
        if (needRedColor)
            ui.allFiltersWidget->item(ui.allFiltersWidget->count()-1)->setForeground(Qt::red);
    }

 /*   QVBoxLayout *layout;
    layout = new QVBoxLayout(ui.widget);

    expSpinBox = new ExponentialSlider(ui.widget);
    expSpinBox->setObjectName(QString::fromUtf8("expSpinBox"));
    expSpinBox->setMinimumSize(QSize(40, 0));
    connect(expSpinBox, SIGNAL(valueChanged(double)), this, SLOT(zoomChanged()));

    layout->addWidget(expSpinBox);*/
    connect(ui.zoomWidget     , SIGNAL(valueChanged(double)), this, SLOT(zoomChanged()));
    connect(ui.resetZoomButton, SIGNAL(released())          , this, SLOT(zoomReset()));

    if (mCompoundFilter == NULL) {
        filterGraph = new FilterGraph(filterPresentations->fCollection, NULL);
    } else {
        filterGraph = mCompoundFilter->graph;
    }

    scene = new DiagramScene(parent);
    scene->setFilterGraph(filterGraph);
    scene->setSceneRect(QRectF(0, 0, 500, 500));
    connect(scene, SIGNAL(blockItemSelected(QGraphicsItem*)),
            this,  SLOT  (blockItemSelected(QGraphicsItem*)));
    connect(scene, SIGNAL(pinItemSelected(QGraphicsItem*)),
            this,  SLOT  (pinItemSelected(QGraphicsItem*)));

    connect(scene, SIGNAL(graphChanged()),
            this,  SLOT  (graphChangedSlot()));

    graphicsView = ui.graphicsView;
    graphicsView->setScene(scene);

   // layout->addWidget(graphicsView);
}

void FilterGraphPresentation::graphChangedSlot(void)
{
    printf("FilterGraphPresentation::Child widget changed. Emitting params changed.\n");
//    delete stream;
//    stream = new XMLPrinter();
//    cout << "---------------------------" << endl;
//    filterGraph->serialize(stream);
//    cout << stream->CStr() << endl;
//    cout << "---------------------------" << endl;

//    emit graphChanged(stream->CStr());

    delete doc;

    doc = filterGraph->serialize();
    emit graphChanged(doc);
}

FilterBlockPresentation* FilterGraphPresentation::presentationByBlock(FilterBlock* block)
{
    for (int i = 0; i < filters.size(); i++)
       if (block == filters[i]->mBlock)  return filters[i];

    return NULL;
}

void FilterGraphPresentation::drawGraph(void)
{
    if (!filterGraph->topologicSort())
    {
        cout<<"Can't do filterGraph->topologicSort()"<<endl;
        return;
    }

    int dx = 220;
    int dy = 150;
    int x0 = 120;
    int y0 = 50;

    FilterBlock* block;
    FilterBlockPresentation* presentation;

    //init levels
    for (int i = 0; i < filters.size(); i++) {
        filters[i]->level = 0;
    }

    //fill levels
    for (int i = 0; i < filters.size(); i++)
    {
        presentation = filters[i];
        if (presentation == NULL) continue;
        block = presentation->mBlock;
        if (block == NULL) continue;
        for (unsigned int j = 0; j < block->inputPins.size(); j++)
        {
            if (block->inputPins[j] == NULL) continue;
            if (block->inputPins[j]->takeFrom == NULL)  continue;
            if (block->inputPins[j]->takeFrom->parent == NULL) continue;

            FilterBlockPresentation* parentPresentation = presentationByBlock(block->inputPins[j]->takeFrom->parent);
            if (parentPresentation == NULL) continue;

            int parentLevel = parentPresentation->level;
            if ( (parentLevel+1) > presentation->level )
                presentation->level = parentLevel + 1;
        }
    }

    int* levels = new int[filterGraph->blocks.size()];
    for (unsigned int i = 0; i < filterGraph->blocks.size(); i++)
        levels[i] = 0;

    //for each node of topologic tree
    for (unsigned int i = 0; i < filterGraph->blocks.size(); i++)
    {
        block        = filterGraph->blocks[i];
        if (block == NULL) continue;

        presentation = presentationByBlock(block);
        if (presentation == NULL) continue;

        int level    = presentation->level;

        int x = x0 + dx * levels[level];
        int y = y0 + dy * level;
        presentation->draw(x, y);
        levels[level]++;
    } // for i
} // drawGraph

void FilterGraphPresentation::addFilter(void)
{
    int row = ui.allFiltersWidget->currentRow();

    if (row >= (int)FiltersCollection::numberOfFilterTypes())
       if (mCompoundBlockPresentation)
           if ( mCompoundFilter->dependsOnMe(row - FiltersCollection::numberOfFilterTypes()) )
               return;

    FilterBlock* filter = filterPresentations->fCollection->getBlockFromRow(row);
    if (filter == NULL)
        return;
    filterGraph->addBlock(filter);

    FilterBlockPresentation *presentation = filterPresentations->presentationByName(row, filter, this, scene);
    connect(presentation, SIGNAL(paramsChanged()), this,  SLOT(graphChangedSlot()));
    filters.push_back(presentation);

    if (mCompoundBlockPresentation)
    {
        if (filter->sort == FilterBlock::INPUT_FILTER)
        {   mCompoundFilter->addInputPin(filter->inputPins[0]);
            mCompoundBlockPresentation->addInputPin(filter->inputPins[0]);
        }
        if (filter->sort == FilterBlock::OUTPUT_FILTER)
        {   mCompoundFilter->addOutputPin(filter->outputPins[0]);
            mCompoundBlockPresentation->addOutputPin(filter->outputPins[0]);
        }
        saveGraphToFile();
        mCompoundFilter->graph->ultimateParent()->renewCompoundFilters(mCompoundFilter->typeId, mCompoundFilter);
        rootParent()->renewCompoundFilters(mCompoundFilter->typeId);
        if (filter->sort == FilterBlock::COMPOUND_FILTER)
        {
            filterPresentations->fCollection->compoundFilters[mFilterType].contain.insert( row - FiltersCollection::numberOfFilterTypes() );
            filterPresentations->fCollection->recalculateDependencies();
        }
    }

//-------- graphical part ----------
    int x = 0;
    int y = 0;
    QList<QGraphicsItem*> listItems = scene->items();
    for (int i = 0; i < listItems.size(); i++)
    {   x += listItems.value(i)->pos().x();
        y += listItems.value(i)->pos().y();
    }
    if (listItems.size() > 0)
    {   x = (int)(x / listItems.size());
        y = (int)(y / listItems.size());
    }

    x += 120;
    y += 50;

    presentation->draw(x, y);

    graphChangedSlot();
}

FilterGraphPresentation::~FilterGraphPresentation()
{
    for (int i = 0; i < filters.size(); i++)
    {
        delete_safe(filters[i]->widget);
        delete_safe(filters[i]);
    }
    filters.clear();

    delete_safe(deleteAction);

    delete_safe(scene);
    delete_safe(filterGraph);

    delete_safe(stream);
    delete_safe(doc);
    delete filterPresentations;
}

void FilterGraphPresentation::deleteItem()
{
    bool changeOccurred = (scene->selectedItems().size() > 0);

    foreach (QGraphicsItem *item, scene->selectedItems())
    {   Arrow* arrow = dynamic_cast<Arrow*>(item);
        if (arrow)
        {   FilterPinPresentation* startItem = dynamic_cast<FilterPinPresentation*>(arrow->startItem());
            FilterPinPresentation* endItem = dynamic_cast<FilterPinPresentation*>(arrow->endItem());
            filterGraph->breakConnection(startItem->mPin, endItem->mPin);
            arrow->startItem()->removeArrow(arrow);
            arrow->endItem()->removeArrow(arrow);
        }

        FilterBlockPresentation *presentation = dynamic_cast<FilterBlockPresentation*>(item);
        if (presentation)
        {   foreach (QGraphicsItem *child, presentation->childItems())
            {   DiagramItem *diagramChild = dynamic_cast<DiagramItem*>(child);
                if (diagramChild)  diagramChild->removeArrows();
            }
            filterGraph->removeBlock(presentation->mBlock);

            QVector<FilterBlockPresentation*>::iterator filtersIterator
                     = std::find(filters.begin(), filters.end(), presentation);
            if (filtersIterator != filters.end())  filters.erase(filtersIterator);

            if (mCompoundBlockPresentation)
            {
                if (presentation->mBlock->sort == FilterBlock::INPUT_FILTER)
                {   mCompoundBlockPresentation->removeInputPin(presentation->mBlock->inputPins[0]);
                    mCompoundFilter->removeInputPin(presentation->mBlock->inputPins[0]);
                }
                if (presentation->mBlock->sort == FilterBlock::OUTPUT_FILTER)
                {   mCompoundBlockPresentation->removeOutputPin(presentation->mBlock->outputPins[0]);
                    mCompoundFilter->removeOutputPin(presentation->mBlock->outputPins[0]);
                }
                saveGraphToFile();
                mCompoundFilter->graph->ultimateParent()->renewCompoundFilters(mCompoundFilter->typeId, mCompoundFilter);
                rootParent()->renewCompoundFilters(mCompoundFilter->typeId);
            }
            delete_safe(presentation->mBlock);
        } // if
        scene->removeItem(item);
        delete item;
    } // foreach
    if (changeOccurred)  graphChangedSlot();
} // deleteItem

void FilterGraphPresentation::blockItemSelected(QGraphicsItem *item)
{
    FilterBlockPresentation *presentation =
    dynamic_cast<FilterBlockPresentation *>(item);

    if (currentWidget != NULL)
    {
        ui.gridLayout->removeWidget(currentWidget);
        currentWidget->setParent(NULL);
        currentWidget = NULL;

        ui.nameLineEdit->clear();
    }

    if (presentation != NULL)
    {
        if (presentation->widget != NULL)
        {
            if (!(mCompoundBlockPresentation != NULL &&
                 (presentation->mBlock->sort == FilterBlock::INPUT_FILTER ||
                  presentation->mBlock->sort == FilterBlock::OUTPUT_FILTER)))
            {
                currentWidget = presentation->widget;
                QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
                currentWidget->setSizePolicy(sizePolicy);

                ui.verticalLayout->insertWidget(ui.verticalLayout->count()-1,currentWidget);
            }
        }

        ui.nameLineEdit->setText(QString(presentation->blockName));
        this->update();
    }
}

void FilterGraphPresentation::pinItemSelected(QGraphicsItem *item)
{
    FilterPinPresentation *presentation =
    dynamic_cast<FilterPinPresentation *>(item);

    if (!ui.nameLineEdit->text().isEmpty())
    {   if (currentWidget != NULL)
        {
            ui.gridLayout->removeWidget(currentWidget);
            currentWidget->setParent(NULL);
            currentWidget = NULL;
        }
        ui.nameLineEdit->clear();
    }

    CompoundBlockPresentation* compound = dynamic_cast<CompoundBlockPresentation *>(presentation->parentItem());
    ui.nameLineEdit->setReadOnly(compound != NULL);

    if (presentation != NULL)
    {   ui.nameLineEdit->setText(QString(presentation->pinName));
        this->update();
    }
}

void FilterGraphPresentation::drawGraphClicked(void)
{
    drawGraph();
}

void FilterGraphPresentation::serializeClicked(void)
{
    cout << "serializeClicked" << endl;
    XMLPrinter* stream = new XMLPrinter();
    filterGraph->serialize(stream);

    cout << "---------------------------" << endl;
    cout << "Initial graph serialization" << endl;
    cout << "---------------------------" << endl;
    cout << stream->CStr() << endl;

    FilterGraph* newFilterGraph = new FilterGraph(filterPresentations->fCollection, NULL);
    newFilterGraph->deserialize(stream->CStr());

    XMLPrinter* stream1 = new XMLPrinter();
    newFilterGraph->serialize(stream1);

    cout << "-----------------------------" << endl;
    cout << "Secondary graph serialization" << endl;
    cout << "-----------------------------" << endl;
    cout << stream1->CStr() << endl;
}

void FilterGraphPresentation::zoom(int /*delta*/)
{

}

void FilterGraphPresentation::zoomReset()
{
    ui.zoomWidget->setValue(1.0);
    zoomChanged();
}

void FilterGraphPresentation::zoomChanged()
{
    double newScale = ui.zoomWidget->value();
    QMatrix oldMatrix = graphicsView->matrix();
    graphicsView->resetMatrix();
    graphicsView->translate(oldMatrix.dx(), oldMatrix.dy());
    graphicsView->scale(newScale, newScale);
}

void FilterGraphPresentation::restoreFromGraph()
{
    for (unsigned int i = 0; i < filterGraph->blocks.size(); i++)
    {   FilterBlockPresentation* presentation = filterPresentations->presentationByName(
                    filterPresentations->fCollection->getRowByName(filterGraph->blocks[i]->getTypeName()),
                    filterGraph->blocks[i],
                    this,
                    scene);
        if (presentation->widget)
            presentation->widget->setParametersVirtual(filterGraph->blocks[i]->getParameters());
        connect(presentation, SIGNAL(paramsChanged()), this,  SLOT(graphChangedSlot()));
        presentation->draw(filterGraph->blocks[i]->x, filterGraph->blocks[i]->y);
        filters.push_back(presentation);
    } // for i
} // restoreFromGraph

void FilterGraphPresentation::closeEvent(QCloseEvent *event)
{
    saveGraphToFile();
    if (mCompoundFilter)
    {   mCompoundFilter->graph->ultimateParent()->renewCompoundFilters(mCompoundFilter->typeId, mCompoundFilter);
//        rootParent()->renewCompoundFilters(mCompoundFilter->typeId);
        filterPresentations->openedFilters.erase(mFilterType);
        mCompoundBlockPresentation->child = NULL;
    }
    QWidget::closeEvent(event);
}

void FilterGraphPresentation::saveGraphToFile()
{
    const char* filename = getGraphFileName();
    filterGraph->saveToFile(filename);
}

const char* FilterGraphPresentation::getGraphFileName()
{
    if (mFilterType == -1)
         return "graph.conf";
    else return filterPresentations->fCollection->compoundFilters[mFilterType].filename.c_str();
}

void FilterGraphPresentation::nameChanged()
{
    if (ui.nameLineEdit->text().isEmpty())
        return;

    QList<QGraphicsItem *> items = scene->selectedItems();
    for (int i = 0; i < items.size(); i++)
    {   FilterBlockPresentation *blockItem = dynamic_cast<FilterBlockPresentation*>(items[i]);
        if (blockItem != NULL)
        {   blockItem->blockName = strdup(ui.nameLineEdit->text().toStdString().c_str());
            blockItem->mBlock->setInstanceName(blockItem->blockName);
            blockItem->drawBlockName();
        }

        FilterPinPresentation *pinItem = dynamic_cast<FilterPinPresentation*>(items[i]);
        if (pinItem != NULL)
        {   delete_safe(pinItem->pinName);

            pinItem->pinName = strdup(ui.nameLineEdit->text().toStdString().c_str());
            pinItem->mPin->instanceName = pinItem->pinName;
            pinItem->drawPinName();
        }
    } // for i
} // nameChanged

void FilterGraphPresentation::saveAsClicked(void)
{
    QFileDialog dialog(this);

    QString fileName = dialog.getSaveFileName(this,
                                              tr("Save graph to file"),
                                              "",
                                              tr("Graph configurations *.conf (*.conf)"));

    if (fileName.isEmpty())  return;
    bool ok;
    QString filterName = QInputDialog::getText(this,
                                               tr("Enter filter name"),
                                               tr("New filter name:"), QLineEdit::Normal,
                                               tr("New filter"), &ok);

    if (!ok || filterName.isEmpty())  return;

    filterGraph->saveToFile(fileName.toStdString().c_str());
    filterPresentations->fCollection->addFilterType(filterName.toStdString().c_str(), fileName.toStdString().c_str());
    filterPresentations->fCollection->saveCompoundFilters();
    ui.allFiltersWidget->addItem(new FilterListIdWidgetItem(filterName.toStdString().c_str()));
}

FilterGraphPresentation* FilterGraphPresentation::rootParent()
{
    FilterGraphPresentation* p = this;

    while (p->mCompoundBlockPresentation != NULL)
        p = p->mCompoundBlockPresentation->parent;

    return p;
}

void FilterGraphPresentation::renewCompoundFilters(int _typeid)
{
//    cout << "renewCompoundFilters" << endl;
    for (int i = 0; i < filters.size(); i++)
    {
        CompoundBlockPresentation* p = dynamic_cast<CompoundBlockPresentation*>(filters[i]);

        if (p == NULL || p->compoundFilter == NULL)
            continue;

        p->updateBlock();

        if (p->child == NULL)
            continue;

        if (p->compoundFilter->typeId == _typeid)
            p->child->updateGraph();
        else
            p->child->renewCompoundFilters(_typeid);
    }
}

void FilterGraphPresentation::updateGraph()
{
//    cout << "updateGraph()" << endl;
    for (int i = 0; i < filters.size(); i++)
    {
        FilterBlockPresentation* presentation = filters[i];
        presentation->updateBlock();
    }
} // updateGraph
