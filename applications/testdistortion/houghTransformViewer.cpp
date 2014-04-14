#include "houghTransformViewer.h"

void HoughTransformViewer::childMouseMoved(QMouseEvent * event)
{
    AdvancedImageWidget::childMouseMoved(event);

    QPoint imagePoint = widgetToImage(event->pos());

    if ((mCurrentToolClass == INFO_TOOL) && AdvancedImageWidget::mImage->valid(imagePoint))
    {
        pointSelected(imagePoint);
    }

}
