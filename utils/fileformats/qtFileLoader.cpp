/**
 * \file QTFileLoader.cpp
 * \brief Add Comment Here
 *
 * \date Jun 24, 2010
 * \author alexander
 */
#include <QImage>
#include <QImageWriter>
#include <QTemporaryFile>
#include <QStandardPaths>
#include <QDebug>

#include "qtFileLoader.h"
#include "g12Image.h"
#include "qtHelper.h"

using std::string;
using namespace corecvs;


/**
 *  This is a form of dirty hack to be sure that int QT builds loading QT images will always
 *  work without any change of code.
 **/
//int QTFileLoader::DUMMY = 1;//QTFileLoader::registerMyself();

corecvs::RGB24Buffer *QTFileLoader::RGB24BufferFromQImage(QImage *image)
{
    if (image == NULL)
        return NULL;

    corecvs::RGB24Buffer *result = new corecvs::RGB24Buffer(image->height(), image->width(), false);

    if (image->format() == QImage::Format_ARGB32 || image->format() == QImage::Format_RGB32)
    {
        for (int i = 0; i < image->height(); i++)
        {
            uint8_t *in = (uint8_t *)image->scanLine(i);
            RGBColor *out = &result->element(i, 0);
            for (int j = 0; j < image->width(); j++)
            {
                uint8_t b = *(in++);
                uint8_t g = *(in++);
                uint8_t r = *(in++);
                *out = RGBColor(r, g, b);
                in++;
                out++;
            }
        }
    }
    else
    {
        /**
         * TODO: Make this faster using .bits() method.
         * So far don't want to mess with possible image formats
         */
        qDebug() << QString("QTFileLoader::RGB24BufferFromQImage(%1):Slow conversion.").arg(printQImageFormat(image->format()));
        for (int i = 0; i < image->height(); ++i)
        {
            for (int j = 0; j < image->width(); ++j)
            {
                QRgb pixel = image->pixel(j, i);
                result->element(i,j) = RGBColor(qRed(pixel), qGreen(pixel), qBlue(pixel));
            }
        }
    }

    return result;
}

QImage* QTFileLoader::RGB24BufferToQImage(RGB24Buffer &buffer)
{
    if (!buffer.isAllocated() || buffer.numElements() == 0)
    {
        return new QImage();
    }

    QImage* img = new QImage(buffer.w, buffer.h, QImage::Format_RGB32);
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

void QTFileLoader::save(const std::string& name, const corecvs::RGB24Buffer *input, int quality)
{
    //qDebug() << "Saving to" << name.c_str() << "[" << input->w << "x" << input->h << " st:" << input->stride << " sz:" << input->sizeInBytes() << "] ptr:" << hex << (void *)input->data;
    //{
    //    std::ostringstream s;
    //    s << "[0,0]= " << input->element(0, 0) << "  [w-1,h-1]= " << input->element(input->h - 1, input->w - 1);
    //    qDebug() << "\tinput RGB24Buffer elements: " << s.str().c_str();
    //}

    QImageWriter imageWriter(QString::fromStdString(name));
    RGB24InterfaceImage imageToSave(input);
    imageWriter.setQuality(quality);
    imageWriter.write(imageToSave);
}

QTemporaryFile* QTFileLoader::saveTemporary(const QImage& image)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::TempLocation);

    QTemporaryFile* file = new QTemporaryFile(path + "/temp_file");
    file->open();

    qDebug() << "Saving to" << file->fileName();

    image.save(file->fileName(), "JPG", 100);
    return file;
}


G12Buffer* QTG12Loader::load(const string &name)
{
    QImage image(QString::fromStdString(name));
    if (image.isNull())
        return NULL;

    G12Buffer *result = new G12Buffer(image.height(), image.width(), false);
    /**
     * TODO: Make this faster using .bits() method.
     * So far don't want to mess with possible image formats
     *
     */
    for (int i = 0; i < image.height(); ++i)
    {
        for (int j = 0; j < image.width(); ++j)
        {
            QRgb pixel = image.pixel(j, i);
            // Do conversion without qGray to possibly save 9-th bit
            uint16_t gray = (11 * qRed(pixel) + 16 * qGreen(pixel) + 5 * qBlue(pixel)) >> 1;
            result->element(i, j) = gray;
        }
    }

    return result;
}

bool QTG12Loader::acceptsFile(const std::string & /*name*/)
{
    return true;
}

corecvs::RGB24Buffer* QTRGB24Loader::load(const std::string &name)
{
    SYNC_PRINT(("QTRGB24Loader::load(%s): called\n", name.c_str()));

    QImage image(QString::fromStdString(name));
    if (image.isNull())
    {
        SYNC_PRINT(("QTRGB24Loader::load(%s): failed!\n", name.c_str()));
        return NULL;
    }

    return QTFileLoader::RGB24BufferFromQImage(&image);
}

bool QTRGB24Loader::acceptsFile(const string & /*name*/)
{
    return true;
}


QTG12Loader::~QTG12Loader()
{}

QTRGB24Loader::~QTRGB24Loader()
{}

bool QTRuntimeLoader::acceptsFile(const std::string & /*name*/)
{
    return true;
}

RuntimeTypeBuffer *QTRuntimeLoader::load(const std::string &name)
{
    QImage image(QString::fromStdString(name));
    if (image.isNull())
        return NULL;

    RuntimeTypeBuffer *result = new RuntimeTypeBuffer(image.height(), image.width());

    /**
     * TODO: Make this faster using .bits() method.
     * So far don't want to mess with possible image formats
     */
    for (int i = 0; i < image.height(); ++i)
    {
        for (int j = 0; j < image.width(); ++j)
        {
            QRgb pixel = image.pixel(j, i);
            RGBColor color(qRed(pixel), qGreen(pixel), qBlue(pixel));
            result->at<uint8_t>(i,j) = (uint8_t)color.y();              // don't use qGray(pixel) as it works not the same as opencv's imread(colorImage, Gray)
        }
    }

    return result;
}

QTRuntimeLoader::~QTRuntimeLoader()
{}

bool QTRGB24Saver::acceptsFile(const std::string &name)
{
    CORE_UNUSED(name);
    return true;
}

bool QTRGB24Saver::save(const corecvs::RGB24Buffer &buffer, const std::string& name, int quality)
{
    QTFileLoader().save(name, &buffer, quality);
    return true;
}
