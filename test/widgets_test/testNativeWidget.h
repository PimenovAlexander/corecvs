#ifndef TESTNATIVEWIDGET_H
#define TESTNATIVEWIDGET_H

#include <QWidget>

namespace Ui {
class TestNativeWidget;
}

class TestNativeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TestNativeWidget(QWidget *parent = 0);
    ~TestNativeWidget();

public slots:
    void testButtonPushed( void );

private:
    Ui::TestNativeWidget *ui;
};

#endif // TESTNATIVEWIDGET_H
