/**
 * \file g12Image.cpp
 *
 * \date Jul 22, 2009
 * \author: Alexander Pimenov
 */

#include "g12Image.h"
#include <QtGui/QPainter>

G12Image::G12Image(G12Buffer *buffer) : QImage (buffer->w, buffer->h, QImage::Format_RGB32){
    int i,j;
    uint8_t *data = bits();
    int bpl = bytesPerLine();
    //int depth = image.depth;
    for (i = 0; i < buffer->h; i++ )
        for (j = 0; j < buffer->w; j++ )
        {
            uint8_t c = buffer->element(i, j) >> 4;
            uint32_t value = c | (c << 8) | (c << 16) | (0xFF << 24);
            ((uint32_t *)(data + i * bpl))[j] = value;
        }
}

G12Image::G12Image(G12Buffer *buffer, int newH, int newW) : QImage (newW, newH, QImage::Format_RGB32){
    int i,j;
    uint8_t *data = bits();
    int bpl = bytesPerLine();
    //int depth = image.depth;
    for (i = 0; i < newH; i++ )
        for (j = 0; j < newW; j++ )
        {
            uint32_t value;
            if (i < buffer->h && j < buffer->w)
            {
                uint8_t c = buffer->element(i, j) >> 4;
                value = c | (c << 8) | (c << 16) | (0xFF << 24);
            } else
            {
                value = 0xFF000000UL;
            }

            ((uint32_t *)(data + i * bpl))[j] = value;
        }
}



G12Image::G12Image(G12Buffer *buffer, bool invert) : QImage (buffer->w, buffer->h, QImage::Format_RGB32){
    int i,j;
    uint8_t *data = bits();
    int bpl = bytesPerLine();
    //int depth = image.depth;
    for (i = 0; i < buffer->h; i++ )
        for (j = 0; j < buffer->w; j++ )
        {
            uint8_t c;
            if (!invert)
                c = buffer->element(i, j) >> 4;
            else
                c = buffer->element(i, buffer->w - 1 - j  ) >> 4;

            uint32_t value = c | (c << 8) | (c << 16) | (0xFF << 24);
            ((uint32_t *)(data + i * bpl))[j] = value;
        }
}

G12Image::~G12Image() {
}


RGB24Image::RGB24Image(RGB24Buffer *buffer) : QImage (buffer->w, buffer->h, QImage::Format_RGB32){
    int i,j;
    uint8_t *data = bits();
    int bpl = bytesPerLine();
    for (i = 0; i < buffer->h; i++ )
        for (j = 0; j < buffer->w; j++ )
        {
            ((uint32_t *)(data + i * bpl))[j] = buffer->element(i, j).color();
        }
}

RGB24Image::~RGB24Image() {
    // \todo TODO Auto-generated destructor stub
}

RGB24InterfaceImage::RGB24InterfaceImage(RGB24Buffer *buffer) : QImage((uchar *)buffer->data, buffer->w, buffer->h, QImage::Format_RGB32)
{

}

RGB24InterfaceImage::~RGB24InterfaceImage() {

}


void ImageWidget::setImage(QImage *newImage)
{
    QImage *oldImage = image;
    image = newImage;
    delete oldImage;
    this->resize(image->size());
    this->update();

}

void ImageWidget::paintEvent(QPaintEvent * event)
{
    QPainter p(this);
    if (image != NULL)
        p.drawImage(QPoint(0,0), *image);
    ViAreaWidget::paintEvent(event);
}
