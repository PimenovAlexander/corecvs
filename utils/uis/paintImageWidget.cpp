#include "paintImageWidget.h"
#include "ui_paintImageWidget.h"
#include "../corestructs/painterHelpers.h"
#include "core/buffers/rgb24/rgbColor.h"
#include "qtHelper.h"

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

    for (unsigned i = 0; i < mFeatures.mPoints.size(); i ++)
    {
        SelectableGeometryFeatures::Vertex *vertex = mFeatures.mPoints[i];
        painter.setPen(vertex->isSelected() ? Qt::red : Qt::green);
        if (vertex->ownerPath == NULL)
        {
            drawCircle(painter, imageToWidgetF(vertex->position), 5);
        }
        else
        {
            drawSquare(painter, imageToWidgetF(vertex->position), 5);
        }

        if (vertex->weight >= 0.0)
        {
            RGBColor color = RGBColor::rainbow1(vertex->weight);
            painter.setPen(QColor(color.r(), color.g(), color.b()));
            drawCircle(painter, imageToWidgetF(vertex->position), 7);
        }

    }

    for (unsigned i = 0; i < mFeatures.mPaths.size(); i++)
    {
        SelectableGeometryFeatures::VertexPath *path = mFeatures.mPaths[i];
        painter.setPen(path->mSelected ? Qt::yellow : Qt::green);
        for (unsigned i = 1; i < path->vertexes.size(); i++)
        {
            Vector2dd point1 = path->vertexes[i    ]->position;
            Vector2dd point2 = path->vertexes[i - 1]->position;
            drawLine(painter, imageToWidgetF(point1), imageToWidgetF(point2));
        }
    }
}

void PaintImageWidget::clear()
{
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
        /* Find points that are not in path yet */
        if (!mFeatures.hasSinglePointsSelected())
            return;

        SelectableGeometryFeatures::VertexPath *newPath = NULL;

        /* Find or create a path to add to*/
        if (mFeatures.mSelectedPaths.empty())
        {
            newPath = mFeatures.appendNewPath();
        }
        else
        {
            newPath = mFeatures.mSelectedPaths[0];
        }

        for (unsigned i = 0; i < mFeatures.mSelectedPoints.size(); i++)
        {
            if (!mFeatures.mSelectedPoints[i]->isInPath())
            {
                mFeatures.addVertexToPath(mFeatures.mSelectedPoints[i], newPath);
            }
        }

        mUi->widget->update();
    }
    else if (button == mDeletePointButton)
    {
        //mFeatures.mSelectedPoints.clear();

        while (!mFeatures.mSelectedPoints.empty()) {
            mFeatures.deleteVertex(mFeatures.mSelectedPoints[0]);
        }
        mUi->widget->update();
    }
}

void PaintImageWidget::childMousePressed(QMouseEvent *event)
{
    AdvancedImageWidget::childMousePressed(event);

    if (! (event->buttons() & Qt::LeftButton))
        return;

    if ((PaintImageWidget::PaintToolClass)mCurrentToolClass == SELECT_TOOL)
    {
        Vector2dd releasePoint = Vector2dd(event->x(), event->y());
        bool shiftPressed = event->modifiers().testFlag(Qt::ShiftModifier);
        mUi->widget->update();

        Vector2dd imagePoint = widgetToImageF(releasePoint);
        SelectableGeometryFeatures::Vertex *vertex = mFeatures.findClosest(imagePoint);
        if (vertex != NULL)
        {
            double dist = (releasePoint - imageToWidgetF(vertex->position)).l2Metric();
            if (dist < SELECTION_RADIUS)
            {
                if (!shiftPressed)
                {
                    mFeatures.deselectAll();
                }
                mFeatures.addSelection(vertex);
                if (vertex->isInPath())
                {
                    mFeatures.addSelection(vertex->ownerPath);
                }
                return;
            }
        }

        SelectableGeometryFeatures::Vertex *newVertex = mFeatures.appendNewVertex(imagePoint);
        if (!shiftPressed)
        {
            mFeatures.deselectAll();
        }
        mFeatures.addSelection(newVertex);
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
            if (!mIsMouseLeftPressed)
                break;

            Vector2dd dragStart    = widgetToImageF(Qt2Core::Vector2ddFromQPoint(mSelectionEnd));
            Vector2dd currentPoint = widgetToImageF(Qt2Core::Vector2ddFromQPoint(event->pos()));
            Vector2dd shift = (dragStart - currentPoint);

            for (unsigned i = 0; i < mFeatures.mSelectedPoints.size(); i++)
            {
                mFeatures.mSelectedPoints[i]->position -= shift;
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

QPointF PaintImageWidget::currentPoint()
{
    if (mFeatures.mSelectedPoints.empty())
    {
        return QPointF();
    }
    return Core2Qt::QPointFromVector2dd(mFeatures.mSelectedPoints.back()->position);
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

