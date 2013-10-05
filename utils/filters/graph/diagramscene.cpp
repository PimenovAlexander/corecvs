/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "iostream"
#include "filterGraphPresentation.h"

using namespace std;

DiagramScene::DiagramScene(QObject *parent)
    : QGraphicsScene(parent)
{
    myMode = MoveItem;
    line = 0;
    textItem = 0;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = Qt::black;
}

void DiagramScene::setFont(const QFont &font)
{
    myFont = font;

    if (isItemChange(DiagramTextItem::Type)) {
        QGraphicsTextItem *item =
            dynamic_cast<DiagramTextItem *>(selectedItems().first());
        //At this point the selection can change so the first selected item might not be a DiagramTextItem
        if (item)
            item->setFont(myFont);
    }
}

void DiagramScene::setMode(Mode mode)
{
    myMode = mode;
}

void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
//    cout<<"DiagramScene::mousePressEvent"<<endl;
    if (mouseEvent->button() != Qt::LeftButton)
        return;

    QList<QGraphicsItem *> listItems = items(mouseEvent->scenePos());

    bool pinSelected   = false;
    bool blockSelected = false;
    for(int i = 0; i < listItems.size(); i++)
    {
        if (listItems.value(i)->type() != DiagramItem::Type)
            continue;

        FilterPinPresentation *item = dynamic_cast<FilterPinPresentation *>(listItems.value(i));
        if (item != NULL)
            pinSelected = true;
        else
        {
            FilterBlockPresentation *item = dynamic_cast<FilterBlockPresentation *>(listItems.value(i));
            if (item != NULL)
                blockSelected = true;
        }
    }

    if (pinSelected)
        myMode = InsertLine;
    else if (blockSelected)
        myMode = InsertItem;

    switch (myMode) {
        case InsertItem:

            setMode(DiagramScene::MoveItem);
            break;

        case InsertLine:
            line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
                                        mouseEvent->scenePos()));
            line->setPen(QPen(myLineColor, 2));
            addItem(line);
            break;

        case InsertText:
            textItem = new DiagramTextItem(0, this);
            textItem->setFont(myFont);
            textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
            textItem->setZValue(1000.0);
            textItem->setDefaultTextColor(myTextColor);
            textItem->setPos(mouseEvent->scenePos());
            setMode(DiagramScene::InsertText);


    default:
        ;
    } // switch
    QGraphicsScene::mousePressEvent(mouseEvent);

    QList<QGraphicsItem *> listSelectedItems = selectedItems();
    if (listSelectedItems.size() == 0){
        emit blockItemSelected(NULL);

        QRectF rect(mouseEvent->scenePos().x()-10, mouseEvent->scenePos().y()-10, 20, 20);
        QList<QGraphicsItem *> listItems = items(rect);

        if (listItems.size() == 1)
        {
            if (listItems.value(0)->type() == Arrow::Type)
            {
                if (listItems.value(0)->isSelected())
                    listItems.value(0)->setSelected(false);
                else
                    listItems.value(0)->setSelected(true);
            }
        }
    }
    else
    {
        FilterBlockPresentation *item = dynamic_cast<FilterBlockPresentation *>(listSelectedItems.value(0));
        if (item != NULL){
            emit blockItemSelected(item);
        }

        FilterPinPresentation *pinItem = dynamic_cast<FilterPinPresentation *>(listSelectedItems.value(0));
        if (pinItem != NULL){
            emit pinItemSelected(pinItem);
        }

    }
} // mousePressEvent

void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
//    cout<<"mouseMoveEvent"<<endl;
    if (myMode == InsertLine && line != 0) {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    } else if (myMode == MoveItem) {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    bool changeOccurred = false;

//    cout<<"mouseReleaseEvent"<<endl;
    if (line != NULL && myMode == InsertLine) {
        QList<QGraphicsItem *> startItems = items(line->line().p1());
        QList<QGraphicsItem *> endItems = items(line->line().p2());

        for (int i = 0; i < startItems.size(); i++)
        {
            if (startItems.value(i) == line)
                continue;

            FilterPinPresentation *startItem = dynamic_cast<FilterPinPresentation*>(startItems.value(i));
            if (startItem == NULL)  continue;

            for (int j = 0; j < endItems.size(); j++)
            {
                if (endItems.value(j) == line)
                    continue;

                FilterPinPresentation *endItem = dynamic_cast<FilterPinPresentation*>(endItems.value(j));
                if (endItem == NULL)       continue;
                if (startItem == endItem)  continue;

                if (!filterGraph->connect(startItem->mPin, endItem->mPin))
                {
                    cout<<"Couldn't connect physically!"<<endl;
                    continue;
                }

                if (!endItem->mPin->setPin(startItem->mPin))
                {
                    cout<<"Pin types mismatch!"<<endl;
                    filterGraph->breakConnection(startItem->mPin, endItem->mPin);
                    continue;
                }

                addArrow(startItem, endItem);
                changeOccurred = true;
            } // for j
        } // for i
        removeItem(line);
        delete_safe(line);
    } // if

    //fill x and y for filterBlocks
    QList<QGraphicsItem *> listItems = items(mouseEvent->scenePos());
    for (int i = 0; i < listItems.size(); i++)
    {
        FilterBlockPresentation *diagramItem = dynamic_cast<FilterBlockPresentation*>(listItems.value(i));
        if (diagramItem == NULL)          continue;
        if (diagramItem->mBlock == NULL)  continue;

        diagramItem->mBlock->x = diagramItem->pos().x();
        diagramItem->mBlock->y = diagramItem->pos().y();
    }

    QGraphicsScene::mouseReleaseEvent(mouseEvent);

    if (changeOccurred)  emit graphChanged();
} // mouseReleaseEvent

void DiagramScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
//    cout << "DiagramScene::mouseDoubleClickEvent" << endl;

    QList<QGraphicsItem *> listItems = items(mouseEvent->scenePos());

    for (int i = 0; i < listItems.size(); i++)
    {
        FilterBlockPresentation *item = dynamic_cast<FilterBlockPresentation *>(listItems.value(i));
        if (item == NULL)  continue;
        if (item->mBlock->sort != FilterBlock::COMPOUND_FILTER)  continue;

//        cout << "FilterBlock::COMPOUND_FILTER" << endl;

        CompoundBlockPresentation* compoundFilterPresentation = dynamic_cast<CompoundBlockPresentation*>(item);
        if (compoundFilterPresentation == NULL)
            continue;
        if (compoundFilterPresentation->parent->filterPresentations->filterIsEdited(compoundFilterPresentation->compoundFilter->typeId))
            continue;

        FilterGraphPresentation* filterGraphPresentation;
        if (compoundFilterPresentation->child != NULL)
            filterGraphPresentation = compoundFilterPresentation->child;
        else
        {   filterGraphPresentation = new FilterGraphPresentation(compoundFilterPresentation->parent->filterPresentations,
                                                                  compoundFilterPresentation);
            compoundFilterPresentation->child = filterGraphPresentation;
        }

        filterGraphPresentation->restoreFromGraph();
        filterGraphPresentation->show();
//        filterGraphPresentation->setAttribute(Qt::WA_DeleteOnClose);

        compoundFilterPresentation->parent->filterPresentations->openedFilters.insert(
                    compoundFilterPresentation->compoundFilter->typeId);
    }

    QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void DiagramScene::addArrow(DiagramItem *startItem, DiagramItem *endItem, QGraphicsItem *parent)
{
    Arrow *arrow = new Arrow(startItem, endItem, parent, this);
    arrow->setColor(myLineColor);
    startItem->addArrow(arrow);
    endItem->addArrow(arrow);
    arrow->setZValue(-1000.0);
}

DiagramItem* DiagramScene::diagramItemByPin(Pin* pin)
{
    QList<QGraphicsItem *> listItems = items();

    for(int i = 0; i < listItems.size(); i++)
    {
        if (listItems.value(i)->type() != DiagramItem::Type)
            continue;

        FilterPinPresentation *item = dynamic_cast<FilterPinPresentation*>(listItems.value(i));
        if (item != NULL && item->mPin == pin)
            return item;
    }
    return NULL;
}

bool DiagramScene::isItemChange(int type)
{
    foreach (QGraphicsItem *item, selectedItems()) {
        if (item->type() == type)
            return true;
    }
    return false;
}
