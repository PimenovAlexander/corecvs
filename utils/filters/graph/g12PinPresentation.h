#ifndef G12PINPRESENTATION_H
#define G12PINPRESENTATION_H

#include "filterBlockPresentation.h"

class G12PinPresentation : public QObject, public FilterPinPresentation
{
    Q_OBJECT

public:
    G12PinPresentation(Pin* pin, DiagramScene* scene, DiagramItem* parent = 0):
        FilterPinPresentation(pin, scene, parent)
    {
        textItem = new DiagramTextItem(this, scene);
        textItem->setFont(scene->font());
        textItem->setZValue(1000.0);
        textItem->setPlainText("g12");

        textItem->setDefaultTextColor(scene->textColor());
        double hRect = textItem->boundingRect().right() - textItem->boundingRect().left();
        double wRect = textItem->boundingRect().top()   - textItem->boundingRect().bottom();
        textItem->setPos(-hRect/2, wRect/2);
    }

    virtual ~G12PinPresentation()
    {
        delete_safe(textItem);
    }

//    virtual void draw(DiagramScene *scene, DiagramItem *blockItem, int x, int y);
private:
    DiagramTextItem* textItem;
};

#endif // G12PINPRESENTATION_H
