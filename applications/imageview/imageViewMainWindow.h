#ifndef IMAGEVIEWMAINWINDOW_H
#define IMAGEVIEWMAINWINDOW_H

#include "rgbTBuffer.h"
#include "g12Buffer.h"
#include "converters/debayer.h"

#include <QWidget>

namespace Ui {
class ImageViewMainWindow;
}

class ImageViewMainWindow : public QWidget
{
    Q_OBJECT

public:
    MetaData meta;
    G12Buffer *bayer;

    RGB48Buffer *input;
    explicit ImageViewMainWindow(QWidget *parent = 0);
    ~ImageViewMainWindow();

public slots:

    void setImage(RGB48Buffer *image);

    void paramsChanged(void);
    void loadImageAction();
    void loadImage(QString name);

    void debayer();
private:
    Ui::ImageViewMainWindow *ui;
};

#endif // IMAGEVIEWMAINWINDOW_H
