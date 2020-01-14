#ifndef IMAGEFORCALIBRATIONWIDGET_H
#define IMAGEFORCALIBRATIONWIDGET_H

#include <QGraphicsScene>
#include <QWidget>
#include <buffers/rgb24/rgb24Buffer.h>
#include <opencv2/core.hpp>

namespace Ui {
class ImageForCalibrationWidget;
}

class ImageForCalibrationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageForCalibrationWidget(QWidget *parent = nullptr);
    ~ImageForCalibrationWidget();


    void setImage(corecvs::RGB24Buffer *);
    void lockButtons();
    void updateThumbnail();
    corecvs::RGB24Buffer * getImage();

    /* Two copies of the image */
    QImage *qImage = NULL;
    corecvs::RGB24Buffer *rgb24Image = NULL;

    void setDefaultImage();
    void unlockButtons();
signals:
     void closed(int k);

private:
     Ui::ImageForCalibrationWidget *ui;

};

#endif // IMAGEFORCALIBRATIONWIDGET_H
