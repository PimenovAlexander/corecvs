#pragma once

/**
 * \file filterBlockPresentation.h
 *
 * \date Nov 12, 2012
 **/

#include "../filterParametersControlWidgetBase.h"
#include "filterPinPresentation.h"

class FilterBlockPresentation : public QObject, public DiagramItem
{
    Q_OBJECT

public:
    FilterBlock *mBlock;
    FilterParametersControlWidgetBase* widget;
    char* blockName;
    int level;
    DiagramScene *scene;

    vector<FilterPinPresentation*> mInputPins;
    vector<FilterPinPresentation*> mOutputPins;

    FilterBlockPresentation(FilterBlock *block,
                            FilterParametersControlWidgetBase* _widget,
                            DiagramScene *_scene);

    /**
     *
     *
     *    *---------------------------------------------*
     *    |                                             |
     *    |<                                           >|
     *    |                    Type                     |
     *    |<                                           >|
     *    |                    Name                     |
     *    |                                            >|
     *    |                                             |
     *    *---------------------------------------------*
     *
     *
     **/
    virtual void draw(int x, int y);
    void createPinPresentations();
    void clearPinPresentations();
    void clearPinPresentations(vector<FilterPinPresentation*> &vec);
    void drawBlock(int x, int y);
    void drawInputPins();
    void drawOutputPins();
    void drawBlockName();
    void updateBlock();

    virtual ~FilterBlockPresentation();

    DiagramTextItem* textItem;

signals:
   void paramsChanged();
public slots:
   void parametersChanged()
   {
       mBlock->setParameters(widget->createParametersVirtual());
       emit paramsChanged();
   }

private:
};

/* EOF */
