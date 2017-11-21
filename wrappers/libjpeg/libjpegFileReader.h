#ifndef LIBJPEGFILEREADER_H
#define LIBJPEGFILEREADER_H

#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/bufferFactory.h"


class LibjpegFileReader : public corecvs::BufferLoader<corecvs::RGB24Buffer>
{
public:
    static std::string prefix1;
    static std::string prefix2;
    static std::string prefix3;
    static std::string prefix4;
public:
    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerLoader(new LibjpegFileReader());
        return 0;
    }

    virtual bool acceptsFile(std::string name) override;
    virtual corecvs::RGB24Buffer *load(std::string name) override;
    virtual std::string name() override { return "LibJpeg"; }

    LibjpegFileReader();
    virtual ~LibjpegFileReader();
};


class LibjpegRuntimeTypeBufferLoader : public corecvs::BufferLoader<corecvs::RuntimeTypeBuffer>
{
public:
    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerLoader(new LibjpegRuntimeTypeBufferLoader());
        return 0;
    }

    virtual bool acceptsFile(std::string name) override;
    virtual corecvs::RuntimeTypeBuffer *load(std::string name) override;
    virtual std::string name() override {return "LibJpeg";}

    LibjpegRuntimeTypeBufferLoader();
    virtual ~LibjpegRuntimeTypeBufferLoader();
};


#endif // LIBJPEGFILEREADER_H
