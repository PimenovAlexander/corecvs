#ifndef TESTBEDMAINWINDOW_H
#define TESTBEDMAINWINDOW_H

#include <deque>
#include <QtGui/QMainWindow>
#include "ui_testbedMainWindow.h"
#include "advancedImageWidget.h"
#include "rgb24Buffer.h"
#include <QListWidget>
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

    AdvancedImageWidget *mImageWidget;
    QListWidget mListWidget;
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

    void addImageToCollection();
    void ClearCollection();
public:
    RGB24Buffer *mImage;
    G8Buffer *mMask;
    bool mMaskChange;
    deque<G8Buffer *> mUndoList;
private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void addElementToCollection();
};

#endif // TESTBEDMAINWINDOW_H
