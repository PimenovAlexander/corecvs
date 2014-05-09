/**
 * \file g12Image.h
 *
 * \date Jul 22, 2009
 * \author: Alexander Pimenov
 */

#ifndef G12IMAGE_H_
#define G12IMAGE_H_

#include <QtGui/QWidget>

#include "g12Buffer.h"
#include "rgb24Buffer.h"

#include "viAreaWidget.h"

using namespace corecvs;


class G12Image : public QImage {
public:
    G12Image(G12Buffer *buffer);
    G12Image(G12Buffer *buffer, int newH, int newW);
    G12Image(G12Buffer *buffer, bool mirror);


    virtual ~G12Image();
};

class G8Image : public QImage {
public:
    G8Image(G8Buffer *buffer);
    /*G8Image(G12Buffer *buffer, int newH, int newW);
    G8Image(G12Buffer *buffer, bool mirror);*/

    virtual ~G8Image();
};

class RGB24Image : public QImage{
public:
    RGB24Image(RGB24Buffer *buffer, bool mirror = false);
    virtual ~RGB24Image();
};

QImage *toQImage(G12Buffer *buffer);
QImage *toQImage(G8Buffer *buffer);
QImage *toQImage(RGB24Buffer *buffer);

class ImageWidget : public ViAreaWidget {
public:
    QImage *image;

    ImageWidget() : image(NULL) {};
    void setImage(QImage *_image);

    virtual void paintEvent(QPaintEvent * );

    ~ImageWidget(){};
};



class RGB24InterfaceImage : public QImage{
public:
    RGB24InterfaceImage(RGB24Buffer *buffer);
    virtual ~RGB24InterfaceImage();
};



#endif /* G12IMAGE_H_ */
