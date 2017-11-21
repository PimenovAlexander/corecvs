#ifndef LIBPNGFILEREADER_H
#define LIBPNGFILEREADER_H

#include <string>
#include <stdint.h>

#include "core/utils/global.h"

#include "core/fileformats/bufferLoader.h"
#include "core/buffers/bufferFactory.h"
#include "core/buffers/g12Buffer.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
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

    virtual bool acceptsFile(string name) override;
    virtual corecvs::RGB24Buffer * load(string name) override;
    virtual std::vector<string> extentions() override   { return std::vector<string>({prefix1, prefix2}); }
    virtual string name() override                      { return "LibPNG"; }
    virtual bool save(string name, corecvs::RGB24Buffer *buffer);
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
    
    virtual bool acceptsFile(string name)             { return LibpngFileReader().acceptsFile(name); }
    virtual bool save(corecvs::RGB24Buffer &buffer, string name) override { return LibpngFileReader().save(name, &buffer); }
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

    virtual bool acceptsFile(string name) override    { return LibpngFileReader().acceptsFile(name); }
    virtual corecvs::RuntimeTypeBuffer *load(string name) override;
    virtual string name() override                    { return "LibPNG_RuntimeTypeLoader"; }
    virtual std::vector<string> extentions() override { return LibpngFileReader().extentions(); }
    virtual ~LibpngRuntimeTypeBufferLoader() {}
};

#endif // LIBPNGFILEREADER_H
