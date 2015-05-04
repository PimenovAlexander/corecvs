#pragma once

#include <QWidget>

namespace Ui {
class Widget;
}

class FoldableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FoldableWidget(QWidget *parent = 0, QString const &name = "", QWidget *centralWidget = 0
            , bool startFolded = true, bool isAdvanced = false);

    ~FoldableWidget();

    void setMainWidget(QWidget *widget);
    bool isFolded() { return mIsFolded; }

public slots:
    void changeFolding();

private:
    Ui::Widget *ui;
    bool mIsFolded;

    QString mName;

    QSize mCurrentSize;
    QSize mPrevSize;
};
