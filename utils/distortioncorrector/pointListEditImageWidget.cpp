#include <QMenu>

#include "pointListEditImageWidget.h"
#include "painterHelpers.h"
#include "qtHelper.h"


PointListEditImageWidget::PointListEditImageWidget(QWidget *parent, bool showHeader) :
    AdvancedImageWidget(parent, showHeader),
    mObservationListModel(NULL),
    mSelectedPoint(-1)
{
    mMoveButton    = addToolButton("Select",            QIcon(":/new/prefix1/select_by_color.png"));
    mAddButton     = addToolButton("Add Point To Path", QIcon(":/new/prefix1/vector_add.png"     ), false);
    mDeleteButton  = addToolButton("Delete Point",      QIcon(":/new/prefix1/vector_delete.png"  ), false);
    mAddInfoButton = addToolButton("Toggle info",       QIcon(":/new/prefix1/info_rhombus.png"   ), false);
    mAddInfoButton ->setCheckable(true);

}

void PointListEditImageWidget::setObservationModel(ObservationListModel *observationListModel)
{
    if (mObservationListModel != NULL) {
        disconnect(mObservationListModel, 0, this, 0);
    }

    mObservationListModel = observationListModel;

    if (mObservationListModel != NULL) {
        connect(mObservationListModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this, SLOT(update()));
    }

    connect(mObservationListModel, SIGNAL(columnsInserted(QModelIndex,int,int))             , this, SLOT(invalidateModel()));
    connect(mObservationListModel, SIGNAL(columnsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(invalidateModel()));
    connect(mObservationListModel, SIGNAL(columnsRemoved(QModelIndex,int,int))              , this, SLOT(invalidateModel()));

    connect(mObservationListModel, SIGNAL(rowsInserted(QModelIndex,int,int))             , this, SLOT(invalidateModel()));
    connect(mObservationListModel, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(invalidateModel()));
    connect(mObservationListModel, SIGNAL(rowsRemoved(QModelIndex,int,int))              , this, SLOT(invalidateModel()));

    connect(mObservationListModel, SIGNAL(modelReset())                                     , this, SLOT(invalidateModel()));
}

/* This is called when model indices are changed, and our cache is no longer valid */
void PointListEditImageWidget::invalidateModel()
{
    mSelectedPoint = -1;
    update();
}

void PointListEditImageWidget::childRepaint(QPaintEvent *event, QWidget *who)
{
    AdvancedImageWidget::childRepaint(event, who);
    if (mImage.isNull())
        return;

    if (mObservationListModel == NULL)
        return;

    /* Now the points */
    QPainter painter(who);

    QModelIndex topLevel = mObservationListModel->parent(QModelIndex());

    int rows = mObservationListModel->rowCount(topLevel);

    for (int i = 0; i < rows; i ++)
    {
        Vector2dd point = getPointById(i);
        Vector2dd imageCoords = imageToWidgetF(point);
        painter.setPen(Qt::yellow);
        drawCircle(painter, imageCoords, 5);
        painter.setPen(Qt::blue);
        drawCircle(painter, imageCoords, 10);

        if (mAddInfoButton->isChecked())
        {
            QString meta = getMetaById(i);
            QPointF pos = Core2Qt::QPointFromVector2dd(imageCoords + Vector2dd(5, -10));
            painter.setPen(Qt::black);
            painter.drawText(pos, meta);
            pos += QPointF(1,1);
            painter.setPen(Qt::white);
            painter.drawText(pos, meta);
        }

        if (i == mSelectedPoint) {
            painter.setPen(Qt::red);
            drawCircle(painter, imageCoords, 7);

            /* Test it a bit and use QSelectionModel */
            imageCoords = imageToWidgetF(widgetToImageF(imageCoords));
            painter.setPen(Qt::cyan);
            drawCircle(painter, imageCoords, 3);
        }
    }
}

void PointListEditImageWidget::toolButtonReleased(QWidget *button)
{
    mUi->widget->unsetCursor();
    AdvancedImageWidget::toolButtonReleased(button);

    if (button == mMoveButton)
    {
        qDebug() << "Move Button";
        mCurrentToolClass = (ToolClass)MOVE_POINT_TOOL;
        mUi->widget->setCursor(Qt::ArrowCursor);
    }
    else if (button == mAddButton)
    {
        qDebug() << "Add Button";
        //mCurrentToolClass = (ToolClass)ADD_POINT_TOOL;
        mObservationListModel->insertRow(mObservationListModel->rowCount());
        mSelectedPoint = mObservationListModel->rowCount() - 1;

        QModelIndex indexX = mObservationListModel->index(mSelectedPoint, ObservationListModel::COLUMN_U);
        QModelIndex indexY = mObservationListModel->index(mSelectedPoint, ObservationListModel::COLUMN_V);

        mObservationListModel->setData(indexY, QVariant(mZoomCenter.y()), Qt::EditRole);
        mObservationListModel->setData(indexX, QVariant(mZoomCenter.x()), Qt::EditRole);
        mUi->widget->update();
    }
    else if (button == mDeleteButton)
    {
        qDebug() << "Delete Button";
        if (mSelectedPoint >= 0)
        {
            mObservationListModel->removeRow(mSelectedPoint);
            mSelectedPoint = -1;
        }
        mUi->widget->update();
    }
    else if (button == mAddInfoButton)
    {
        mUi->widget->update();
    }
}

Vector2dd PointListEditImageWidget::getPointById(int row)
{
    QModelIndex topLevel = mObservationListModel->parent(QModelIndex());
    QModelIndex indexX = mObservationListModel->index(row, ObservationListModel::COLUMN_U, topLevel);
    QModelIndex indexY = mObservationListModel->index(row, ObservationListModel::COLUMN_V, topLevel);

    double x = mObservationListModel->data(indexX).toDouble();
    double y = mObservationListModel->data(indexY).toDouble();

    return Vector2dd(x,y);
}

QString PointListEditImageWidget::getMetaById(int row)
{
    QModelIndex topLevel = mObservationListModel->parent(QModelIndex());
    QModelIndex indexX = mObservationListModel->index(row, ObservationListModel::COLUMN_X, topLevel);
    QModelIndex indexY = mObservationListModel->index(row, ObservationListModel::COLUMN_Y, topLevel);
    QModelIndex indexZ = mObservationListModel->index(row, ObservationListModel::COLUMN_Z, topLevel);


    double x = mObservationListModel->data(indexX).toDouble();
    double y = mObservationListModel->data(indexY).toDouble();
    double z = mObservationListModel->data(indexZ).toDouble();

    return QString("%1 [%2 %3 %4]").arg(row + 1).arg(x).arg(y).arg(z);
}

int PointListEditImageWidget::findClosest(Vector2dd imagePoint, double limitDistance )
{
    QModelIndex topLevel = mObservationListModel->parent(QModelIndex());
    int rows = mObservationListModel->rowCount(topLevel);

    double bestDistance = limitDistance;
    int bestIndex = -1;

    for (int i = 0; i < rows; i ++)
    {
        Vector2dd currentPoint = getPointById(i);
        double dist = (imagePoint - currentPoint).l2Metric();
        if (dist < bestDistance) {
            bestDistance = dist;
            bestIndex = i;
        }
    }

    return bestIndex;
}

void PointListEditImageWidget::childMousePressed(QMouseEvent *event)
{
    AdvancedImageWidget::childMousePressed(event);

    if ((event->buttons() & Qt::LeftButton) == 0)
        return;

    PaintToolClass tool = (PaintToolClass)mCurrentToolClass;

    if (tool == ADD_POINT_TOOL)
    {
    }

    if (tool == DEL_POINT_TOOL)
    {
    }

    if (tool == MOVE_POINT_TOOL)
    {

        Vector2dd releasePoint = Vector2dd(event->x(), event->y());
//        bool shiftPressed = event->modifiers().testFlag(Qt::ShiftModifier);

        Vector2dd imagePoint = widgetToImageF(releasePoint);
        mSelectedPoint = findClosest(imagePoint, 5);
        mUi->widget->update();

    }
}

void PointListEditImageWidget::childMouseMoved(QMouseEvent * event)
{
    QModelIndex topLevel = mObservationListModel->parent(QModelIndex());

    if ((event->buttons() & Qt::LeftButton) != 0)
    {
        switch (mCurrentToolClass)
        {
        case MOVE_POINT_TOOL:
        {
            //   qDebug() << "Drag in selected tool";
            if (!mIsMouseLeftPressed)
                break;

            Vector2dd dragStart    = widgetToImageF(Qt2Core::Vector2ddFromQPoint(mSelectionEnd));
            Vector2dd currentPoint = widgetToImageF(Qt2Core::Vector2ddFromQPoint(event->pos()));
            Vector2dd shift = (dragStart - currentPoint);


           /* for (unsigned i = 0; i < mFeatures.mSelectedPoints.size(); i++)
            {
                mFeatures.mSelectedPoints[i]->position -= shift;
            }*/
            if (mSelectedPoint >= 0) {
                Vector2dd currentPoint = getPointById(mSelectedPoint);
                currentPoint -= shift;

                QModelIndex indexX = mObservationListModel->index(mSelectedPoint, ObservationListModel::COLUMN_U, topLevel);
                QModelIndex indexY = mObservationListModel->index(mSelectedPoint, ObservationListModel::COLUMN_V, topLevel);
                mObservationListModel->setData(indexY, QVariant(currentPoint.y()), Qt::EditRole);
                mObservationListModel->setData(indexX, QVariant(currentPoint.x()), Qt::EditRole);
            }


            mUi->widget->update();
        }
        default:
            break;
        }
    }

    AdvancedImageWidget::childMouseMoved(event);
}

/*********************************************************************************************************
 *
 *
 *
 *********************************************************************************************************/

PointListEditImageWidgetUnited::PointListEditImageWidgetUnited(QWidget *parent, bool showHeader)
    : AdvancedImageWidget(parent, showHeader)
    , mObservationListModel(NULL)
    , selectionModel(NULL)
    , mSelectedPoint(-1)
{
    mMoveButton    = addToolButton("Select",            QIcon(":/new/prefix1/select_by_color.png"));
    mAddButton     = addToolButton("Add Point To Path", QIcon(":/new/prefix1/vector_add.png"     ), false);
    mDeleteButton  = addToolButton("Delete Point",      QIcon(":/new/prefix1/vector_delete.png"  ), false);
    mAddInfoButton = addToolButton("Toggle info",       QIcon(":/new/prefix1/info_rhombus.png"   ), false);
    mAddInfoButton ->setCheckable(true);

    /*Delegate activity*/
    mDelegateStyleButton = new QPushButton(this);
    mDelegateStyleButton->setText("Style");
    mDelegateStyleButton->setEnabled(true);
    mDelegateStyleButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(mDelegateStyleButton, SIGNAL(released()), this, SLOT(delegateMenuShow()));

    QWidget* holder = mUi->frame_2;
    holder->layout()->addWidget(mDelegateStyleButton);
    //connect(mDelegateStyleBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));

}

void PointListEditImageWidgetUnited::setObservationModel(PointImageEditorInterface *observationListModel)
{
    if (mObservationListModel != NULL) {
        disconnect(mObservationListModel, 0, this, 0);
    }

    mObservationListModel = observationListModel;

    if (mObservationListModel != NULL)
    {
        connect(mObservationListModel, SIGNAL(updateView())      , this, SLOT(update()));
        connect(mObservationListModel, SIGNAL(modelInvalidated()), this, SLOT(invalidateModel()));
    }
}

void PointListEditImageWidgetUnited::setSelectionModel(QItemSelectionModel *_selectionModel)
{
    if (selectionModel != NULL) {
        disconnect(selectionModel, 0, this, 0);
    }

    selectionModel = _selectionModel;

    if (selectionModel != NULL) {
        connect(selectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(update()));
    }
}


/* This is called when model indices are changed, and our cache is no longer valid */
void PointListEditImageWidgetUnited::invalidateModel()
{
    mSelectedPoint = -1;
    update();
}

void PointListEditImageWidgetUnited::selectPoint(int id)
{
    qDebug("PointListEditImageWidgetUnited::selectPoint(%d)", id);
    mSelectedPoint = id;
    if (selectionModel != NULL && mObservationListModel != NULL)
    {
        QModelIndex pos = mObservationListModel->index(id, 0);

        if (mSelectedPoint != -1) {
            selectionModel->select         (pos, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
            selectionModel->setCurrentIndex(pos, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        }
        else {
            selectionModel->clear();
        }
    }
}

static QAction * addCheckBoxAction(QString name, bool state, QMenu *menu)
{
    QAction *action = new QAction(name, menu);
    action->setCheckable(true);
    action->setChecked(state);
    menu->addAction(action);
    return action;
}


void PointListEditImageWidgetUnited::delegateMenuShow()
{
    QPoint p = QCursor::pos();
    QMenu menu;
    QAction *actionDecAll      = addCheckBoxAction("Show Decorator All"     , mDecortatorAll     , &menu);
    QAction *actionDecMatched  = addCheckBoxAction("Show Decorator Matched" , mDecortatorMatched , &menu);
    QAction *actionDecSelected = addCheckBoxAction("Show Decorator Selected", mDecortatorSelected, &menu);
    QAction *actionAll         = addCheckBoxAction("Show All"               , mMarkAll, &menu);
    QAction *actionMatched     = addCheckBoxAction("Show Matched"           , mMarkMatched, &menu);
    QAction *actionSelected    = addCheckBoxAction("Show Selected"          , mMarkSelected, &menu);
    QAction *actionFast        = addCheckBoxAction("Show Fast"              , mMarkFast, &menu);

    QAction* selectedItem = menu.exec(p);

    if (selectedItem == actionDecAll)      mDecortatorAll = selectedItem->isChecked();
    if (selectedItem == actionDecMatched)  mDecortatorMatched = selectedItem->isChecked();
    if (selectedItem == actionDecSelected) mDecortatorSelected = selectedItem->isChecked();
    if (selectedItem == actionAll)         mMarkAll = selectedItem->isChecked();
    if (selectedItem == actionMatched)     mMarkMatched = selectedItem->isChecked();
    if (selectedItem == actionSelected)    mMarkSelected = selectedItem->isChecked();
    if (selectedItem == actionFast)        mMarkFast = selectedItem->isChecked();

    update();
}

void PointListEditImageWidgetUnited::paintDirectionArrows(QPainter &painter, int type)
{
    if (type & TOP_ARROW)
        painter.drawRect(QRect(
                         mOutputRect.left() + mOutputRect.width() / 2 - 5,
                         mOutputRect.top() + 5,
                         10, 5));

    if (type & BOTTOM_ARROW)
        painter.drawRect(QRect(
                         mOutputRect.left() + mOutputRect.width() / 2 - 5,
                         mOutputRect.bottom() - 15,
                         10, 5));

    if (type & LEFT_ARROW)
        painter.drawRect(QRect(
                         mOutputRect.left() + 5,
                         mOutputRect.top() + mOutputRect.height() / 2 - 10,
                         5, 10));

    if (type & RIGHT_ARROW)
        painter.drawRect(QRect(
                         mOutputRect.right() - 15,
                         mOutputRect.top() + mOutputRect.height() / 2 - 10,
                         5, 10));
}

/**
 *      *
 *    *  *
 *     *  *
 *      *  *  *
 *       *     *
 *          0
 *       *     *
 *         * *
 *
 **/
void PointListEditImageWidgetUnited::paintTarget(QPainter &painter, Vector2dd imageCoords, double len)
{
    painter.setPen(Qt::red);

    drawLine(painter, imageCoords + Vector2dd( len + 2, len + 1), imageCoords + Vector2dd( 2.0, 1.0));
    drawLine(painter, imageCoords + Vector2dd( len + 1, len + 2), imageCoords + Vector2dd( 1.0, 2.0));

    drawLine(painter, imageCoords - Vector2dd( len + 2, len + 1), imageCoords - Vector2dd( 2.0, 1.0));
    drawLine(painter, imageCoords - Vector2dd( len + 1, len + 2), imageCoords - Vector2dd( 1.0, 2.0));

    /*--*/

    drawLine(painter, imageCoords + Vector2dd( len + 2, -len - 1), imageCoords + Vector2dd( 2.0, -1.0));
    drawLine(painter, imageCoords + Vector2dd( len + 1, -len - 2), imageCoords + Vector2dd( 1.0, -2.0));

    drawLine(painter, imageCoords - Vector2dd( len + 2, -len - 1), imageCoords - Vector2dd( 2.0, -1.0));
    drawLine(painter, imageCoords - Vector2dd( len + 1, -len - 2), imageCoords - Vector2dd( 1.0, -2.0));

}



void PointListEditImageWidgetUnited::childRepaint(QPaintEvent * /*event*/, QWidget *who)
{
    //AdvancedImageWidget::childRepaint(event, who);
    if (mImage.isNull())
        return;

    if (mObservationListModel == NULL)
        return;

    /* Now the points */
    QPainter painter(who);
    AdvancedImageWidget::repaintImage(painter);

    int rows = (int)mObservationListModel->getPointCount();


    bool shortCut = mMarkFast && mMarkAll;
    /* This is the fastest way to draw, even outer cycle is brought invards */
    if (shortCut)
    {
        PreciseTimer timer = PreciseTimer::currentTime();
        painter.setPen(Qt::yellow);
        Matrix33 curT = currentTransformMatrix();

        for (int i = 0; i < rows; i ++)
        {
            /* TODO: Profiler says this call is very long... */
            Vector2dd point = mObservationListModel->getPoint(i);
            if (!mInputRect.contains(fround(point.x()), fround(point.y())))
                continue;

            drawCircle(painter, curT * point, 2);
        }
        uint64_t delay = timer.usecsToNow();
        qDebug() << "Point draw delay is:" << (double)delay / rows << " usec" << endl;

    } else {
        for (int i = 0; i < rows; i ++)
        {
            Vector2dd point = mObservationListModel->getPoint(i);
            Vector2dd imageCoords = imageToWidgetF(point);

            bool isSelected = false;
            if (selectionModel != NULL)
            {
                isSelected = selectionModel->isRowSelected(i, QModelIndex());
            } else {
                isSelected = (i == mSelectedPoint);
            }

            if (mDecortatorAll || (isSelected && mDecortatorSelected))
            {
                QTransform old = painter.transform();
                Matrix33 matrix = currentTransformMatrix();
                QTransform transform = Core2Qt::QTransformFromMatrix(matrix);
                painter.setTransform(transform, true);

                DrawDelegate *delegate = mObservationListModel->getDrawDelegate(i);
                if (delegate != NULL) {
                    delegate->drawAt(painter, point, isSelected);
                }
                painter.setTransform(old);
            }

            if (mMarkAll)
            {
                painter.setPen(Qt::yellow);
                drawCircle(painter, imageCoords, 5);
                painter.setPen(Qt::blue);
                drawCircle(painter, imageCoords, 10);

                if (mAddInfoButton->isChecked())
                {
                    QString meta = mObservationListModel->getMeta(i);
                    QPointF pos = Core2Qt::QPointFromVector2dd(imageCoords + Vector2dd(5, -10));
                    painter.setPen(Qt::black);
                    painter.drawText(pos, meta);
                    pos += QPointF(1,1);
                    painter.setPen(Qt::white);
                    painter.drawText(pos, meta);
                }
                int flags = NONE_ARROW;

                if (isSelected)
                {
                    painter.setPen(Qt::red);
                    drawCircle(painter, imageCoords, 7);

                    imageCoords = imageToWidgetF(widgetToImageF(imageCoords));
                    painter.setPen(Qt::cyan);
                    drawCircle(painter, imageCoords, 3);

                    if (imageCoords.x() < mOutputRect.left ()) flags |= LEFT_ARROW;
                    if (imageCoords.x() > mOutputRect.right()) flags |= RIGHT_ARROW;

                    if (imageCoords.y() < mOutputRect.top   ()) flags |= TOP_ARROW;
                    if (imageCoords.y() > mOutputRect.bottom()) flags |= BOTTOM_ARROW;
                }

                painter.setBrush(Qt::red);
                painter.setPen(Qt::blue);
                paintDirectionArrows(painter, flags);
                painter.setBrush(Qt::NoBrush);
            }
        }
    }

    /* Draw additional points */
    for (Vector2dd &point : pointList)
    {
        Vector2dd imageCoords = imageToWidgetF(point);

        /*painter.setPen(Qt::green);
        drawLine(painter, imageCoords - Vector2dd( 4.0, 4.0), imageCoords + Vector2dd(4.0,  4.0));
        drawLine(painter, imageCoords - Vector2dd(-4.0, 4.0), imageCoords + Vector2dd(-4.0, 4.0));*/
        paintTarget(painter, imageCoords, 10);
    }

    AdvancedImageWidget::repaintTools(painter);
}

void PointListEditImageWidgetUnited::toolButtonReleased(QWidget *button)
{
    if (mObservationListModel == nullptr)
        return;

    mUi->widget->unsetCursor();
    AdvancedImageWidget::toolButtonReleased(button);

    if (button == mMoveButton)
    {
        qDebug() << "Move Button";
        mCurrentToolClass = (ToolClass)MOVE_POINT_TOOL;
        mUi->widget->setCursor(Qt::ArrowCursor);
    }
    else if (button == mAddButton)
    {
        qDebug() << "Add Button";
        //mCurrentToolClass = (ToolClass)ADD_POINT_TOOL;
        mObservationListModel->appendPoint();
        mSelectedPoint = (int)mObservationListModel->getPointCount() - 1;
        /*if (selectionModel != NULL) {
            QItemSelection::Se
            selectionModel->select(QModelIndex(mSelectedPoint, 0), QI);
        }*/

        mObservationListModel->setPoint(mSelectedPoint, Qt2Core::Vector2ddFromQPoint(mZoomCenter));
        mUi->widget->update();
    }
    else if (button == mDeleteButton)
    {
        qDebug() << "Delete Button";
        if (mSelectedPoint >= 0)
        {
            mObservationListModel->deletePoint(mSelectedPoint);
            mSelectedPoint = -1;
        }
        mUi->widget->update();
    }
    else if (button == mAddInfoButton)
    {
        mUi->widget->update();
    }
}

int PointListEditImageWidgetUnited::findClosest(Vector2dd imagePoint, double limitDistance )
{
    int rows = (int)mObservationListModel->getPointCount();

    double bestDistance = limitDistance;
    int bestIndex = -1;

    for (int i = 0; i < rows; i ++)
    {
        Vector2dd currentPoint = mObservationListModel->getPoint(i);
        double dist = (imagePoint - currentPoint).l2Metric();
        if (dist < bestDistance) {
            bestDistance = dist;
            bestIndex = i;
        }
    }

    return bestIndex;
}

void PointListEditImageWidgetUnited::childMousePressed(QMouseEvent *event)
{
    AdvancedImageWidget::childMousePressed(event);

    if (!(event->buttons() & Qt::LeftButton))
        return;

    PaintToolClass tool = (PaintToolClass)mCurrentToolClass;

    if (tool == ADD_POINT_TOOL)
    {
    }

    if (tool == DEL_POINT_TOOL)
    {
    }

    if (tool == MOVE_POINT_TOOL)
    {
        Vector2dd releasePoint = Vector2dd(event->x(), event->y());
//        bool shiftPressed = event->modifiers().testFlag(Qt::ShiftModifier);

        Vector2dd imagePoint = widgetToImageF(releasePoint);
        Vector2dd shift = widgetToImageF(Vector2dd(5,5)) - widgetToImageF(Vector2dd(0,0));

        int selectedPoint = findClosest(imagePoint, shift.l2Metric());
        selectPoint(selectedPoint);
        mUi->widget->update();

        //event->type() == QMouseEvent::MouseButtonDblClick;
    }
}

void PointListEditImageWidgetUnited::childMouseMoved(QMouseEvent * event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        switch (mCurrentToolClass)
        {
            case MOVE_POINT_TOOL:
            {
                //   qDebug() << "Drag in selected tool";
                if (!mIsMouseLeftPressed)
                    break;

                Vector2dd dragStart    = widgetToImageF(Qt2Core::Vector2ddFromQPoint(mSelectionEnd));
                Vector2dd currentPoint = widgetToImageF(Qt2Core::Vector2ddFromQPoint(event->pos()));
                Vector2dd shift        = (dragStart - currentPoint);

                /*for (unsigned i = 0; i < mFeatures.mSelectedPoints.size(); i++)
                 {
                     mFeatures.mSelectedPoints[i]->position -= shift;
                 }*/
                if (mSelectedPoint >= 0)
                {
                    Vector2dd currentPoint = mObservationListModel->getPoint(mSelectedPoint);
                    currentPoint -= shift;
                    mObservationListModel->setPoint(mSelectedPoint, currentPoint);
                }

                mUi->widget->update();
            }
            default:
                break;
        }
    }

    AdvancedImageWidget::childMouseMoved(event);
}

void DrawKeypointAreaDelegate::drawAt(QPainter &painter, Vector2dd position, int state)
{
    if (observation == NULL)
        return;

    KeyPointArea &area = observation->keyPointArea;

    if (state) {
        painter.setPen(state ? Qt::blue : Qt::yellow);
        painter.drawEllipse(
            position.x() - area.size / 2 - 1,
            position.y() - area.size / 2 - 1,
            area.size + 2.0,
            area.size + 2.0);
    }

    RGBColor color = RGBColor::rainbow1(observation->accuracy.x());
    painter.setPen(color.color());

    painter.drawEllipse(
        position.x() - area.size / 2,
        position.y() - area.size / 2,
        area.size,
        area.size);
    painter.drawLine(
        position.x(),
        position.y(),
        cos(degToRad(area.angle)) * observation->keyPointArea.size / 2.0 + position.x(),
        sin(degToRad(area.angle)) * observation->keyPointArea.size / 2.0 + position.y()
    );


}
