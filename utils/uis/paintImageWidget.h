#pragma once

#include <QWidget>

#include "advancedImageWidget.h"
#include "polygons.h"

namespace Ui {
class PaintImageWidget;
}

class PaintImageWidget : public AdvancedImageWidget
{
    Q_OBJECT

    static double SELECTION_RADIUS;

    enum PaintToolClass
    {
        SELECT_TOOL = TOOL_CLASS_LAST,
        ADD_POINT_TOOL,
        DEL_POINT_TOOL,
        PAINT_TOOL_CLASS_LAST
    };

    enum ToolButtonType
    {
        selectButton,
        panButton,
        infoButton,
        selectZoomButton
    };



public:
    explicit PaintImageWidget(QWidget *parent = 0);
    ~PaintImageWidget();
    void clear();
    void setCurrentPoint(QPointF const &currentPoint);
    QPointF currentPoint();
    void setSelectedPoint(QPointF const &point, bool isSelected);

    vector<vector<Vector2dd> > getPaths();

    Vector2dd widgetToImageF(const Vector2dd &p);
    Vector2dd imageToWidgetF(const Vector2dd &p);

public slots:
    virtual void toolButtonReleased(QWidget *button);

    virtual void childRepaint(QPaintEvent *event, QWidget *who);
    virtual void childMouseMoved   (QMouseEvent  *);
    virtual void childMousePressed (QMouseEvent  *);
    virtual void childMouseReleased(QMouseEvent  *);

    void clickToolButton(ToolButtonType type);

private:
    Ui::PaintImageWidget *ui;

    void clickOnToolButton(const QWidget *button, const QWidget *targetButton, ToolButtonType type);

    bool mEventFromOtherWidget;

    // TODO: Separate this and hide
public:
    struct VertexPath;

    struct Vertex
    {
        bool        isSelected;
        Vector2dd   position;
        double      weight;
        VertexPath *ownerPath;

        explicit Vertex(const Vector2dd &_position) :
            isSelected(false),
            position(_position),
            weight(-1.0),
            ownerPath(NULL)
        {}

        bool isInPath() {return (ownerPath != NULL);};
    };

    typedef Vector2dd * TargetPoint;

    /** Points **/
    QList<Vertex> mPoints;
    QList<Vertex*> mSelectedPoints;

    void addSelection(Vertex &vertex)
    {
        mSelectedPoints.append(&vertex);
        vertex.isSelected = true;
    }

    void removeSelection(Vertex &vertex)
    {
        mSelectedPoints.removeOne(&vertex);
        vertex.isSelected = false;
    }

    void deselectAllPoints()
    {
        for (int i = 0; i < mSelectedPoints.size(); i++)
        {
            mSelectedPoints[i]->isSelected = false;
        }
        mSelectedPoints.clear();
    }

    Vertex *findClosest(const Vector2dd &position)
    {
        double minDist = numeric_limits<double>::max();
        Vertex *result =  NULL;
        for (int i = 0; i < mPoints.size(); i++)
        {
            double dist = (position - mPoints[i].position).l2Metric();
            if (dist < minDist)
            {
                minDist = dist;
                result = &mPoints[i];
            }
        }
        return result;
    }

    struct VertexPath
    {
        bool isSelected;
        QList<Vertex *> vertexes;
    };

    /** Pathes **/
    QList<VertexPath> mPaths;
    QList<VertexPath *> mSelectedPaths;

    void deselectAllPath()
    {
        for (int i = 0; i < mSelectedPaths.size(); i++)
        {
            mSelectedPaths[i]->isSelected = false;
        }
        mSelectedPaths.clear();
    }

    void addVertexToPath(Vertex *vertex, VertexPath *path)
    {
        path->vertexes.append(vertex);
        vertex->ownerPath = path;
    }

    void addSelection(VertexPath &path)
    {
        mSelectedPaths.append(&path);
        path.isSelected = true;
    }

    void deleteVertex(Vertex *vertex);
    void deleteVertex(const Vector2dd &point);
    void deleteVertex(int num);

    void addVertex(const Vertex &vertex);
    void addVertex(const Vector2dd &point);

    void drawPath(QPainter *painter, const PointPath &path);

    /* UI */
    QToolButton *mSelectButton;
    QToolButton *mAddPointButton;
    QToolButton *mDeletePointButton;

signals:
    void newPointFSelected(int, QPointF);
    void existedPointSelected(int, QPointF);
    void editPoint(QPointF, QPointF);
    void newPolygonPointSelected(int, QPointF);

    void toolButtonClicked(ToolButtonType type);

    void vectorDataChanged();
};

/*EOF*/

