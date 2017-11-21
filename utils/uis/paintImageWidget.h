#pragma once

#include <QWidget>

#include "core/alignment/selectableGeometryFeatures.h"
#include "advancedImageWidget.h"
#include "core/geometry/polygons.h"

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

public:
    /* Data that is manupulated by widget*/
    SelectableGeometryFeatures mFeatures;

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

