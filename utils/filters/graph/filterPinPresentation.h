#pragma once

#include "global.h"

#include <QObject>
#include "diagramscene.h"

class FilterPinPresentation : /*public QObject,*/ public DiagramItem
{
//    Q_OBJECT
public:
    Pin* mPin;
    char* pinName;
    DiagramScene *scene;

    FilterPinPresentation(Pin* pin, DiagramScene *scene, DiagramItem *parent = 0);
    static FilterPinPresentation* getPinByType(Pin* pin, DiagramScene *scene, DiagramItem *parent = 0);

    virtual void draw(DiagramScene *scene, DiagramItem *blockItem, int x, int y);

    void drawInputPinName();
    void drawOutputPinName();
    void drawPinName();

    virtual ~FilterPinPresentation();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    DiagramTextItem* pinNameItem;
};
