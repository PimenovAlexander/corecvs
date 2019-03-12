#ifndef MIXERCHANNELOPERATION_H
#define MIXERCHANNELOPERATION_H

#include <QWidget>

namespace Ui {
class MixerChannelOperationWidget;
}

class MixerChannelOperationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MixerChannelOperationWidget(QWidget *parent = 0);
    ~MixerChannelOperationWidget();

public slots:
    void setInput (int input );
    void setOutput(int output);


private:
    Ui::MixerChannelOperationWidget *ui;
};

#endif // MIXERCHANNELOPERATION_H
