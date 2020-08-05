#ifndef LIB_GIF_FILEREADER_H
#define LIB_GIF_FILEREADER_H

#include <string>
#include <stdint.h>


#include "core/utils/global.h"

#include "core/fileformats/bufferLoader.h"
#include "core/buffers/bufferFactory.h"
#include "core/buffers/g12Buffer.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/rgb24/rgbTBuffer.h"
#include "core/buffers/runtimeTypeBuffer.h"
#include "core/framesources/dummyVideoEncoderInterface.h"

using std::string;

class LibgifFileReader : public corecvs::BufferLoader<corecvs::RGB24Buffer>
{
public:
    static string prefix1;
    static string prefix2;

    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerLoader(new LibgifFileReader());
        return 0;
    }

    static const char *getErrorName    (int value);
    static const char *getExtentionName(int value);

    virtual bool acceptsFile(const string &name) override;
    virtual corecvs::RGB24Buffer * load     (const string & name) override;

    virtual std::vector<string> extentions() override   { return std::vector<string>({prefix1, prefix2}); }
    virtual string name() override                      { return "LibGIF"; }
    virtual bool save(const string& name, const corecvs::RGB24Buffer *buffer, int quality = 95, bool alpha=false);

    bool saveGIF(const string& name, const corecvs::RGB24Buffer *buffer, int quality = 95, bool alpha=false);

    virtual ~LibgifFileReader() {}
};

class LibgifFileSaver : public corecvs::BufferSaver<corecvs::RGB24Buffer>
{
public:
    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerSaver(new LibgifFileSaver());
        return 0;
    }
    
    virtual bool acceptsFile(const string & name)     { return LibgifFileReader().acceptsFile(name); }
    virtual bool save(const corecvs::RGB24Buffer& buffer, const string& name, int quality = 95) override { return LibgifFileReader().save(name, &buffer, quality); }

    virtual string              name()       override { return "LibGIF_Saver"; }
    virtual std::vector<string> extentions() override { return LibgifFileReader().extentions(); }
    virtual ~LibgifFileSaver() {}
};

class LibpgifRuntimeTypeBufferLoader : public corecvs::BufferLoader<corecvs::RuntimeTypeBuffer>
{
public:
    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerLoader(new LibpgifRuntimeTypeBufferLoader());
        return 0;
    }

    virtual bool acceptsFile(const string & name) override    { return LibgifFileReader().acceptsFile(name); }
    virtual corecvs::RuntimeTypeBuffer *load(const string & name) override;
    virtual string name() override                    { return "LibGIF_RuntimeTypeLoader"; }
    virtual std::vector<string> extentions() override { return LibgifFileReader().extentions(); }
    virtual ~LibpgifRuntimeTypeBufferLoader() {}
};

class GifFileType;

class GifEncoder : corecvs::DummyVideoEncoderInterface {

    GifFileType *gif = NULL;
    // DummyVideoEncoderInterface interface
public:
    virtual int startEncoding(const std::string &name, int h, int w, int codec_id = -1) override;
    virtual void addFrame(corecvs::RGB24Buffer *) override;
    virtual void endEncoding() override;
};


#endif // LIB_GIF_FILEREADER_H
