#ifndef MIXERCHANNELOPERATION_H
#define MIXERCHANNELOPERATION_H

#include <QWidget>

namespace Ui {
class MixerChannelOperation;
}

class MixerChannelOperation : public QWidget
{
    Q_OBJECT

public:
    explicit MixerChannelOperation(QWidget *parent = 0);
    ~MixerChannelOperation();

private:
    Ui::MixerChannelOperation *ui;
};

#endif // MIXERCHANNELOPERATION_H
