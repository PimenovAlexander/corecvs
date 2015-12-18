#include <QPainter>
#include <limits>

#include "global.h"

#include "g12Buffer.h"
#include "histogramwidget.h"
#include "ui_histogramwidget.h"

double const zoomSpeed = 1.3;
int const initialGridStep = 50;
int const gridDensity = 20;

HistogramWidget::HistogramWidget(QWidget *parent)
    : QWidget(parent)
    , mSelectedH(0)
    , mUi(new Ui::HistogramWidgetClass)
    , mUseMargin(false)
    , mHistogram(NULL)
    , mZoneStart(std::numeric_limits<int>::min())
    , mZoneEnd(std::numeric_limits<int>::min())
    , mPreZoneStart(std::numeric_limits<int>::min())
    , mClickZoneEnd(std::numeric_limits<int>::min())
    , mZoneSettingState(none)
    , mSettingZone(false)
    , mZoomFactor(1.0)
    , mFrameLeftBorder(0.0)
{
    mUi->setupUi(this);
}

HistogramWidget::~HistogramWidget()
{
    delete mUi;
}

void HistogramWidget::paintEvent(QPaintEvent * /*event */ )
{
    QPainter painter(this);
    painter.setPen(Qt::black);
    int h = size().height();

    if (mHistogram == NULL)
    {
        painter.drawText(0, 0, size().width(), size().height(), Qt::AlignHCenter | Qt::AlignVCenter, QString("NO Histogram"));
        return;
    }

    if (h == 0) {
        h = 1;
    }

    uint32_t globalMax = 0;

    unsigned const start = mUseMargin ? 0 : 1;
    unsigned const end   = (unsigned)mHistogram->data.size() - start - 1;
    unsigned x = start;

    globalMax = mHistogram->getMaximum(mUseMargin);

    double scaleY = static_cast<double>(h) / (globalMax + 1);

    drawGrid(painter, end - start, globalMax + 1);

    double const step = qMax(1.0 / mZoomFactor, 1.0);
    int const barWidth = qMax(mZoomFactor, 1.0);
    int selectedBar = floor(toAbsolute(mSelectedH));

    for (int i = mHistogram->min; i < mHistogram->max; ++i, ++x)
    {
        int max  =  0;
        int min  =  std::numeric_limits<int>::max();
        int sum  =  0;
        int count = 0;

        for (unsigned j = x; j < static_cast<unsigned>(x + step) && j <= end; j++)
        {
            int const val = mHistogram->data[j];
            sum += val;
            if (val > max)
                max = val;
            if (val < min)
                min = val;
            count++;
        }

        if (count == 0)
            continue;

        sum /= count;

        painter.setPen(Qt::blue);
        painter.setBrush(Qt::blue);
        painter.drawRect(toRelative(i), h - 1, barWidth, -(max * scaleY));
        painter.setPen(Qt::black);
        painter.setBrush(Qt::black);
        painter.drawRect(toRelative(i), h - 1, barWidth, -(sum * scaleY));
        painter.setPen(barWidth > 1 ? Qt::darkGreen : Qt::green);
        painter.setBrush(Qt::green);
        painter.drawRect(toRelative(i), h - 1, barWidth, -(min * scaleY));

        if (i == selectedBar)
        {
            painter.save();
            painter.setOpacity(0.3);
            painter.setPen(Qt::yellow);
            painter.setBrush(Qt::yellow);
            painter.drawRect(toRelative(i), h - 1, barWidth, - h + 2);
            painter.restore();
        }
    }

    int bar = 0;
    if (getBarByX(floor(toAbsolute(mSelectedH)), &bar))
    {
        painter.setPen(Qt::black);
        if (bar < (mHistogram->max - mHistogram->min) / 2)
            painter.drawText(mSelectedH, (h - 1) / 2, 200, 20, Qt::AlignLeft, QString("%1 %2").arg(bar).arg(mHistogram->getData(bar)));
        else
            painter.drawText(mSelectedH - 200, (h - 1) / 2, 200, 20, Qt::AlignRight, QString("%1 %2").arg(bar).arg(mHistogram->getData(bar)));
    }

    painter.save();
    painter.setOpacity(0.5);
    if (mPreZoneStart > std::numeric_limits<int>::min())
    {
        painter.setBrush(QColor(248, 168, 255));
        int const zoneStart = mZoneStart > mPreZoneStart ? mZoneStart : width();
        painter.drawRect(toRelative(mPreZoneStart), 0, toRelative(zoneStart) - toRelative(mPreZoneStart), h - 1);
    }

    if (mZoneStart > std::numeric_limits<int>::min())
    {
        int const zoneEnd = mZoneEnd > mZoneStart ? mZoneEnd : width();
        QLinearGradient gradient(QPointF(toRelative(mZoneStart), 0), QPointF(toRelative(mZoneEnd), 0));

        gradient.setColorAt(0, Qt::yellow);
        gradient.setColorAt(0.5, Qt::green);
        gradient.setColorAt(1, Qt::blue);
        QBrush brush(gradient);

        painter.setBrush(brush);
        painter.drawRect(toRelative(mZoneStart), 0, toRelative(zoneEnd) - toRelative(mZoneStart), h - 1);
    }

    if (mZoneEnd > std::numeric_limits<int>::min())
    {
        int const clickZoneEnd = mClickZoneEnd > mZoneEnd ? mClickZoneEnd : width();

        painter.setBrush(QColor(128, 0, 0));
        painter.drawRect(toRelative(mZoneEnd), 0, toRelative(clickZoneEnd) - toRelative(mZoneEnd), h - 1);
    }
    painter.restore();
}

void HistogramWidget::drawGrid(QPainter &painter, int scaleX, int scaleY)
{
    Q_UNUSED(scaleX);

    int const h = size().height();
    double dy = static_cast<double>(h) / scaleY;

    while (dy < 10)
        dy *= 10;

    double x = mHistogram->min;
    double y = 0;

    double gridStep = initialGridStep;

    while (width() / mZoomFactor / gridStep < gridDensity)
        gridStep /= 2;

    QPen pen(Qt::DotLine);
    for (int j = 0; x < mHistogram->max; x += gridStep, j++)
    {
        pen.setColor(j % 5 ? Qt::gray : Qt::black);
        painter.setPen(pen);
        painter.drawLine(toRelative(x), 0, toRelative(x), h);
    }

    for (int i = 0; y < h; y += dy, i++)
    {
        pen.setColor(i % 5 ? Qt::gray : Qt::black);
        painter.setPen(pen);
        double const start = qMax(0.0, toRelative(mHistogram->min));
        double const end = qMin(static_cast<double>(size().width()), toRelative(mHistogram->max));
        painter.drawLine(start, y, end, y);
    }
}

void HistogramWidget::notifyHistogramChange(Histogram *histogram, bool useMargin)
{
//    mSelectedH = 0;
    bool needRecalculateFrame = (mHistogram == NULL);
    mHistogram = histogram;
    mUseMargin = useMargin;
    if (needRecalculateFrame && mHistogram != NULL) {
        recalculateFrame();
    }
    update();
}

void HistogramWidget::recalculateFrame()
{
    /* Searching right and left borders */
    int min = std::numeric_limits<int>::max();
    int max = std::numeric_limits<int>::min();
    for (int i = mHistogram->min; i < mHistogram->max; ++i)
    {
        if (min > i && mHistogram->data[i - mHistogram->min] > 0) {
            min = i;
        }
        if (max < i && mHistogram->data[i - mHistogram->min] > 0) {
            max = i;
        }
    }

    mFrameLeftBorder = min;
    if (max != min) {
        mZoomFactor = width() / static_cast<double>(max - min + 1);
    }
}

bool HistogramWidget::getBarByX(int x, int *bar)
{
    if (mHistogram == NULL || x < mHistogram->min || x >= mHistogram->max)
        return false;

    *bar = x;
    return true;
}

void HistogramWidget::mousePressEvent(QMouseEvent * event)
{
    if (event->buttons() & Qt::RightButton & Qt::LeftButton)
    {
        if (mHistogram)
        {
            mPreZoneStart = mHistogram->max;
            mZoneStart = mHistogram->max;
            mZoneEnd = mHistogram->max;
            mClickZoneEnd = mHistogram->max;

            emit preZoneChanged(mHistogram->max);
            emit zoneStartChanged(mHistogram->max);
            emit zoneEndChanged(mHistogram->max);
            emit clickZoneEndChanged(mHistogram->max);
        }
        update();
        return;
    }

    if (event->button() == Qt::LeftButton) {
        int const x = floor(toAbsolute(event->x()));
        mSettingZone = true;
        setZone(x);
        int bar = 0;
        if (getBarByX(x, &bar)) {
            emit histogramBarClicked(bar);
        } else {
            printf("Bar out of range %d\n", bar);
        }
    }
}

void HistogramWidget::mouseMoveEvent(QMouseEvent *event)
{
    int const x = event->x();

    if (x > 0 && x < size().width())
    {
        mSelectedH = x;

        if (mSettingZone)
            setZone(toAbsolute(x));

#if QT_VERSION >= 0x040700
        if (event->buttons() & (Qt::MiddleButton | Qt::RightButton))
#else
        if (event->buttons() & (Qt::MidButton | Qt::RightButton))
#endif
            {
            if (mPrevMousePos != QPointF())
            {
                double const delta = (event->localPos().x() - mPrevMousePos.x()) / mZoomFactor;
                if (!(mFrameLeftBorder < mHistogram->min && delta > 0)
                    && !(mFrameLeftBorder + toAbsolute(width()) > mHistogram->max && delta < 0))
                    {
                    mFrameLeftBorder -= delta;
                }
            }
            mPrevMousePos = event->localPos();
        }

        update();
    }
}

void HistogramWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    mSettingZone = false;
    mPrevMousePos = QPointF();
}

void HistogramWidget::setZoneSettingState(ZoneSettingState state)
{
    mZoneSettingState = state;
}

void HistogramWidget::setZone(int zoneBorder)
{
    int recalculated = 0;

    switch (mZoneSettingState) {
    case none: return;
    case preZone:
        {
            if (zoneBorder > mZoneStart)
                return;
            mPreZoneStart = zoneBorder;
            if (getBarByX(mPreZoneStart, &recalculated))
                emit preZoneChanged(recalculated);
            break;
        }
    case zoneStart:
        {
            if (zoneBorder < mPreZoneStart || zoneBorder > mZoneEnd)
                return;
            mZoneStart = zoneBorder;
            if (getBarByX(mZoneStart, &recalculated))
                emit zoneStartChanged(recalculated);
            break;
        }
    case zoneEnd:
        {
            if (zoneBorder < mZoneStart || zoneBorder > mClickZoneEnd)
                return;
            mZoneEnd = zoneBorder;
            if (getBarByX(mZoneEnd, &recalculated))
                emit zoneEndChanged(recalculated);
            break;
        }
    case clickEnd:
        {
            if (zoneBorder < mZoneEnd)
                return;
            mClickZoneEnd = zoneBorder;
            if (getBarByX(mClickZoneEnd, &recalculated))
                emit clickZoneEndChanged(recalculated);
            break;
        }
    }
    update();
}

void HistogramWidget::setZones(int zoneStart, int zoneEnd, int preZoneStart, int clickZoneEnd)
{
    if (mSettingZone || preZoneStart > zoneStart
        || zoneStart > zoneEnd || zoneEnd > clickZoneEnd)
        return;
    mZoneStart = zoneStart;
    mZoneEnd = zoneEnd;
    mPreZoneStart = preZoneStart;
    mClickZoneEnd = clickZoneEnd;
    update();
}

void HistogramWidget::resizeEvent(QResizeEvent * /*event*/)
{
}

void HistogramWidget::wheelEvent(QWheelEvent *event)
{
    if (event->delta() > 0)
        zoomIn();
    else
        zoomOut();
}

double HistogramWidget::toAbsolute(double const &x) const
{
    return x / mZoomFactor + mFrameLeftBorder;
}

double HistogramWidget::toRelative(double const &x) const
{
    return (x - mFrameLeftBorder) * mZoomFactor;
}

void HistogramWidget::zoomIn()
{
    double const mOldZoomFactor = mZoomFactor;
    mZoomFactor *= zoomSpeed;
    mFrameLeftBorder += static_cast<double>(mSelectedH) / mOldZoomFactor - static_cast<double>(mSelectedH) / mZoomFactor;
    update();
}

void HistogramWidget::zoomOut()
{
    double const mOldZoomFactor = mZoomFactor;
    mZoomFactor /= zoomSpeed;
    mFrameLeftBorder += static_cast<double>(mSelectedH) / mOldZoomFactor - static_cast<double>(mSelectedH) / mZoomFactor;
    update();
}
