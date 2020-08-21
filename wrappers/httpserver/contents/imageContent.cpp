//#include <QtCore/QBuffer>

#include "imageContent.h"


ImageContent::ImageContent(std::shared_ptr<corecvs::RGB24Buffer>, double scale, std::string format) :
    mScale(scale),
    mFormat(format),
    mImage(image)
{
}

std::vector<uint8_t> ImageContent::getContent()
{
    std::vector<uint8_t> data;
    QBuffer buffer(&data);
    if (mImage != NULL)
    {
        buffer.open(QIODevice::WriteOnly);
        if (mScale == 1.0) {
            mImage->save(&buffer, mFormat.toLatin1());
        } else {
            mImage->scaled(mImage->size() * mScale).save(&buffer, mFormat.toLatin1());
        }

    } else {
        QImage image(100,100, QImage::Format_RGB32);
        image.fill(Qt::cyan);
        /*QPainter p(&image);
        p.setPen(Qt::black);
        p.drawText(5, 30, "No image");
        p.end();*/
        image.save(&buffer, mFormat.toLatin1());
    }
    return data;
}

QString ImageContent::getContentType()
{
    return QString("image/%1").arg(mFormat.toLower());
}

ImageListContent::ImageListContent(QList<QString> names) :
    mNames(names)
{

}

QByteArray ImageListContent::getContent()
{
    QByteArray data;
    data.append("<h1>Frames</h1>\n");
    data.append("<ol>\n");
    foreach (QString name , mNames)
    {
        data.append(QString("  <li><a href=\"frame.jpg?name=%1\">").arg(name));
        data.append( name );
        data.append("</a>&nbsp");
        data.append(QString("<a href=\"frame.bmp?name=%1\">bmp</a>&nbsp").arg(name));
        data.append(QString("<a href=\"frame.png?name=%1\">png</a>").arg(name));
        data.append("</li>\n");
    }

    data.append("</ol>\n");
    return data;
}

QString ImageListContent::getContentType()
{
    return "text/html";
}
