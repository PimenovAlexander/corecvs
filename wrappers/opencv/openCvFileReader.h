#ifndef OPENCVFILEREADER_H
#define OPENCVFILEREADER_H

#include "core/fileformats/bufferLoader.h"
#include "core/buffers/bufferFactory.h"
#include "core/features2d/bufferReaderProvider.h"

class OpenCvBufferReader : public BufferReader
{
public:
    corecvs::RuntimeTypeBuffer read(const std::string &s);
    corecvs::RGB24Buffer readRgb(const std::string &s);
    bool write(const corecvs::RuntimeTypeBuffer& buffer, const std::string &s);
    bool writeRgb(const corecvs::RGB24Buffer& buffer, const std::string &s);
    ~OpenCvBufferReader() {}
};

class OpenCvBufferReaderProvider : public BufferReaderProviderImpl
{
public:
    BufferReader* getBufferReader(const std::string &filename);
    bool provides(const std::string &filename);
    ~OpenCvBufferReaderProvider() {}
};

/**/
class OpenCVRGB24Loader : public corecvs::BufferLoader<corecvs::RGB24Buffer>
{
public:
    OpenCVRGB24Loader();

    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerLoader(new OpenCVRGB24Loader());
        return 0;
    }

    virtual bool acceptsFile(const std::string &name) override;
    virtual corecvs::RGB24Buffer *load(const std::string &name) override;
    virtual std::string name() override { return "OpenCVRGB24Loader(multitype)"; }
};

class OpenCVRuntimeTypeBufferLoader : public corecvs::BufferLoader<corecvs::RuntimeTypeBuffer>
{
public:
    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerLoader(new OpenCVRuntimeTypeBufferLoader());
        return 0;
    }

    OpenCVRuntimeTypeBufferLoader();
    virtual bool acceptsFile(const std::string &name) override;
    virtual corecvs::RuntimeTypeBuffer *load(const std::string &name) override;
    virtual std::string name() override { return "OpenCVRuntimeLoader(multitype)"; }
};

/**/

class OpenCVRGB24Saver : public corecvs::BufferSaver<corecvs::RGB24Buffer>
{
public:

    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerSaver(new OpenCVRGB24Saver());
        return 0;
    }

    virtual bool acceptsFile(const std::string &name) override;
    virtual bool save(const corecvs::RGB24Buffer &buffer, const std::string &name, int quality) override;
    virtual std::string              name()        override { return "OpenCVRGB24Saver"; }
    virtual std::vector<std::string> extentions() override {
        return std::vector<std::string>({".bmp", ".BMP", ".jpg", ".JPG", ".png", ".PNG"});
    }
    virtual ~OpenCVRGB24Saver() {}

};


extern "C"
{
    void init_opencv_reader_provider();
}

#endif // OPENCVFILEREADER_H
