#ifndef TXTPINPRESENTATION_H
#define TXTPINPRESENTATION_H

#include "filterBlockPresentation.h"

class TxtPinPresentation : public QObject, public FilterPinPresentation
{
    Q_OBJECT

public:
    TxtPinPresentation(Pin* pin, DiagramScene* scene, DiagramItem* parent = 0):
        FilterPinPresentation(pin, scene, parent)
    {
        textItem = new DiagramTextItem(this, scene);
        textItem->setFont(scene->font());
        textItem->setZValue(1000.0);
        textItem->setPlainText("T");

        textItem->setDefaultTextColor(scene->textColor());
        double hRect = textItem->boundingRect().right() - textItem->boundingRect().left();
        double wRect = textItem->boundingRect().top()   - textItem->boundingRect().bottom();
        textItem->setPos(-hRect/2, wRect/2);
    }

    virtual ~TxtPinPresentation()
    {
        delete_safe(textItem);
    }

//    virtual void draw(DiagramScene *scene, DiagramItem *blockItem, int x, int y);
private:
    DiagramTextItem* textItem;
};

#endif // TXTPINPRESENTATION_H
