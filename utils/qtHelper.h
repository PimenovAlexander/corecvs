#pragma once

/**
 * \file qtHelper.h
 *
 * \date Apr 27, 2013
 **/

#include <QtCore/QDebug>
#include <QtGui/QImage>

#include <QDoubleSpinBox>
#include <QItemSelectionModel>
#include <QTransform>
#include <QWidget>
#include <QString>
#include <QFileInfo>
#include <QDir>

#include "math/vector/vector2d.h"
#include "math/vector/vector3d.h"
#include "geometry/rectangle.h"
#include "math/matrix/matrix33.h"

#include "corestructs/parametersControlWidgetBase.h"

using corecvs::Vector2d;
using corecvs::Vector2d32;
using corecvs::Vector2dd;
using corecvs::Rectangle32;
using corecvs::Vector3dd;
using corecvs::Matrix33;


class Core2Qt {
public:

    static QPoint QPointFromVector2d32(const Vector2d32 &point)
    {
        return QPoint(point.x(), point.y());
    }

    static QPointF QPointFromVector2dd(const Vector2dd &point)
    {
        return QPointF(point.x(), point.y());
    }

    static QRect QRectFromRectangle (const Rectangle32 &rect)
    {
        return QRect(
            rect.corner.x(),  rect.corner.y(),
            rect.size.  x(),  rect.size.  y());
    }

    static QTransform QTransformFromMatrix(const Matrix33 &m);
};

class Qt2Core {
public:

    static Vector2dd Vector2ddFromQPoint(const QPoint &point)
    {
        return Vector2dd(point.x(), point.y());
    }

    static Vector2dd Vector2ddFromQPointF(const QPointF &point)
    {
        return Vector2dd(point.x(), point.y());
    }

    static Vector2d32 Vector2d32FromQPoint(const QPoint &point)
    {
        return Vector2d32(point.x(), point.y());
    }

    static Rectangle32 RectangleFromQRect (const QRect &rect)
    {
        return Rectangle32(
            rect.left(),  rect.top(),
            rect.width(), rect.height());
    }
};


QDebug & operator<< (QDebug & stream, const Vector2dd & vector);
QDebug & operator<< (QDebug & stream, const Vector3dd & vector);

QDebug & operator<< (QDebug & stream, const Vector2d<int> & vector);


void    setValueBlocking(QDoubleSpinBox *box, double value);

QString printWindowFlags(const Qt::WindowFlags &flags);
QString printWindowState(const Qt::WindowStates &state);
QString printSelectionModel(const QItemSelectionModel::SelectionFlags &flag);
QString printWidgetAttributes(QWidget *widget);

QString printQImageFormat(const QImage::Format &format);
QString printModelItemRole(int role);
/* File management support 
 */
namespace
{

    inline QString addFileExtIfNotExist(const QString& fileName, const QString& ext)
    {
        return fileName.endsWith(ext) ? fileName : fileName + ext;
    }

    inline QString getDirectory(const QString& absoluteFilePath)
    {
        Q_ASSERT(!absoluteFilePath.isEmpty());
        QFileInfo info(absoluteFilePath);
        return info.dir().absolutePath();
    }

    inline QString getFileName(const QString& fileName)
    {
        QFileInfo info(fileName);
        return info.fileName();
    }

    inline QString getFileNameIfExist(const QString& fileName, const QString& relativePath)
    {
        //std::cout << fileName.toStdString() << std::endl;
        QFileInfo info(fileName);
        if (info.exists())
            return fileName;

        QFileInfo infoNew(relativePath + PATH_SEPARATOR + info.fileName());
        if (infoNew.exists())
            return infoNew.absoluteFilePath();

        std::cout << "couldn't locate <" << fileName.toStdString() << "> with relativePath:" << relativePath.toStdString() << std::endl;
        return "";
    }


} // namespace


class RecentPathKeeper : public SaveableWidget
{
public:
    static const QString LAST_INPUT_DIRECTORY_KEY ;
    static const QString LAST_OUTPUT_DIRECTORY_KEY;
    static const QString RECENT_SCENES_KEY;

    static const int MAX_RECENT = 10;

    QString in;             // last input  dir
    QString out;            // last output dir

    std::vector<std::string> lastScenes;

    void    updateIn (const QString& absoluteFilePath) { in  = getDirectory(absoluteFilePath); }
    void    updateOut(const QString& absoluteFilePath) { out = getDirectory(absoluteFilePath); }

    void    addRecent(const std::string &recent) {
        if (std::find(lastScenes.begin(), lastScenes.end(), recent) == lastScenes.end())
            lastScenes.push_back(recent);

        while (lastScenes.size() >= MAX_RECENT) {
            lastScenes.erase(lastScenes.begin(), lastScenes.begin() + 1);
        }
    }


    virtual void loadFromQSettings(const QString &fileName, const QString &_root) override;
    virtual void saveToQSettings  (const QString &fileName, const QString &_root) override;

    ~RecentPathKeeper();

};

/* EOF */
