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

using namespace std;
using namespace corecvs;

class QTFileLoader: public BufferLoader<G12Buffer>
{
private:
    static int DUMMY;

public:
    QTFileLoader();
    virtual ~QTFileLoader();

    static int registerMyself()
    {
        BufferFactory::getInstance()->registerLoader(new QTFileLoader());   // TODO: memory leak: this loader is never destroyed!!!
        return 0;
    }

/*  static G12Buffer   *G12BufferFromQImage(QImage *image);*/
    static RGB24Buffer *RGB24BufferFromQImage(QImage *image);

    virtual bool acceptsFile(string name);

    virtual G12Buffer * load(string name);
    void save(string name, RGB24Buffer *input);

};
