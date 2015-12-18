#pragma once
/**
 * \file QTFileLoader.h
 * \brief This file is responsible for adding support for core of all file formats that are
 * supported by QT
 *
 * \date Jun 24, 2010
 * \author alexander
 */
#include <string.h>

#include <QImage>

#include "global.h"

#include "bufferLoader.h"
#include "g12Buffer.h"
#include "bufferFactory.h"
#include "rgb24Buffer.h"

using namespace corecvs;

using std::string;

class QTFileLoader
{
public:
    QTFileLoader() {}
    virtual ~QTFileLoader() {}

/*  static G12Buffer   *G12BufferFromQImage(QImage *image);*/

    static RGB24Buffer *RGB24BufferFromQImage(QImage *image);
    static QImage* RGB24BufferToQImage(RGB24Buffer &buffer);
    void save(const std::string& name, RGB24Buffer *input);
    void save(const std::string& name, RGB24Buffer *input, int quality);
};

class QTG12Loader : public BufferLoader<G12Buffer>
{
public:
    static int registerMyself()
    {
        BufferFactory::getInstance()->registerLoader(new QTG12Loader());   // TODO: memory leak: this loader is never destroyed!!!
        return 0;
    }

    virtual bool acceptsFile(string name);
    virtual G12Buffer * load(string name);

    virtual ~QTG12Loader();

};


class QTRGB24Loader : public BufferLoader<RGB24Buffer>
{
public:
    static int registerMyself()
    {
        BufferFactory::getInstance()->registerLoader(new QTRGB24Loader());   // TODO: memory leak: this loader is never destroyed!!!
        return 0;
    }

    virtual bool acceptsFile(string name);
    virtual RGB24Buffer * load(string name);

    virtual ~QTRGB24Loader();

};
