#pragma once
/**
 * \file QTFileLoader.h
 * \brief This file is responsible for adding support for core of all file formats that are
 * supported by QT
 *
 * \date Jun 24, 2010
 * \author alexander
 */
#include <string>

#include <QImage>

#include "core/utils/global.h"

#include "core/fileformats/bufferLoader.h"
#include "core/buffers/g12Buffer.h"
#include "core/buffers/bufferFactory.h"
#include "core/buffers/rgb24/rgb24Buffer.h"

class QTemporaryFile;

class QTFileLoader
{
public:
    QTFileLoader() {}
    virtual ~QTFileLoader() {}

    static corecvs::RGB24Buffer *RGB24BufferFromQImage(QImage *image);
    static QImage*               RGB24BufferToQImage(corecvs::RGB24Buffer &buffer);
    static void                  save(const std::string& name, const corecvs::RGB24Buffer *input, int quality = 100);
    static QTemporaryFile*       saveTemporary(const QImage& image);
};

class QTG12Loader : public corecvs::BufferLoader<corecvs::G12Buffer>
{
public:
    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerLoader(new QTG12Loader());
        return 0;
    }

    virtual bool          acceptsFile(const std::string &name);
    virtual corecvs::G12Buffer * load(const std::string &name);

    virtual std::string name() override { return "QTG12Loader(multitype)"; }

    virtual ~QTG12Loader();
};

class QTRGB24Loader : public corecvs::BufferLoader<corecvs::RGB24Buffer>
{
public:
    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerLoader(new QTRGB24Loader());
        return 0;
    }

    virtual bool acceptsFile(const std::string & name) override;
    virtual corecvs::RGB24Buffer * load(const std::string & name)  override;
    virtual std::string name() override { return "QTRGB24Loader(multitype)"; }

    virtual ~QTRGB24Loader();
};

class QTRuntimeLoader : public corecvs::BufferLoader<corecvs::RuntimeTypeBuffer>
{
public:
    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerLoader(new QTRuntimeLoader());
        return 0;
    }

    virtual bool acceptsFile(const std::string & name);
    virtual corecvs::RuntimeTypeBuffer * load(const std::string & name);
    virtual std::string name() override { return "QTRuntimeLoader(multitype)"; }

    virtual ~QTRuntimeLoader();
};

class QTRGB24Saver : public corecvs::BufferSaver<corecvs::RGB24Buffer>
{
public:
    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerSaver(new QTRGB24Saver());
        return 0;
    }

    virtual bool acceptsFile(const std::string & name) override;
    virtual bool save(const corecvs::RGB24Buffer &buffer, const std::string& name, int quality = 100) override;

    virtual std::string              name()       override { return "QTRGB24Saver(multitype)"; }
    virtual std::vector<std::string> extentions() override { return {".jpeg", ".jpg" }; }
    virtual ~QTRGB24Saver() {}
};
