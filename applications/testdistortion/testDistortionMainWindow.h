#ifndef TESTBEDMAINWINDOW_H
#define TESTBEDMAINWINDOW_H

#include <deque>
#include <QtGui/QMainWindow>


#include "distortioncorrector/distortionWidget.h"

#include "ui_testbedMainWindow.h"
#include "advancedImageWidget.h"
#include "houghTransformViewer.h"
#include "rgb24Buffer.h"
#include "houghSpace.h"

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
    void updateImages();
    G12Buffer* executeEdgeFilter(RGB24Buffer *image);
    G12Buffer* executeHoughTransform(G12Buffer* image);
    Ui::TestbedMainWindowClass *mUi;
    bool isEdgeFilterActive;
    RGB24Buffer *originalImage;
    G12Buffer *edgeFilterImage;
    HoughSpace *houghSpace;

    AdvancedImageWidget *mImageWidget;
    HoughTransformViewer *showerWidget;
    DistortionWidget *mDistrtionWidget;
/* Move this out */

public slots:
    void loadImage();
    void pointSelected(int toolID, QPoint point);

    void updateViewImage(void);
    void toggleMask(void);
    void resetMask(void);
    void undoMask(void);
    void openDistortionWindow(void);
    void openHoughTransformWindow(void);
    void toogleEdgeFilter(void);
    void selectedPointInHoughSpace(QPoint point);

    void maskHue(int hue1, int hue2);
    void maskTolerance(QPoint point);
    void maskTolerance1(QPoint point);
    void recursiveTolerance(RGBColor startColor, int tolerance, int x, int y);


public:
    RGB24Buffer *mImage;
    G8Buffer *mMask;
    bool mMaskChange;
    deque<G8Buffer *> mUndoList;
};

#endif // TESTBEDMAINWINDOW_H
