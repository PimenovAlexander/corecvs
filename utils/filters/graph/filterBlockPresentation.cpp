/**
 * \file filterBlockPresentation.cpp
 *
 * \date Nov 12, 2012
 **/

#include "filterBlockPresentation.h"
#include "filterBlock.h"

FilterBlockPresentation::FilterBlockPresentation(FilterBlock *block,
                                                 FilterParametersControlWidgetBase* _widget,
                                                 DiagramScene *_scene) :
                         DiagramItem(0, _scene),
                         mBlock(block),
                         widget(_widget),
                         scene(_scene)
{
    if (block->getInstanceName())
        blockName = mBlock->getInstanceName();
    else
    {   blockName = strdup(mBlock->getFullName().c_str());
        block->setInstanceName(blockName);
    }

    myPolygon << QPointF(-100, -50) << QPointF(100, -50)
              << QPointF(100, 50) << QPointF(-100, 50)
              << QPointF(-100, -50);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    textItem = new DiagramTextItem(this, scene);

    drawBlockName();
    createPinPresentations();
    finishInit();
}

void FilterBlockPresentation::draw(int x, int y)
{
    drawBlock(x, y);
    drawInputPins();
    drawOutputPins();
} // draw

void FilterBlockPresentation::drawBlock(int x, int y)
{
    mBlock->x = x;
    mBlock->y = y;

    setBrush(scene->itemColor());
    setPos(x, y);
}

void FilterBlockPresentation::drawInputPins()
{
    int xPin, yPin;
    double left   = polygon()[0].x();
    double top    = polygon()[0].y();
    double right  = -left;
    double width  = right - left;

    int totalInputPins  = (int)mInputPins.size();
    int inputStep       = (int)(width / (totalInputPins + 1));

    for (int i = 0; i < totalInputPins; i++)
    {
        xPin = left + inputStep*(i+1);
        yPin = top;

        mInputPins[i]->draw(scene, this, xPin, yPin);
        mInputPins[i]->drawInputPinName();
    }
}

void FilterBlockPresentation::drawOutputPins()
{
    int xPin, yPin;
    double left   = polygon()[0].x();
    double top    = polygon()[0].y();
    double right  = -left;
    double bottom = -top;
    double width  = right - left;

    int totalOutputPins = (int)mOutputPins.size();
    int outputStep      = (int)(width / (totalOutputPins + 1));

    for (int i = 0; i < totalOutputPins; i++)
    {
        xPin = left + outputStep*(i+1);
        yPin = bottom;

        mOutputPins[i]->draw(scene, this, xPin, yPin);
        mOutputPins[i]->drawOutputPinName();
    }
}

void FilterBlockPresentation::createPinPresentations()
{
    if (mBlock->sort != FilterBlock::INPUT_FILTER)
        for (unsigned int i = 0; i < mBlock->inputPins.size(); i++)
            mInputPins.push_back(FilterPinPresentation::getPinByType(mBlock->inputPins[i], scene, this));

    if (mBlock->sort != FilterBlock::OUTPUT_FILTER)
        for (unsigned int i = 0; i < mBlock->outputPins.size(); i++)
            mOutputPins.push_back(FilterPinPresentation::getPinByType(mBlock->outputPins[i], scene, this));
}

void FilterBlockPresentation::clearPinPresentations()
{
    clearPinPresentations(mInputPins);
    clearPinPresentations(mOutputPins);
//    for (unsigned int i = 0; i < mInputPins.size(); i++)
//    {   scene->removeItem(mInputPins[i]);
//        delete_safe(mInputPins[i]);
//    }
//    mInputPins.clear();

//    for (unsigned int i = 0; i < mOutputPins.size(); i++)
//    {   scene->removeItem(mOutputPins[i]);
//        delete_safe(mOutputPins[i]);
//    }
//    mOutputPins.clear();
}

void FilterBlockPresentation::clearPinPresentations(vector<FilterPinPresentation*> &vec)
{
    for (unsigned int i = 0; i < vec.size(); i++)
    {   scene->removeItem(vec[i]);
        delete_safe(vec[i]);
    }
    vec.clear();
}

void FilterBlockPresentation::drawBlockName()
{
    textItem->setFont(scene->font());
    textItem->setZValue(1000.0);
    textItem->setPlainText(QString("\"")+ QString(mBlock->getTypeName()) + QString("\" ") + QString(blockName));

    textItem->setDefaultTextColor(scene->textColor());
    double hRect = textItem->boundingRect().right() - textItem->boundingRect().left();
    double wRect = textItem->boundingRect().top()   - textItem->boundingRect().bottom();
    textItem->setPos(-hRect/2, wRect/2);
}

void FilterBlockPresentation::updateBlock()
{
    clearPinPresentations();
    createPinPresentations();

    blockName = mBlock->getInstanceName();
    draw(mBlock->x, mBlock->y);
}

FilterBlockPresentation::~FilterBlockPresentation()
{
    delete_safe(textItem);
    clearPinPresentations();
}
