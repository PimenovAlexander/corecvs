#ifndef TESTSUPERRESOLUTIONMAINWINDOW_H
#define TESTSUPERRESOLUTIONMAINWINDOW_H

#include <deque>
#include <QtGui/QMainWindow>
#include "ui_testSuperResolutionMainWindow.h"
#include "advancedImageWidget.h"
#include "rgb24Buffer.h"
#include "listsOfLRImages.h"
#include <QListWidget>
using corecvs::RGB24Buffer;
using std::deque;

class TestSuperResolutionMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    TestSuperResolutionMainWindow(QWidget *parent = 0);
    ~TestSuperResolutionMainWindow();

    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent * event);

private:
    void connectActions();
    Ui::TestSuperResolutionMainWindowClass *mUi;

    AdvancedImageWidget *mImageWidget;

    std::deque<RGB24Buffer*> mImageCollection;
    std::deque<LRImage> mListOfLRImages;

    bool canDelete;
/* Move this out */

public slots:
    void loadImage();
    void pointSelected(int toolID, QPoint point);

    void updateViewImage(void);
    void toggleMask(void);
    void resetMask(void);
    void undoMask(void);

    void maskHue(int hue1, int hue2);
    void maskTolerance(QPoint point);
    void maskTolerance1(QPoint point);
    void recursiveTolerance(RGBColor startColor, int tolerance, int x, int y);

public:
    RGB24Buffer *mImage;
    G8Buffer *mMask;
    bool mMaskChange;
    deque<G8Buffer *> mUndoList;
private slots:
    void ClearCollection();
    void cutImage();
    void addImageFromTheScreenToCollection();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void addElementToCollection();
    void resampleUsingBilinearInterpolation();
    void resampleUsingLancsozFilter2x2();
    void resampleUsingLancsozFilter4x4();
    void resampleUsingLancsozFilter6x6();
    void resampleUsingLancsozFilter8x8();
    void resampleUsingNearestNeighbour();
    void resampleUsingSquares();
    void convolutionImage();
    void simpleMethodModelingProcess();
    void rotateByAngle();
};

#endif // TESTSUPERRESOLUTIONMAINWINDOW_H
