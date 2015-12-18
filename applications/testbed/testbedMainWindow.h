#ifndef TESTBEDMAINWINDOW_H
#define TESTBEDMAINWINDOW_H

#include <deque>
#include <QMainWindow>

#include "ui_testbedMainWindow.h"
#include "advancedImageWidget.h"
#include "rgb24Buffer.h"
#include "cloudViewDialog.h"
#include "localHistogram.h"
#include "widgets/testbedImageWidget.h"
#include "graphPlotDialog.h"

using corecvs::RGB24Buffer;
using std::deque;

class TestbedMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    TestbedMainWindow(QWidget *parent = 0);
    ~TestbedMainWindow();

    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent * event);

private:
    void connectActions();
    Ui::TestbedMainWindowClass *mUi;

    TestbedImageWidget *mImageWidget;
    CloudViewDialog *m3DHist;
    GraphPlotDialog *mGraphPlot;

/* Move this out */

public slots:
    void loadImage();

    /**/
    G8Buffer *projectToDirection(RGB24Buffer *input, const Vector3dd &direction);
    void preprocessImage();


    void pointSelected(int toolID, QPoint point);
    void pointSelectedMoved(int toolID, QPoint point);

    void rectSelected (int toolID, QRect rect);

    /* Testing only */
    void paramChanged();

    void prepareBlendedMask();
    void updateViewImage(void);
    void toggleMask(void);
    void resetMask(void);
    void undoMask(void);
    void doUpdateLocalHist(QPoint point);
    void doUpdateLocalHist2D(QPoint point);

    void doMask(QPoint point, int id);
    void doFill();
    void doErease(QPoint point);
    void doFillGapsMorpho();
    void doFillGapsMorphoFast();
    void doFillGapsSegment();

    void maskHue(int hue1, int hue2);
    void maskTolerance(QPoint point);
    void maskTolerance1(QPoint point);
    void maskToleranceFinal(QPoint point);
    void maskToleranceGraph(QPoint point);
    void maskLocalHist(QPoint point);
    void maskLocalHist2D(QPoint point);
    void recursiveTolerance(RGBColor startColor, int tolerance, int x, int y);

    /*UI related*/
    void radiusChanged(void);

public:
    RGB24Buffer *mImage;
    G8Buffer *mMask;
    G8Buffer *mMaskBlended;
    G12Buffer *mMaskStore;

    G8Buffer *mHComp;
    G8Buffer *mSComp;
    G8Buffer *mVComp;
    G8Buffer *mPrincipal;
    G8Buffer *mPrincipal2;
    G8Buffer *mPrincipal3;
    HistogramBuffer   *mHistBuffer;
    Histogram2DBuffer *mHist2DBuffer;

    AdvancedImageWidget *mLocalHistWidget;
    AdvancedImageWidget *mLocalHist2DWidget;

    G12Buffer *mCannyEdges;
    G12Buffer *mEdges;

    bool mMaskChange;
    deque<G8Buffer *> mUndoList;
};

#endif // TESTBEDMAINWINDOW_H
