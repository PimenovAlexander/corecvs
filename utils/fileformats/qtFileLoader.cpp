/**
 * \file QTFileLoader.cpp
 * \brief Add Comment Here
 *
 * \date Jun 24, 2010
 * \author alexander
 */
#include <QImage>
#include <QImageWriter>

#include "qtFileLoader.h"
#include "g12Image.h"

/**
 *  This is a form of dirty hack to be sure that int QT builds loading QT images will always
 *  work without any change of code.
 **/
//int QTFileLoader::DUMMY = 1;//QTFileLoader::registerMyself();

RGB24Buffer *QTFileLoader::RGB24BufferFromQImage(QImage *image)
{
    if (image == NULL)
        return NULL;

    RGB24Buffer *result = new RGB24Buffer(image->height(), image->width(), false);

    if (image->format() == QImage::Format_ARGB32 || image->format() == QImage::Format_RGB32 )
    {
        for (int i = 0; i < image->height(); i++)
        {
            uint8_t *in = (uint8_t *)image->scanLine(i);
            RGBColor *out = &result->element(i, 0);
            for (int j = 0; j < image->width(); j++)
            {
                uint8_t r = *(in++);
                uint8_t g = *(in++);
                uint8_t b = *(in++);
                *out = RGBColor(b,g,r);
                in++;
                out++;
            }
        }
    } else {
        /**
         * TODO: Make this faster using .bits() method.
         * So far don't want to mess with possible image formats
         *
         */
        qDebug("QTFileLoader::RGB24BufferFromQImage():Slow conversion.");
        for (int i = 0; i < image->height(); i++)
        {
            for (int j = 0; j < image->width(); j++)
            {
                QRgb pixel = image->pixel(j,i);
                result->element(i,j) = RGBColor(qRed(pixel), qGreen(pixel), qBlue(pixel));
            }
        }
    }

    return result;
}

QImage* QTFileLoader::RGB24BufferToQImage(RGB24Buffer &buffer)
{
    auto* img = new QImage(buffer.w, buffer.h, QImage::Format_RGB32);
    for (int i = 0; i < buffer.h; ++i)
    {
        for (int j = 0; j < buffer.w; ++j)
        {
            auto& src = buffer.element(i, j);
            img->setPixel(j, i, QColor(src.r(), src.g(), src.b()).rgb());
        }
    }
    return img;
}

void QTFileLoader::save(const std::string& name, RGB24Buffer *input)
{
    save(name, input, 95);
}

void QTFileLoader::save(const std::string& name, RGB24Buffer *input, int quality)
{
    QImageWriter imageWriter(QString(name.c_str()));
    RGB24InterfaceImage imageToSave(input);
    imageWriter.setQuality(quality);
    imageWriter.write(imageToSave);
}


G12Buffer* QTG12Loader::load(string name)
{
    QString qtName = QString::fromStdString(name);
    QImage image(qtName);

    if (image.isNull())
        return NULL;

    G12Buffer *result = new G12Buffer(image.height(), image.width(), false);

    /**
     * TODO: Make this faster using .bits() method.
     * So far don't want to mess with possible image formats
     *
     */
    for (int i = 0; i < image.height(); i++)
    {
        for (int j = 0; j < image.width(); j++)
        {
            QRgb pixel = image.pixel(j,i);
            // Do conversion without qGray to possibly save 9-th bit
            uint16_t gray = (11 * qRed(pixel) + 16 * qGreen(pixel) + 5 * qBlue(pixel)) >> 1;
            result->element(i,j) = gray;
        }
    }

    return result;
}

bool QTG12Loader::acceptsFile(string /*name*/)
{
    return true;
}

RGB24Buffer* QTRGB24Loader::load(string name)
{
    SYNC_PRINT(("QTRGB24Loader::load(%s): called\n", name.c_str()));
    QString qtName = QString::fromStdString(name);
    QImage image(qtName);

    if (image.isNull()) {
        return NULL;
    }

    return QTFileLoader::RGB24BufferFromQImage(&image);
}

bool QTRGB24Loader::acceptsFile(string /*name*/)
{
    return true;
}


QTG12Loader::~QTG12Loader()
{
    // TODO Auto-generated destructor stub
}

QTRGB24Loader::~QTRGB24Loader()
{
    // TODO Auto-generated destructor stub
}
