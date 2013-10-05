#include "filterPinPresentation.h"
#include "g12PinPresentation.h"
#include "txtPinPresentation.h"

FilterPinPresentation::FilterPinPresentation(Pin* pin, DiagramScene *_scene, DiagramItem *parent) :
    DiagramItem(parent, _scene),
    mPin(pin), scene(_scene)
{
    if (pin->instanceName)
        pinName = pin->instanceName;
    else
    {   pinName = strdup(pin->instanceName);
        pin->instanceName = pinName;
    }

    myPolygon << QPointF(-10, -10) << QPointF(10, -10)
              << QPointF(10, 10)   << QPointF(-10, 10)
              << QPointF(-10, -10);

    pinNameItem = new DiagramTextItem(this, scene);
    drawPinName();

    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    finishInit();

    if (mPin != NULL && mPin->takeFrom != NULL)
    {   DiagramItem* startItem = scene->diagramItemByPin(mPin->takeFrom);
        if (startItem)  scene->addArrow(startItem, this);
    }

    if (mPin->parent && mPin->parent->parent)
    {
        for (unsigned int i = 0; i < mPin->parent->parent->blocks.size(); i++)
        {
            FilterBlock* block = mPin->parent->parent->blocks[i];
            for (unsigned int j = 0; j < block->inputPins.size(); j++)
            {
                if (block->inputPins[j]->takeFrom == mPin)
                {
                    DiagramItem* startItem = this;
                    DiagramItem* endItem = scene->diagramItemByPin(block->inputPins[j]);
                    if (endItem != NULL)  scene->addArrow(startItem, endItem);
                }
            }
        }
    }
}

void FilterPinPresentation::draw(DiagramScene *scene, DiagramItem *blockItem, int x, int y)
{
    if (scene == NULL || blockItem == NULL)  return;

//    cout<<"FilterPinPresentation::draw"<<endl;
    setBrush(scene->itemColor());
    setPos(x, y);
}

void FilterPinPresentation::drawInputPinName()
{
    double hRect = pinNameItem->boundingRect().right() - pinNameItem->boundingRect().left();
    double wRect = pinNameItem->boundingRect().top()   - pinNameItem->boundingRect().bottom();
    pinNameItem->setPos(-hRect/2, wRect/2+20);
}

void FilterPinPresentation::drawOutputPinName()
{
    double hRect = pinNameItem->boundingRect().right() - pinNameItem->boundingRect().left();
    double wRect = pinNameItem->boundingRect().top()   - pinNameItem->boundingRect().bottom();
    pinNameItem->setPos(-hRect/2, wRect/2-20);
}

FilterPinPresentation* FilterPinPresentation::getPinByType(Pin* pin,
                                                           DiagramScene *scene,
                                                           DiagramItem *parent)
{
    if (dynamic_cast<G12Pin*>(pin))
         return new G12PinPresentation(pin, scene, parent);
    else if (dynamic_cast<TxtPin*>(pin))
         return new TxtPinPresentation(pin, scene, parent);
    else return new FilterPinPresentation(pin, scene, parent);
}

void FilterPinPresentation::drawPinName()
{
    pinNameItem->setFont(scene->font());
    pinNameItem->setZValue(1000.0);
    pinNameItem->setPlainText(pinName);
    pinNameItem->setDefaultTextColor(scene->textColor());

    if (mPin->type == Pin::INPUT_PIN)
        drawInputPinName();
    if (mPin->type == Pin::OUTPUT_PIN)
        drawOutputPinName();
}

void FilterPinPresentation::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
//    cout << "FilterPinPresentation::paint ";
    DiagramItem::paint(painter, option, widget);

//    if ((mPin->type == Pin::INPUT_PIN) && (mPin->takeFrom == NULL))
//        painter->fillRect(option->rect, Qt::red);
}


FilterPinPresentation::~FilterPinPresentation()
{
    delete_safe(pinNameItem);
}
