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

    virtual bool acceptsFile(const std::string & name) override;
    virtual corecvs::RGB24Buffer *load(const std::string &name) override;
    virtual corecvs::RGB24Buffer *load(std::vector<unsigned char> &mem_vector);

    virtual std::string name() override { return "LibJpeg"; }
    virtual std::vector<std::string> extentions() override { return {".jpg", ".jpeg"}; }

    bool saveJPEG(const std::string& name, const corecvs::RGB24Buffer *buffer, int quality = 95, bool alpha = false);
    bool saveJPEG(std::vector<unsigned char> &mem_vector, const corecvs::RGB24Buffer *buffer, int quality = 95, bool alpha = false);

    LibjpegFileReader();
    virtual ~LibjpegFileReader();
};

class LibjpegFileSaver : public corecvs::BufferSaver<corecvs::RGB24Buffer>
{
public:
    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerSaver(new LibjpegFileSaver());
        return 0;
    }

    virtual bool acceptsFile(const std::string & name)  { return LibjpegFileReader().acceptsFile(name); }
    virtual bool save(const corecvs::RGB24Buffer& buffer, const std::string& name, int quality = 95) override { return LibjpegFileReader().saveJPEG(name, &buffer, quality); }
    virtual std::string              name()       override { return "LibJpeg_Saver"; }
    virtual std::vector<std::string> extentions() override { return LibjpegFileReader().extentions(); }
    virtual ~LibjpegFileSaver() {}
};

class LibjpegRuntimeTypeBufferLoader : public corecvs::BufferLoader<corecvs::RuntimeTypeBuffer>
{
public:
    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerLoader(new LibjpegRuntimeTypeBufferLoader());
        return 0;
    }

    virtual bool acceptsFile(const std::string & name) override;
    virtual corecvs::RuntimeTypeBuffer *load(const std::string &name) override;
    virtual std::string name() override {return "LibJpeg";}
    virtual std::vector<std::string> extentions() override { return LibjpegFileReader().extentions(); }

    LibjpegRuntimeTypeBufferLoader();
    virtual ~LibjpegRuntimeTypeBufferLoader();
};


#endif // LIBJPEGFILEREADER_H
