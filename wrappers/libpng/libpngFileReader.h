#ifndef LIBPNGFILEREADER_H
#define LIBPNGFILEREADER_H

#include <string>
#include <stdint.h>

#include "core/utils/global.h"

#include "core/fileformats/bufferLoader.h"
#include "core/buffers/bufferFactory.h"
#include "core/buffers/g12Buffer.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/rgb24/rgbTBuffer.h"
#include "core/buffers/runtimeTypeBuffer.h"

using std::string;

class LibpngFileReader : public corecvs::BufferLoader<corecvs::RGB24Buffer>
{
public:
    static string prefix1;
    static string prefix2;

    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerLoader(new LibpngFileReader());
        return 0;
    }

    const char *getColorTypeName(int value);

    virtual bool acceptsFile(const string &name) override;
    virtual corecvs::RGB24Buffer * load     (const string & name) override;
            corecvs::RGB48Buffer * loadRGB48(const string & name);

    virtual std::vector<string> extentions() override   { return std::vector<string>({prefix1, prefix2}); }
    virtual string name() override                      { return "LibPNG"; }
    virtual bool save(const string& name, const corecvs::RGB24Buffer *buffer, int quality = 95, bool alpha=false);

    bool savePNG(const string& name                    , const corecvs::RGB24Buffer *buffer, int quality = 95, bool alpha=false);
    bool savePNG(std::vector<unsigned char> &mem_vector, const corecvs::RGB24Buffer *buffer, int quality = 95, bool alpha=false);
    bool savePNG(const string& name, const corecvs::RGB48Buffer *buffer, int quality = 95, bool alpha=false);

    virtual ~LibpngFileReader() {}
};

class LibpngFileSaver : public corecvs::BufferSaver<corecvs::RGB24Buffer>
{
public:
    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerSaver(new LibpngFileSaver());
        return 0;
    }
    
    virtual bool acceptsFile(const string & name)     { return LibpngFileReader().acceptsFile(name); }
    virtual bool save(const corecvs::RGB24Buffer& buffer, const string& name, int quality = 95) override { return LibpngFileReader().save(name, &buffer, quality); }

    virtual string              name()       override { return "LibPNG_Saver"; }
    virtual std::vector<string> extentions() override { return LibpngFileReader().extentions(); }
    virtual ~LibpngFileSaver() {}
};

class LibpngRuntimeTypeBufferLoader : public corecvs::BufferLoader<corecvs::RuntimeTypeBuffer>
{
public:
    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerLoader(new LibpngRuntimeTypeBufferLoader());
        return 0;
    }

    virtual bool acceptsFile(const string & name) override    { return LibpngFileReader().acceptsFile(name); }
    virtual corecvs::RuntimeTypeBuffer *load(const string & name) override;
    virtual string name() override                    { return "LibPNG_RuntimeTypeLoader"; }
    virtual std::vector<string> extentions() override { return LibpngFileReader().extentions(); }
    virtual ~LibpngRuntimeTypeBufferLoader() {}
};

class KittiFlowLoader : public corecvs::BufferLoader<corecvs::FloatFlowBuffer>
{
public:
    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerLoader(new KittiFlowLoader());
        return 0;
    }

    virtual bool acceptsFile(const string & name) override    { return LibpngFileReader().acceptsFile(name); }
    virtual corecvs::FloatFlowBuffer *load(const string & name) override;
    virtual string name() override                    { return "Kitti Flow Loader"; }
    virtual std::vector<string> extentions() override { return LibpngFileReader().extentions(); }
    virtual ~KittiFlowLoader() {}
};


class KittiFlowSaver : public corecvs::BufferSaver<corecvs::FloatFlowBuffer>
{
public:
    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerSaver(new KittiFlowSaver());
        return 0;
    }

    virtual bool acceptsFile(const string & name)     override  { return LibpngFileReader().acceptsFile(name); }
    virtual bool save(const corecvs::FloatFlowBuffer& buffer, const string& name, int quality = 100) override ;

    virtual string              name()       override { return "Kitti Flow Saver"; }
    virtual std::vector<string> extentions() override { return LibpngFileReader().extentions(); }
    virtual ~KittiFlowSaver() {}
};

#endif // LIBPNGFILEREADER_H
