/**
 * \file g12Image.h
 *
 * \date Jul 22, 2009
 * \author: Alexander Pimenov
 */
#ifndef G12IMAGE_H_
#define G12IMAGE_H_

#include <QWidget>

#include "core/buffers/g12Buffer.h"
#include "core/buffers/rgb24/rgb24Buffer.h"

using namespace corecvs;


class G12Image : public QImage
{
public:
    G12Image(G12Buffer *buffer, int newH, int newW);
    G12Image(G12Buffer *buffer, bool mirror = false);

    virtual ~G12Image();
};

class G8Image : public QImage
{
public:
    G8Image(G8Buffer *buffer);
    /*G8Image(G12Buffer *buffer, int newH, int newW);
    G8Image(G12Buffer *buffer, bool mirror);*/

    virtual ~G8Image();
};

class RGB24Image : public QImage
{
public:
    RGB24Image(RGB24Buffer *buffer, bool mirror = false);
    virtual ~RGB24Image();
};

QImage *toQImage(G12Buffer   *buffer);
QImage *toQImage(G8Buffer    *buffer);
QImage *toQImage(RGB24Buffer *buffer);


class RGB24InterfaceImage : public QImage
{
public:
    RGB24InterfaceImage(const RGB24Buffer *buffer);
    virtual ~RGB24InterfaceImage();
};


#endif /* G12IMAGE_H_ */
