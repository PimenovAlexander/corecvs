#ifndef ADVANCEDIMAGEWIDGET_H_
#define ADVANCEDIMAGEWIDGET_H_

/**
 * \file advancedImageWidget.h
 *
 * \date Feb 27, 2012
 * \author: Olga Sapershteyn
 */
#include <vector>

#include <QWidget>
#include <QtGui>

#include "vector2d.h"
#include "viAreaWidget.h"
#include "saveFlowSettings.h"
#include "parametersControlWidgetBase.h"

#include "ui_advancedImageWidget.h"

using std::vector;

class AdvancedImageWidget : public ViAreaWidget, public SaveableWidget
{
    Q_OBJECT

public:
    QSharedPointer<QImage> mImage;

    AdvancedImageWidget(QWidget *parent = 0, bool showHeader = true);
   ~AdvancedImageWidget();

    void            setImage(QSharedPointer<QImage> _image);

    QWidget*        getWidget();
    QFrame*         getToolsFrame();
    QRect           getInputRect() const                { return mInputRect; }

    void            setInfoValueLabel(QString &string);

    void            setCollapseTitle(bool collapse);

public slots:
    virtual void    childRepaint(QPaintEvent *event, QWidget *who);
    void            freezeImage();

   /**
    *   This could be reimplemented to add additional tools
    **/
    virtual void    toolButtonReleased(QWidget *button);

    void            saveImageToFile();
    void            startSaveFlow();
    void            stopSaveFlow();

    virtual void    childMouseMoved   (QMouseEvent  *);
    virtual void    childMousePressed (QMouseEvent  *);
    virtual void    childMouseReleased(QMouseEvent  *);
    virtual void    childWheelEvent   (QWheelEvent  *);
    virtual void    childResized      (QResizeEvent *);

    void            zoomIn();
    void            zoomOut();
    void            zoomReset();
    void            zoomChanged();

    void            fitToggled();
    void            setFitWindow(bool flag = true);

    void            setInfoString(QString info)         { mUi->infoValueLabel->setText(info); }

    /* Additional tools support*/
    bool            addSelectionTool (int toolID, QString name, QIcon icon);
    bool            addLineTool      (int toolID, QString name, QIcon icon);
    bool            addPointTool     (int toolID, QString name, QIcon icon);

    void            changeZoom(double zoom);
    void            changeCenterPoint(QPoint point);

signals:
    void            newAreaSelected (int toolID, QRect  area);
    void            newPointSelected(int toolID, QPoint point);
    void            newLineSelected (int toolID, QLine  line);

    void            pointToolMoved(int toolID, QPoint point);

    void            notifyZoomChanged(double zoom);
    void            notifyCenterPointChanged(QPoint point);

protected:

    bool            isRotationLandscape();
    void            drawResized (QPainter &painter);

    /* Freeze related variables */
    bool                    mIsFreezed;
    QIcon                   mFreezeIcon;
    QIcon                   mContinueIcon;

    bool                    mSaveProcStarted;
    unsigned int            mImageNumber;
    QString                 mImageSavePath;
    SaveFlowSettings*       mSaveDialog;
    Ui_advancedImageWidget *mUi;

    QPoint                  mZoomCenter;

    /**
     * Two input geometry controls
     * Trasformation is done it three steps
     *   1. Input is cropped to mInputRect
     *   2. Rotated according to ui->rotation
     *   3. Output is scaled to mOutputRect
     **/
    QRect                   mOutputRect;
    QRect                   mInputRect;

    bool                    mIsMousePressed;
    QPoint                  mSelectionStart;
    QPoint                  mSelectionEnd;

    /**
     * Image is transformed only once. If transformation is done, result is saved to
     * this variable.
     **/
    QImage                 *mResizeCache;

    /**
     *  Exandable toolbar has several predefined  instuments, and th ability to add addintional tools
     *  tools added by user can be used to select point, line or area.
     *
     *  For more advanced tools you will need to derive from this class
     *
     **/
    enum ToolClass {
        NO_TOOL,
        ZOOM_SELECT_TOOL,
        PAN_TOOL,
        INFO_TOOL,

        RECT_SELECTION_TOOLS,
        LINE_SELECTION_TOOLS,
        POINT_SELECTION_TOOLS,

        TOOL_CLASS_LAST
    };

    ToolClass               mCurrentToolClass;

    /** Tools in the toolbar bars */
    struct WidgetToolButton {
        int          id;
        QToolButton *button;

        WidgetToolButton(int _id, QToolButton *_button) : id(_id), button(_button) {}
    };

    vector<WidgetToolButton> mSelectionButtons;
    int                      mCurrentSelectionButton;
    vector<WidgetToolButton> mPointButtons;
    int                      mCurrentPointButton;
    vector<WidgetToolButton> mLineButtons;
    int                      mCurrentLineButton;

    QSignalMapper            mToolMapper;

    QToolButton             *addToolButton(QString name, QIcon icon, bool checkable = true);

    QPointF     widgetToImageF(const QPointF &p);
    QPointF     imageToWidgetF(const QPointF &p);
    QPoint      widgetToImage(const QPoint &p);

    Vector2dd   widgetToImageF(const Vector2dd &p);
    Vector2dd   imageToWidgetF(const Vector2dd &p);

    QRect       computeInputRect();

    QSize       mImageSize;

    void        recomputeRects();
    void        saveFlowImage(QImage * image);
    void        recalculateZoomCenter();

public:
    /* Saving loading parameters to/from widget */
    /* TODO: Use visitors here*/
    QString      mRootPath;

    virtual void loadFromQSettings(const QString &fileName, const QString &_root);
    virtual void saveToQSettings  (const QString &fileName, const QString &_root);

    void         setSavingRoot(const QString &root) { mRootPath = root; }
};

#endif /* ADVANCEDIMAGEWIDGET_H */
