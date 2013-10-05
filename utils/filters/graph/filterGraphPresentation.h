//#pragma once
#ifndef FILTERGRAPHPRESENTATION_H
#define FILTERGRAPHPRESENTATION_H

#include "ui_filterGraphPresentation.h"
#include "arrow.h"
#include "exponentialSlider.h"
#include "filterPresentationsCollection.h"

class FilterGraphPresentation : public QWidget
{
    Q_OBJECT

public:
    FilterGraphPresentation(FilterPresentationsCollection* fcollection,
                            CompoundBlockPresentation* compoundfilterpresentation = NULL,
                            QWidget *parent = 0);

    virtual ~FilterGraphPresentation();

    enum FilterTypes{GAIN_OFFSET_FILTER = 0
                    ,BIT_SELECTOR_FILTER
                    ,SOBEL_FILTER
                    ,CANNY_FILTER
                    ,BACKGROUND_FILTER
                    ,OPERATION_FILTER
                    ,TXT_FILTER
                    ,INPUT_FILTER
                    ,OUTPUT_FILTER
    };

    template<typename T, int size>
    int GetArrLength(T(&)[size]){return size;}

    static const char *names[];

    FilterBlockPresentation* presentationByBlock(FilterBlock* block);
    DiagramItem* diagramItemByPin(Pin* pin);

    void drawGraph(void);
    void saveGraphToFile(void);
    const char* getGraphFileName(void);


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

    void restoreFromGraph();
    void renewCompoundFilters(int _typeid);
    void updateGraph();
    FilterGraphPresentation* rootParent();


public slots:
    // Filter work
    void addFilter();
    void drawGraphClicked();
    void serializeClicked();
    void saveAsClicked();
    void nameChanged();
    void graphChangedSlot();
    void zoom(int delta);
    void zoomReset();
    void zoomChanged();
    void closeEvent(QCloseEvent *);

signals:
//    void graphChanged(const char*);
    void graphChanged(tinyxml2::XMLDocument*);
    void GraphPresentationClosed(const char*);

private slots:
    void deleteItem();
    void blockItemSelected(QGraphicsItem *item);
    void pinItemSelected(QGraphicsItem *item);

public:
    FilterGraph* filterGraph;
    FilterPresentationsCollection* filterPresentations;

private:
    Ui_FilterGraphPresentationClass ui;

    QVector<FilterBlockPresentation*> filters;

    QWidget *currentWidget;

    QPushButton *deleteButton;
    QPushButton *drawGraphButton;
    QPushButton *serializeButton;

    QGraphicsView *graphicsView;
    DiagramScene *scene;

    QColor myTextColor;

    QAction *deleteAction;

    XMLPrinter* stream;
    tinyxml2::XMLDocument* doc;

    CompoundFilter* mCompoundFilter;
    CompoundBlockPresentation* mCompoundBlockPresentation;
    int mFilterType;
};

#endif // UI_FILTERGRAPHPRESENTATION_H
/* EOF */
