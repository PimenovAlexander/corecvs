#ifndef IMAGEFORCALIBRATIONWIDGET_H
#define IMAGEFORCALIBRATIONWIDGET_H

#include <QGraphicsScene>
#include <QWidget>
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
    int getId();
    void setId(int i);
    void setImage(cv::Mat *);
private:
    Ui::ImageForCalibrationWidget *ui;
    bool imageSet = false;
    bool imageApproved = false;
    cv::Mat image;
    QImage qimage;
    int id = -1;
    QGraphicsScene *sc;
 private slots:
    void showImage();
    void closeWid();
    void approve();
signals:
     void approved();
     void closed(int k);


};

#endif // IMAGEFORCALIBRATIONWIDGET_H
