#ifndef IMAGEVIEWMAINWINDOW_H
#define IMAGEVIEWMAINWINDOW_H

#include "core/buffers/rgb24/rgbTBuffer.h"
#include "core/buffers/g12Buffer.h"
#include "core/buffers/converters/debayer.h"

#include <QWidget>

namespace Ui {
class ImageViewMainWindow;
}

using namespace corecvs;

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
