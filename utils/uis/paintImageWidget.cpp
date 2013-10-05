#include "paintImageWidget.h"
#include "ui_paintImageWidget.h"
#include "../corestructs/painterHelpers.h"
#include "rgbColor.h"

double PaintImageWidget::SELECTION_RADIUS = 5.0;

PaintImageWidget::PaintImageWidget(QWidget *parent) :
    AdvancedImageWidget(parent),
    ui(new Ui::PaintImageWidget)
{
    mSelectButton      = addToolButton("Select",            QIcon(":/new/prefix1/select_by_color.png"));
    mAddPointButton    = addToolButton("Add Point To Path", QIcon(":/new/prefix1/vector_add.png"), false);
    mDeletePointButton = addToolButton("Delete Point",      QIcon(":/new/prefix1/vector_delete.png"), false);

    ui->setupUi(this);
}

void PaintImageWidget::childRepaint(QPaintEvent *event, QWidget *who)
{
    AdvancedImageWidget::childRepaint(event, who);
    if (mImage.isNull())
    {
        return;
    }

    /* Now the points */
    QPainter painter(who);

    for (int i = 0; i < mPoints.size(); i ++)
    {
        Vertex *vertex = &mPoints[i];
        painter.setPen(vertex->isSelected ? Qt::red : Qt::green);
        if (vertex->ownerPath == NULL)
        {
            drawCircle(painter, imageToWidgetF(mPoints[i].position), 5);
        }
        else
        {
            drawSquare(painter, imageToWidgetF(mPoints[i].position), 5);
        }

        if (vertex->weight >= 0.0)
        {
            RGBColor color = RGBColor::rainbow1(vertex->weight);
            painter.setPen(QColor(color.r(), color.g(), color.b()));
            drawCircle(painter, imageToWidgetF(mPoints[i].position), 7);
        }

    }

    for (int i = 0; i < mPaths.size(); i++)
    {
        VertexPath *path = &mPaths[i];
        painter.setPen(path->isSelected ? Qt::yellow : Qt::green);
        for (int i = 1; i < path->vertexes.size(); i++)
        {
            Vector2dd point1 = path->vertexes[i    ]->position;
            Vector2dd point2 = path->vertexes[i - 1]->position;
            drawLine(painter, imageToWidgetF(point1), imageToWidgetF(point2));
        }
    }
}

void PaintImageWidget::clear()
{
    mSelectedPaths.clear();
    mSelectedPoints.clear();

    mPaths.clear();
    mPoints.clear();
    update();
}

PaintImageWidget::~PaintImageWidget()
{
    delete_safe(ui);
}

void PaintImageWidget::clickOnToolButton(QWidget const *button, QWidget const *targetButton, ToolButtonType buttonType)
{
    if (button == targetButton)
    {
        if (mEventFromOtherWidget)
            mEventFromOtherWidget = false;
        else
            emit toolButtonClicked(buttonType);
    }
}

void PaintImageWidget::toolButtonReleased(QWidget *button)
{
    AdvancedImageWidget::toolButtonReleased(button);

    if (button == mUi->panButton)
    {
        clickOnToolButton(button, mUi->panButton, panButton);
    }
    else if (button == mUi->selectZoomButton)
    {
        clickOnToolButton(button, mUi->selectZoomButton, selectZoomButton);
    }
    else if (button == mUi->infoButton)
    {
        clickOnToolButton(button, mUi->infoButton, infoButton);
    }
    else if (button == mSelectButton)
    {
        mCurrentToolClass = (ToolClass)SELECT_TOOL;
        mUi->widget->setCursor(Qt::ArrowCursor);

        clickOnToolButton(button, mSelectButton, selectButton);
    }
    else if (button == mAddPointButton)
    {
        int i = 0;
        for (; i < mSelectedPoints.size(); i++)
        {
            if (!mSelectedPoints[i]->isInPath()) {
                break;
            }
        }
        if (i == mSelectedPoints.size()) {
            return;
        }

        VertexPath *newPath = NULL;

        if (mSelectedPaths.isEmpty())
        {
            mPaths.append(VertexPath());
            newPath = &mPaths[mPaths.size() - 1];
        }
        else
        {
            newPath = mSelectedPaths[0];
        }

        for (int i = 0; i < mSelectedPoints.size(); i++)
        {
            if (!mSelectedPoints[i]->isInPath())
            {
                addVertexToPath(mSelectedPoints[i], newPath);
            }
        }

        mUi->widget->update();
    }
    else if (button == mDeletePointButton)
    {
        mSelectedPoints.clear();

        for (int i = 0; i < mPoints.size();)
        {
            if (mPoints[i].isSelected)
            {
                deleteVertex(i);
            }
            else
            {
                i++;
            }
        }

        mUi->widget->update();
    }
}

void PaintImageWidget::childMousePressed(QMouseEvent *event)
{
    AdvancedImageWidget::childMousePressed(event);

    if ((PaintImageWidget::PaintToolClass)mCurrentToolClass == SELECT_TOOL)
    {
        Vector2dd releasePoint = Vector2dd(event->x(), event->y());
        bool shiftPressed = event->modifiers().testFlag(Qt::ShiftModifier);
        mUi->widget->update();

        Vector2dd imagePoint = widgetToImageF(releasePoint);
        Vertex *vertex = findClosest(imagePoint);
        if (vertex != NULL)
        {
            double dist = (releasePoint - imageToWidgetF(vertex->position)).l2Metric();
            if (dist < SELECTION_RADIUS)
            {
                if (!shiftPressed)
                {
                    deselectAllPoints();
                    deselectAllPath();
                }
                addSelection(*vertex);
                if (vertex->isInPath())
                {
                    addSelection(*vertex->ownerPath);
                }
                return;
            }
        }

        mPoints.append(Vertex(widgetToImageF(releasePoint)));
        if (!shiftPressed)
        {
            deselectAllPoints();
            deselectAllPath();
        }
        addSelection(mPoints[mPoints.size() - 1]);
    }
}

void PaintImageWidget::childMouseMoved(QMouseEvent * event)
{
    //qDebug() << "Doing the drag";
    switch (mCurrentToolClass)
    {
    case SELECT_TOOL:
    {
        //   qDebug() << "Drag in selected tool";
        if (!mIsMousePressed)
            break;

        Vector2dd dragStart    = widgetToImageF(Qt2Core::Vector2ddFromQPoint(mSelectionEnd));
        Vector2dd currentPoint = widgetToImageF(Qt2Core::Vector2ddFromQPoint(event->pos()));
        Vector2dd shift = (dragStart - currentPoint);

        for (int i = 0; i < mSelectedPoints.size(); i++)
        {
            mSelectedPoints[i]->position -= shift;
        }
        mUi->widget->update();
    }
    default:
        break;
    }
    AdvancedImageWidget::childMouseMoved(event);
}

void PaintImageWidget::childMouseReleased(QMouseEvent *event)
{
    AdvancedImageWidget::childMouseReleased(event);
}

void PaintImageWidget::clickToolButton(PaintImageWidget::ToolButtonType type)
{
    mEventFromOtherWidget = true;

    switch (type)
    {
    case selectButton: mSelectButton->click(); break;
    case panButton: mUi->panButton->click(); break;
    case infoButton: mUi->infoButton->click(); break;
    case selectZoomButton: mUi->selectZoomButton->click(); break;
    default: break;
    }
}

void PaintImageWidget::addVertex(const Vertex &vertex)
{
    mPoints.append(vertex);
    mPoints.last().ownerPath = NULL;
}

void PaintImageWidget::addVertex(const Vector2dd &point)
{
    addVertex(Vertex(point));
}

void PaintImageWidget::deleteVertex(Vertex *vertex)
{
    for (int i = 0; i < mPoints.size(); i++)
    {
        if (&mPoints[i] == vertex)
        {
            deleteVertex(i);
            return;
        }
    }
}

void PaintImageWidget::deleteVertex(const Vector2dd &point)
{
    QList<Vertex*> toDelete;

    for (int i = 0; i < mPoints.size(); i++)
    {
        if (mPoints[i].position == point)
        {
            toDelete.append(&mPoints[i]);
        }
    }

    foreach (Vertex *vertex, toDelete)
    {
        removeSelection(*vertex);
        deleteVertex(vertex);
    }
    mUi->widget->update();
}

void PaintImageWidget::deleteVertex(int id)
{
    VertexPath *ownerPath = mPoints[id].ownerPath;
    do
    {
        if (ownerPath == NULL)
        {
            break;
        }

        ownerPath->vertexes.removeAll(&(mPoints[id]));
        if (!ownerPath->vertexes.isEmpty())
        {
            break;
        }

        mSelectedPaths.removeAll(ownerPath);
        for (int i = 0; i < mPaths.size(); i++)
        {
            if (&mPaths[i] == ownerPath)
            {
                mPaths.removeAt(i);
                break;
            }
        }
    } while (false);
    mPoints.removeAt(id);
    mUi->widget->update();
}

QPointF PaintImageWidget::currentPoint()
{
    if (mSelectedPoints.isEmpty())
    {
        return QPointF();
    }
    return Core2Qt::QPointFromVector2dd(mSelectedPoints.last()->position);
}

void PaintImageWidget::setSelectedPoint(const QPointF &/*point*/, bool /*isSelected*/)
{
    /* for (int i = 0; i < mPoints.count(); i ++)
    {
        if ((point - mPoints[i].first).manhattanLength() < epsilon)
        {
            mPoints[i].second = isSelected;
        }
    }*/
    update();
}

vector<vector<Vector2dd> > PaintImageWidget::getPaths()
{
    vector<vector<Vector2dd> >straights;
    for (int i = 0; i < mPaths.size(); i++)
    {
        PaintImageWidget::VertexPath *vertexPath = &mPaths[i];
        vector<Vector2dd> path;
        for (int j = 0; j < vertexPath->vertexes.size(); j++)
        {
            path.push_back(vertexPath->vertexes[j]->position);
        }
        straights.push_back(path);
    }
    return straights;
}


Vector2dd PaintImageWidget::widgetToImageF(const Vector2dd &p)
{
    return Qt2Core::Vector2ddFromQPointF(AdvancedImageWidget::widgetToImageF(Core2Qt::QPointFromVector2dd(p)));
}

Vector2dd PaintImageWidget::imageToWidgetF(const Vector2dd &p)
{
    return Qt2Core::Vector2ddFromQPointF(AdvancedImageWidget::imageToWidgetF(Core2Qt::QPointFromVector2dd(p)));
}
