/**
 * \file advancedImageWidget.h
 *
 * \date Feb 27, 2012
 * \author Olga Sapershteyn
 * \author Alexander Pimenov
 */
#include <QtCore/QDebug>
#include <QPainter>
#include <QFileDialog>
#include <QMouseEvent>
#include <QToolTip>

#include "core/utils/global.h"

#include "core/buffers/rgb24/rgbColor.h"
#include "advancedImageWidget.h"
#include "saveFlowSettings.h"
#include "core/math/mathUtils.h"
#include "qtHelper.h"

AdvancedImageWidget::AdvancedImageWidget(QWidget *parent, bool showHeader)
  : ViAreaWidget(parent, false)
  , mImage(NULL)
  , mIsFreezed(false)
    /* Icons */
  , mFreezeIcon  (QString::fromUtf8(":/new/prefix1/pause.png"))
  , mContinueIcon(QString::fromUtf8(":/new/prefix1/play.png" ))
  , mSaveProcStarted(false)
  , mImageNumber(0)
  , mImageSavePath("")
  , mSaveDialog(NULL)
  , mUi(new Ui::advancedImageWidget)
  , mZoomCenter(-1,-1)
  , mIsMouseLeftPressed(false)
  , mIsMouseRightPressed(false)
  , mRightMouseButtonDrag(false)
  , mResizeCache(NULL)
  , mCurrentToolClass(NO_TOOL)
  , mCurrentSelectionButton(0)
  , mCurrentPointButton(0)
  , mCurrentLineButton(0)
  , mImageSize(QSize())
{
    mUi->setupUi(this);
    mSaveDialog = new SaveFlowSettings();
    mSaveDialog->setAccessibleName("Save flow setting");
    mSaveDialog->setWindowTitle("Save flow setting");
    mSaveDialog->setGeometry(0, 0, 400, 400);

    mUi->fileNamelabel ->hide();
    mUi->fileValuelabel->hide();

    if (!showHeader)
    {
        mUi->frame_2 ->hide();
        mUi->widget_2->hide();
    }

    //mUi->widget->setCursor(Qt::OpenHandCursor);

    connect(mUi->widget, SIGNAL(askParentRepaint(QPaintEvent*,QWidget*)), this, SLOT(childRepaint(QPaintEvent*,QWidget*)));
    connect(mSaveDialog->getUi()->pushButton,SIGNAL(released()), this, SLOT(startSaveFlow()) );


    connect(mUi->widget, SIGNAL(notifyParentWheelEvent(QWheelEvent *)),   this, SLOT(childWheelEvent   (QWheelEvent *)));
    connect(mUi->widget, SIGNAL(notifyParentMouseMove(QMouseEvent *)),    this, SLOT(childMouseMoved   (QMouseEvent *)));
    connect(mUi->widget, SIGNAL(notifyParentMousePress(QMouseEvent *)),   this, SLOT(childMousePressed (QMouseEvent *)));
    connect(mUi->widget, SIGNAL(notifyParentMouseRelease(QMouseEvent *)), this, SLOT(childMouseReleased(QMouseEvent *)));
    connect(mUi->widget, SIGNAL(notifyParentResize(QResizeEvent *)),      this, SLOT(childResized      (QResizeEvent *)));

    connect(mUi->infoButton,       SIGNAL(released()), &mToolMapper, SLOT(map()));
    mToolMapper.setMapping(mUi->infoButton, mUi->infoButton);
    connect(mUi->panButton,        SIGNAL(released()), &mToolMapper, SLOT(map()));
    mToolMapper.setMapping(mUi->panButton, mUi->panButton);
    connect(mUi->selectZoomButton, SIGNAL(released()), &mToolMapper, SLOT(map()));
    mToolMapper.setMapping(mUi->selectZoomButton, mUi->selectZoomButton);

    connect(&mToolMapper, SIGNAL(mapped(QWidget *)), this, SLOT(toolButtonReleased(QWidget *)));
    mCurrentToolClass = NO_TOOL;

  //  qDebug("AdvancedImageWidget::AdvancedImageWidget(%p): called", this);
}

AdvancedImageWidget::~AdvancedImageWidget()
{
    delete_safe(mUi);
    delete_safe(mSaveDialog);
    delete_safe(mResizeCache);

   // qDebug("AdvancedImageWidget::~AdvancedImageWidget(%p): called", this);
}

void AdvancedImageWidget::setCollapseTitle(bool collapse)
{
    QList<int> sizes;
    if (collapse) {
        sizes << 0 << 1;
    } else {
        sizes << 1 << 1;
    }

    mUi->splitter->setSizes(sizes);
}

void AdvancedImageWidget::setImage(QSharedPointer<QImage> newImage)
{
    if (mIsFreezed)
        return;

    /*if (newImage.isNull())
        return;*/

    mImage = newImage;

    delete_safe(mResizeCache);

    if (mSaveProcStarted)
    {
        mResizeCache = new QImage(mOutputRect.size(), QImage::Format_RGB32);
        mResizeCache->fill(0);
        QPainter cachePainter(mResizeCache);
        drawResized(cachePainter);
        cachePainter.setPen(QColor(Qt::red));
        cachePainter.drawRect(mResizeCache->rect().adjusted(0,0,-1,-1));

        cachePainter.end();
        saveFlowImage(mResizeCache);
    }

    if ((!mImage.isNull() && mImageSize != mImage->size()) || (mZoomCenter == QPoint(-1,-1)))
    {
        if (mImage.isNull()) {
            mImageSize = QSize();
        } else {
            mImageSize = mImage->size();
        }
        recalculateZoomCenter();
    }
    recomputeRects();
    forceUpdate();
}

void AdvancedImageWidget::drawResized(QPainter &painter)
{
    //qDebug("AdvancedImageWidget::drawResized():called");
    if (mImage.isNull())
    {
        qDebug("AdvancedImageWidget::drawResized():image is null");

        painter.drawText(0,0, this->width(), this->height(), Qt::AlignHCenter | Qt::AlignVCenter, QString("NO ACTIVE IMAGE"));
        return;
    }

    if (mUi->rotationComboBox->currentIndex() == 0)
    {
        painter.drawImage(mOutputRect, *mImage, mInputRect);
        return;
    }

//    qDebug() << "Input"  << mInputRect;
//    qDebug() << "Output" << mOutputRect;


    Matrix33 matrix = currentTransformMatrix();
//    cout << "Transfrom Matrix:\n" << matrix << std::endl << std::flush;
    QTransform transform = Core2Qt::QTransformFromMatrix(matrix);
//    qDebug() << "QTransfrom" << transform;



    QTransform old = painter.transform();
    painter.setTransform(transform, false);

    painter.drawImage(mOutputRect.topLeft(), *mImage, mImage->rect());

#if 0
    painter.setPen(Qt::red);
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            painter.drawPoint(i * 64, j * 48);
        }
    }
#endif

    painter.setTransform(old);

}


Matrix33 AdvancedImageWidget::currentTransformMatrix() const
{
    if (mOutputRect.height() == 0 || mOutputRect.width() == 0 ||
        mInputRect .height() == 0 || mInputRect .width() == 0)
    {
        return Matrix33::Identity();
    }

    Matrix33 shiftToZero = Matrix33::ShiftProj(-mInputRect.left(), -mInputRect.top());
    Matrix33 scaleTo2    = Matrix33::Scale2(2.0 / mInputRect.width(), 2.0 / mInputRect.height());

    Matrix33 shiftToMin1 = Matrix33::ShiftProj(-1.0,-1.0);

    Matrix33 inputToOne = shiftToMin1 * scaleTo2 * shiftToZero;

    Matrix33 rotation = Matrix33::Identity();
    switch (mUi->rotationComboBox->currentIndex()) {
        case 3: rotation = Matrix33::RotationZ90 (); break; /*  -90 */
        case 2: rotation = Matrix33::RotationZ180(); break; /* -180 */
        case 1: rotation = Matrix33::RotationZ270(); break; /* -270 */
        default:
            break;
    }

    Matrix33 shiftFromMin1 = Matrix33::ShiftProj(1.0,1.0);
    Matrix33 scaleFrom2 = Matrix33::Scale2(mOutputRect.width() / 2.0, mOutputRect.height() / 2.0);
    Matrix33 shiftFromZero = Matrix33::ShiftProj(mOutputRect.left(), mOutputRect.top());

    Matrix33 oneToOutput =  shiftFromZero * scaleFrom2 * shiftFromMin1;

    return oneToOutput * rotation * inputToOne;
}

QPointF AdvancedImageWidget::widgetToImageF(const QPointF &p)
{
#if 0
    if (mOutputRect.height() == 0 || mOutputRect.width() == 0 ||
        mInputRect .height() == 0 || mInputRect .width() == 0)
    {
        return p;
    }

    double x1 = (double)(p.x() - mOutputRect.left()) / (mOutputRect. width());
    double y1 = (double)(p.y() - mOutputRect. top()) / (mOutputRect.height());
    double x2 = x1;
    double y2 = y1;

    switch (mUi->rotationComboBox->currentIndex()) {
        case 3: x2 = 1.0 - y1; y2 = x1;       break; /*  -90 */
        case 2: x2 = 1.0 - x1; y2 = 1.0 - y1; break; /* -180 */
        case 1: x2 =       y1; y2 = 1.0 - x1; break; /* -270 */
        default:
            break;
    }

    double x = x2 * (mInputRect.width() ) + mInputRect.x();
    double y = y2 * (mInputRect.height()) + mInputRect.y();

    return QPointF(x,y);
#else
    Matrix33 m = currentTransformMatrix().inv();
    Vector2dd out = m * Qt2Core::Vector2ddFromQPointF(p);
    return Core2Qt::QPointFromVector2dd(out);
#endif

}

QPointF AdvancedImageWidget::imageToWidgetF(const QPointF &p)
{
#if 0
    if (mOutputRect.height() == 0 || mOutputRect.width() == 0 ||
        mInputRect .height() == 0 || mInputRect .width() == 0)
    {
        return p;
    }

    double x1 = (double)(p.x() - mInputRect.x()) / (mInputRect. width());
    double y1 = (double)(p.y() - mInputRect.y()) / (mInputRect.height());
    double x2 = x1;
    double y2 = y1;


    switch (mUi->rotationComboBox->currentIndex()) {
        case 1: x2 = 1.0 - y1; y2 = x1;       break; /*  90 */
        case 2: x2 = 1.0 - x1; y2 = 1.0 - y1; break; /* 180 */
        case 3: x2 =       y1; y2 = 1.0 - x1; break; /* 270 */
        default:
            break;
    }

    double resultX = x2 * (mOutputRect.width() ) + mOutputRect.x();
    double resultY = y2 * (mOutputRect.height()) + mOutputRect.y();

    return QPointF(resultX, resultY);
#else
    Matrix33 m = currentTransformMatrix();
    Vector2dd out = m * Qt2Core::Vector2ddFromQPointF(p);
    return Core2Qt::QPointFromVector2dd(out);
#endif
}

QPoint AdvancedImageWidget::widgetToImage(const QPoint &p)
{
    QPointF out = widgetToImageF(p);
    return QPoint(fround(out.x()), fround(out.y()));
}


Vector2dd AdvancedImageWidget::widgetToImageF(const Vector2dd &p)
{
    return Qt2Core::Vector2ddFromQPointF(AdvancedImageWidget::widgetToImageF(Core2Qt::QPointFromVector2dd(p)));
}

Vector2dd AdvancedImageWidget::imageToWidgetF(const Vector2dd &p)
{
    return Qt2Core::Vector2ddFromQPointF(AdvancedImageWidget::imageToWidgetF(Core2Qt::QPointFromVector2dd(p)));
}

QRect AdvancedImageWidget::getClientArea()
{
    return mUi->widget->geometry().translated(mUi->splitter->geometry().topLeft());
}

Vector2dd AdvancedImageWidget::getVisibleImageCenter()
{
    QPoint center = mInputRect.center();
    return Vector2dd(center.x(), center.y());
}

void AdvancedImageWidget::childRepaint(QPaintEvent* /*event*/, QWidget* childWidget)
{
    // SYNC_PRINT(("AdvancedImageWidget::childRepaint():called\n"));
    if (mImage == NULL)
        return;

    QPainter p(childWidget);
    repaintImage(p);
    repaintTools(p);
}

void AdvancedImageWidget::repaintImage(QPainter &p)
{
    if (mResizeCache != NULL)
    {
        p.drawImage(mOutputRect.topLeft(), *mResizeCache);
    }
    else
    {
        drawResized(p);
    }

    p.drawRect(mOutputRect.adjusted(-1,-1, 1, 1));
}

void AdvancedImageWidget::repaintTools(QPainter &p)
{
    if (mIsMouseLeftPressed && (mCurrentToolClass == ZOOM_SELECT_TOOL))
    {
        p.setPen(Qt::DashLine);
        p.drawRect(QRect(mSelectionStart, mSelectionEnd));
    }

    if (mIsMouseLeftPressed && (mCurrentToolClass == RECT_SELECTION_TOOLS))
    {
        QPen pen;
        pen.setColor(Qt::red);
        pen.setStyle(Qt::DashDotLine);
        p.setPen(pen);
        p.drawRect(QRect(mSelectionStart, mSelectionEnd));
    }

    if (mIsMouseLeftPressed && (mCurrentToolClass == LINE_SELECTION_TOOLS))
    {
        QPen pen;
        pen.setColor(Qt::red);
        pen.setStyle(Qt::DashDotLine);
        p.setPen(pen);
        p.drawLine(QLine(mSelectionStart, mSelectionEnd));
      //p.drawText(mSelectionEnd, QString::number(mDistance));
    }
}

void AdvancedImageWidget::freezeImage()
{
    mIsFreezed = !mIsFreezed;
    mUi->freezeButton->setIcon( mIsFreezed ? mContinueIcon : mFreezeIcon);
}

void AdvancedImageWidget::toolButtonReleased(QWidget * /*button*/)
{
    if (mUi->panButton->isChecked())
    {
        mCurrentToolClass = PAN_TOOL;
        mUi->widget->setCursor(Qt::OpenHandCursor);
        return;
    }

    if (mUi->selectZoomButton->isChecked())
    {
        mCurrentToolClass = ZOOM_SELECT_TOOL;
        mUi->widget->setCursor(Qt::ArrowCursor);
        return;
    }

    if (mUi->infoButton->isChecked()) {
        mCurrentToolClass = INFO_TOOL;
        mUi->widget->setCursor(Qt::CrossCursor);
        return;
    }

    mUi->widget->setMouseTracking(mCurrentToolClass == INFO_TOOL);

    for (int i = 0; i < (int)mSelectionButtons.size(); i++)
    {
        QToolButton *button = mSelectionButtons.at(i).button;
        if (!button->isChecked())
            continue;

        mUi->widget->setCursor(Qt::ArrowCursor);
        mCurrentSelectionButton = mSelectionButtons.at(i).id;
        mCurrentToolClass = RECT_SELECTION_TOOLS;
        return;
    }

    for (int i = 0; i < (int)mPointButtons.size(); i++)
    {
        QToolButton *button = mPointButtons.at(i).button;
        if (!button->isChecked())
          continue;

        mCurrentToolClass = POINT_SELECTION_TOOLS;
        mUi->widget->setCursor(Qt::CrossCursor);
        mCurrentPointButton = mPointButtons.at(i).id;
        return;
    }

    for (int i = 0; i < (int)mLineButtons.size(); i++)
    {
       QToolButton *button = mLineButtons.at(i).button;
       if (!button->isChecked())
           continue;

       mCurrentToolClass = LINE_SELECTION_TOOLS;
       mUi->widget->setCursor(Qt::CrossCursor);
       mCurrentLineButton = mLineButtons.at(i).id;
       return;
    }

    //printf("tools [%c%c%c%c]\n", isInSelectArea ? 'X' : 'O', isInPan ? 'X' : 'O', isInSelectZoom ? 'X' : 'O', isInInfo ? 'X' : 'O');
}

void AdvancedImageWidget::saveImageToFile()
{
    QString filename = QFileDialog::getSaveFileName(
            this,
            tr("Save Image"),
            "./Image.bmp",
            tr("Image Files (*.png *.jpg *.bmp)"));

    if (mImage == NULL)
    {
        return;
    }

    if (mResizeCache == NULL)
    {
        mResizeCache = new QImage(mOutputRect.size(), QImage::Format_RGB32);
        QPainter painter(mResizeCache);
        drawResized(painter);
        painter.end();
    }

    mResizeCache->save(filename);
}

void AdvancedImageWidget::startSaveFlow()
{
    mSaveDialog->hide();
    mSaveProcStarted = true;
    mUi->saveFlowButton->setText("Stop");
    mImageSavePath = mSaveDialog->getUi()->pathEdit->text() + "/" + mSaveDialog->getUi()->fileTemplateEdit->text();
    mUi->fileNamelabel->show();
    mUi->fileValuelabel->show();
}

void AdvancedImageWidget::stopSaveFlow()
{
    if( mSaveProcStarted )
    {
        mUi->saveFlowButton->setText("Save flow");
        mSaveProcStarted = false;
        mImageSavePath = "";
        mImageNumber = 0;
        mUi->fileNamelabel->hide();
        mUi->fileValuelabel->hide();
    }
    else
    {
        mSaveDialog->show();
    }
}


void AdvancedImageWidget::childWheelEvent(QWheelEvent  *event)
{
   // qDebug("AdvancedImageWidget::childWheelEvent() delta=%d\n", event->delta());
    QPointF scrollWidgetPoint = event->pos();
    QPointF scrollImagePoint = widgetToImageF(scrollWidgetPoint);

    if (event->delta() > 0)
    {
        for (int i = 0; i < event->delta() / 60; i++) {
            zoomIn();
        }
    }

    if (event->delta() < 0)
    {
        for (int i = 0; i < -event->delta() / 60; i++) {
            zoomOut();
        }
    }

    QPointF newScrollImagePoint = widgetToImageF(scrollWidgetPoint);

    QPointF shift = newScrollImagePoint - scrollImagePoint;
    mZoomCenter -= QPoint(fround(shift.x()), fround(shift.y()));
    recomputeRects();
    emit notifyCenterPointChanged(mZoomCenter);
    forceUpdate();
}


void AdvancedImageWidget::childMousePressed(QMouseEvent * event)
{
/*    if (!mIsMousePressed && mCurrentToolClass == LINE_SELECTION_TOOLS){
        mLineSelectionMode = true;
    }*/

    mIsMouseLeftPressed  = (event->buttons() & Qt::LeftButton ) != 0;
    mIsMouseRightPressed = (event->buttons() & Qt::RightButton) != 0;

    mSelectionStart = event->pos();
    mSelectionEnd   = event->pos();

    if (mCurrentToolClass == PAN_TOOL ||
       (mRightMouseButtonDrag &&  mIsMouseRightPressed))
    {
        mUi->widget->setCursor(Qt::ClosedHandCursor);
    }

    forceUpdate();
}

void AdvancedImageWidget::childMouseReleased(QMouseEvent * event)
{
    bool leftReleased  = mIsMouseLeftPressed  && !(event->buttons() & Qt::LeftButton);
    bool rightReleased = mIsMouseRightPressed && !(event->buttons() & Qt::RightButton);

    // qDebug() << "AdvancedImageWidget::childMouseReleased: leftReleased " << leftReleased;
    // qDebug() << "AdvancedImageWidget::childMouseReleased: rightReleased " << rightReleased;


    mIsMouseLeftPressed = (event->buttons() & Qt::LeftButton) != 0;
    mIsMouseRightPressed = (event->buttons() & Qt::RightButton) != 0;

    mSelectionEnd = event->pos();


    if (rightReleased && mRightMouseButtonDrag) {
       mUi->widget->setCursor(Qt::ArrowCursor);
    }

    if (leftReleased)
    {
        if (mCurrentToolClass == PAN_TOOL)
        {
            mUi->widget->setCursor(Qt::OpenHandCursor);
        }

        /*All other tools depend on image*/
        if (mImage.isNull())
        {
            return;
        }

        if (mCurrentToolClass == ZOOM_SELECT_TOOL)
        {
            QRect selection(widgetToImage(mSelectionStart), widgetToImage(mSelectionEnd));
            mZoomCenter = selection.center();
            emit notifyCenterPointChanged(mZoomCenter);
            QSize outSize = selection.normalized().size();
            if (outSize.isNull())
            {
                outSize = QSize(1,1);
            }
            QSize inSize = mImage->size();
            if (inSize.isNull())
            {
                inSize = QSize(1,1);
            }

            double verticalAspect   = (double)outSize.height() / inSize.height();
            double horizontalAspect = (double)outSize.width()  / inSize.width();
            double aspect = std::min(verticalAspect, horizontalAspect);
            aspect = std::min(aspect, 1.0);
            aspect = 1.0 / aspect;
            aspect = std::max(aspect, (double)mUi->expSpinBox->minimum());
            aspect = std::min(aspect, (double)mUi->expSpinBox->maximum());

            mUi->expSpinBox->setValue((int)aspect);
        }

        if (mCurrentToolClass == RECT_SELECTION_TOOLS)
        {
            QRect selection(widgetToImage(mSelectionStart), widgetToImage(mSelectionEnd));

            qDebug("AdvancedImageWidget::mouseReleased: Emitting newAreaSelected(%d, _)", mCurrentSelectionButton);

            emit newAreaSelected(mCurrentSelectionButton, selection);
        }

        if (mCurrentToolClass == LINE_SELECTION_TOOLS)
        {
            QLine line(widgetToImage(mSelectionStart), widgetToImage(mSelectionEnd));

            emit newLineSelected(mCurrentLineButton, line);
        }

        if (mCurrentToolClass == POINT_SELECTION_TOOLS)
        {
            emit newPointSelected(mCurrentPointButton, widgetToImage(mSelectionEnd));
        }
    }

    forceUpdate();
}

void AdvancedImageWidget::childMouseMoved(QMouseEvent * event)
{
    if (mImage.isNull())
        return;

    QPoint imagePoint = widgetToImage(event->pos());

    if ((mCurrentToolClass == INFO_TOOL) && mImage->valid(imagePoint))
    {
        QColor color = mImage->pixel(imagePoint);
        RGBColor c = RGBColor(color.red(), color.green(), color.blue());

        QString info = QString("[%1 x %2] of [%3 x %4] (R:%5 G:%6 B:%7) (H:%8 S:%9 V:%10)" )
                .arg(imagePoint.x())
                .arg(imagePoint.y())
                .arg(mImage->width())
                .arg(mImage->height())
                .arg(color.red())
                .arg(color.green())
                .arg(color.blue())
                .arg(c.hue())
                .arg(c.saturation())
                .arg(c.brightness());
        setInfoValueLabel(info);
    }

    /* Pan */
    bool isInPan = false;
    if (mCurrentToolClass == PAN_TOOL && mIsMouseLeftPressed)
        isInPan = true;
    if (mRightMouseButtonDrag && mIsMouseRightPressed)
        isInPan = true;

    if (isInPan)
    {
        QPoint shift = (widgetToImage(mSelectionEnd) - imagePoint);
        mSelectionEnd = event->pos();
        mZoomCenter += shift;
        recomputeRects();
        emit notifyCenterPointChanged(mZoomCenter);

        /* Draw tooling instruments */
        forceUpdate();
        return;
    }

    mSelectionEnd = event->pos();

    /*if (mCurrentToolClass == LINE_SELECTION_TOOLS)
    {
        QLine line(widgetToImage(mSelectionStart), widgetToImage(mSelectionEnd));

        emit newLineSelected(mCurrentSelectionButton, line);
    }*/

    if (mIsMouseLeftPressed)
    {
        if (mCurrentToolClass == POINT_SELECTION_TOOLS)
        {
            emit pointToolMoved(mCurrentPointButton, widgetToImage(mSelectionEnd));
        }

        /* Draw tooling instruments */
        forceUpdate();
        return;
    }


}

void AdvancedImageWidget::zoomReset()
{
    if (mImage == NULL) {
        mZoomCenter = QPoint(-1,-1);
    }
    else
    {
        mZoomCenter = QPoint(mImage->size().width(), mImage->size().height()) / 2.0;
    }
    mUi->expSpinBox->setValue(1.0);
    recomputeRects();
    emit notifyCenterPointChanged(mZoomCenter);
    forceUpdate();
}

void AdvancedImageWidget::zoomChanged()
{
    mUi->zoomInButton ->setEnabled(mUi->expSpinBox->value() != mUi->expSpinBox->maximum());
    mUi->zoomOutButton->setEnabled(mUi->expSpinBox->value() != mUi->expSpinBox->minimum());
    recomputeRects();
    emit notifyZoomChanged(mUi->expSpinBox->value());
    forceUpdate();
}

void AdvancedImageWidget::changeZoom(double zoom)
{
    mUi->expSpinBox->setValue(zoom);
    zoomChanged();
}

void AdvancedImageWidget::changeCenterPoint(QPoint point)
{
    mZoomCenter = point;
    recomputeRects();
    emit notifyCenterPointChanged(mZoomCenter);
    forceUpdate();
}

void AdvancedImageWidget::showEvent(QShowEvent *event)
{
    ViAreaWidget::showEvent(event);

    qDebug("AdvancedImageWidget::showEvent():called");
    QTimer::singleShot(2000, this, SLOT(showScheduledHint()));
}

void AdvancedImageWidget::hideEvent(QHideEvent *event)
{
    ViAreaWidget::hideEvent(event);

    qDebug("AdvancedImageWidget::hideEvent():called");

}

void AdvancedImageWidget::showScheduledHint()
{
    QList<int> sizes = mUi->splitter->sizes();
    bool showHint = (!sizes.isEmpty() && sizes[0] == 0);

    if (isVisible() && showHint)  {
        QLabel *label = new QLabel(this);
        label->setText("Drag handle for more");
        Qt::WindowFlags flags = label->windowFlags();
        flags &= ~Qt::WindowTitleHint;
        label->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        label->setWindowModality(Qt::NonModal);
        qDebug() << printWindowFlags(label->windowFlags());

        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        QPalette palette = label->palette();
        palette.setColor(label->backgroundRole(), Qt::black);
        palette.setColor(label->foregroundRole(), Qt::white);
        label->setPalette(palette);

        QPoint position = mUi->splitter->mapToGlobal((mUi->splitter->geometry().topLeft() + mUi->splitter->geometry().topRight())/ 2);
        //QPoint position = mapToGlobal(QPoint(0,0));
        label->setGeometry(position.x(), position.y(), 200,40);
        label->show();
        qDebug() << "childRepaint:" << position;
        qDebug() << geometry();

        connect(this, SIGNAL(destroyed(QObject*)), label, SLOT(deleteLater()));

        QTimer::singleShot(5000, label, SLOT(deleteLater()));
    }

}

bool AdvancedImageWidget::isRotationLandscape()
{
    int rotation = mUi->rotationComboBox->currentIndex();
    return (rotation == 0) || (rotation == 2);
}


void AdvancedImageWidget::zoomIn()
{    
    mUi->expSpinBox->stepUp();
}

void AdvancedImageWidget::zoomOut()
{
    mUi->expSpinBox->stepDown();
}

void AdvancedImageWidget::fitToggled()
{
    recomputeRects();
    forceUpdate();
}

void AdvancedImageWidget::setFitWindow(bool flag)
{
    mUi->fitToWindowCheckBox->setChecked(flag);
}

void AdvancedImageWidget::setKeepAspect(bool flag)
{
    if (mUi->fitToWindowCheckBox->isChecked())
    {
        mUi->aspectCheckBox->setChecked(flag);
    }
}

void AdvancedImageWidget::setRightDrag(bool flag)
{
    mRightMouseButtonDrag = flag;
}

void AdvancedImageWidget::setCompactStyle(bool flag)
{
    mUi->zoomInButton ->setHidden(flag);
    mUi->zoomOutButton->setHidden(flag);
    mUi->expSpinBox->setHidden(flag);
}

void AdvancedImageWidget::forceUpdate()
{
    emit preUpdate();

    mUi->widget->show();
    emit mUi->widget->update();
}

void AdvancedImageWidget::childResized (QResizeEvent * /*event*/)
{
   fitToggled();
   // printf("Child resize called to %d %d\n", event->size().width(), event->size().height());
}

QRect AdvancedImageWidget::computeInputRect()
{
    if (mImage == NULL)
    {
        return QRect();
    }

    double zoomFactor = mUi->expSpinBox->value();
    QSize size = mImage->rect().normalized().size() / zoomFactor;
    if (size.isNull())
    {
        size = QSize(1,1);
    }

    QPoint topLeft = mZoomCenter - QPoint(size.width(), size.height()) / 2.0;
    return QRect (topLeft, size);
}

void AdvancedImageWidget::recomputeRects()
{
    if (mImage == NULL)
    {
       return;
    }

    QRect output (mImage->rect());
    if (!isRotationLandscape()) {
        output.setWidth (mImage->height());
        output.setHeight(mImage->width ());
    }

    output = output.normalized();
    QRect input = computeInputRect();

    if (mUi->fitToWindowCheckBox->isChecked())
    {
        output = mUi->widget->rect().normalized();

        if (mUi->aspectCheckBox->isChecked())
        {
            double inputAspect = 1.0;
            if (isRotationLandscape()) {
                inputAspect = (double)input.height() / input.width();
            } else {
                inputAspect = (double)input.width() / input.height();
            }

            int outH = output.width() * inputAspect;
            if (outH > output.height())
                output.setWidth(output.height() / inputAspect);
            else
                output.setHeight(output.width() * inputAspect);
        }
    }

    if (!output.isValid())
        output = QRect(0,0,1,1);

    mOutputRect = output;
    mInputRect  = input;
    delete_safe(mResizeCache);
}

void AdvancedImageWidget::saveFlowImage(QImage * image)
{
    if (mSaveProcStarted && image != NULL)
    {
        unsigned int pathLength = mImageSavePath.size() + 10;
        char *currentPath = new char[pathLength];
        snprintf(currentPath, pathLength, mImageSavePath.toStdString().c_str(), mImageNumber);
        image->save(currentPath);
        mImageNumber++;
        mUi->fileValuelabel->setText(currentPath);
        delete[] currentPath;
    }
}

void AdvancedImageWidget::recalculateZoomCenter()
{
    if (mImage.isNull())
    {
        mZoomCenter = QPoint(0,0);
    }
    else
    {
        mZoomCenter = QPoint(mImage->size().width(), mImage->size().height()) / 2.0;
    }
    emit notifyCenterPointChanged(mZoomCenter);
}

QWidget* AdvancedImageWidget::getWidget()
{
    return mUi->widget;
}

QFrame* AdvancedImageWidget::getToolsFrame()
{
    return mUi->frame;
}

QToolButton * AdvancedImageWidget::addToolButton(QString name, QIcon icon, bool checkable)
{
    QFrame* holder = getToolsFrame();
    QToolButton *toolButton = new QToolButton();
    toolButton->setIcon(icon);
    toolButton->setText(name);
    toolButton->setToolTip(name);
    toolButton->setIconSize(QSize(24,24));
    if (checkable)
    {
        toolButton->setAutoExclusive(true);
        toolButton->setCheckable(true);
        toolButton->setAutoRaise(true);
    }
    holder->layout()->addWidget(toolButton);

    connect(toolButton, SIGNAL(released()),&mToolMapper, SLOT(map()));
    mToolMapper.setMapping(toolButton, toolButton);
    //connect(toolButton, SIGNAL(released()), this, SLOT(toolButtonReleased()));
    return toolButton;
}

bool AdvancedImageWidget::addSelectionTool(int toolID, QString name, QIcon icon)
{
//    qDebug("AdvancedImageWidget::addSelectionTool(%d, _ , _)", toolID);

    QToolButton *toolButton = addToolButton(name, icon);
    WidgetToolButton toolRef(toolID, toolButton);
    mSelectionButtons.push_back(toolRef);
    return true;
}

bool AdvancedImageWidget::addLineTool(int toolID, QString name, QIcon icon)
{
//    qDebug("AdvancedImageWidget::addLineTool(%d, _ , _)", toolID);

    QToolButton *toolButton = addToolButton(name, icon);
    WidgetToolButton toolRef(toolID, toolButton);
    mLineButtons.push_back(toolRef);
    return true;
}

bool AdvancedImageWidget::addPointTool(int toolID, QString name, QIcon icon)
{
//    qDebug("AdvancedImageWidget::addPointTool(%d, _ , _)", toolID);

    QToolButton *toolButton = addToolButton(name, icon);
    WidgetToolButton toolRef(toolID, toolButton);
    mPointButtons.push_back(toolRef);
    return true;
}

void AdvancedImageWidget::setInfoValueLabel(QString &infoString)
{
    mUi->infoValueLabel->setText(infoString);
}

void AdvancedImageWidget::loadFromQSettings(const QString &fileName, const QString &_root)
{
    QSettings loader(fileName, QSettings::IniFormat);
    loader.beginGroup(_root + mRootPath);
    mZoomCenter.setX(loader.value("center.x", 0).toInt());
    mZoomCenter.setY(loader.value("center.y", 0).toInt());

    mUi->fitToWindowCheckBox->setChecked(loader.value("fitToWindow", false).toBool());
    mUi->aspectCheckBox     ->setChecked(loader.value("fixAspect", false).toBool());
    mUi->expSpinBox         ->setValue  (loader.value("zoom", 1.0).toDouble());

    mUi->rotationComboBox   ->setCurrentIndex(loader.value("rotation", 0).toInt());
}

void AdvancedImageWidget::saveToQSettings(const QString &fileName, const QString &_root)
{
    QSettings saver(fileName, QSettings::IniFormat);
    saver.beginGroup(_root + mRootPath);
    saver.setValue("fitToWindow", mUi->fitToWindowCheckBox->isChecked());
    saver.setValue("fixAspect"  , mUi->aspectCheckBox->isChecked());
    saver.setValue("zoom", mUi->expSpinBox->value());
    saver.setValue("center.x", mZoomCenter.x());
    saver.setValue("center.y", mZoomCenter.y());

    saver.setValue("rotation", mUi->rotationComboBox->currentIndex());
}
