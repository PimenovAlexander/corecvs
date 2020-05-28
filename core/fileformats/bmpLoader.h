#pragma once
/**
 * \file bmpLoader.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */
#include <string>
#include <stdint.h>

#include "utils/global.h"

#include "fileformats/bufferLoader.h"
#include "buffers/g12Buffer.h"
#include "buffers/rgb24/rgb24Buffer.h"

namespace corecvs {

using std::string;


class BMPHeader {
public:
    bool trace = false;

public:
    static const unsigned HEADER_SIZE = 0x36;
    static uint8_t HEADER_SIGNATURE[2];

    uint32_t bmpSize;
    uint32_t headerSize;
    uint32_t w;
    uint32_t h;
    uint16_t planes;
    uint16_t bpp;
    uint32_t dataLen;


    int lineLength;
    int bytesPerPixel;

    int parseHeader(FILE *fp);
};


class BMPLoaderBase
{
protected:
    static string prefix1;

public:
    bool trace = false;

    BMPLoaderBase();
    virtual ~BMPLoaderBase();

    bool acceptsFile(string name);

    virtual G12Buffer   * loadG12(string name);
    virtual RGB24Buffer * loadRGB(string name);

    virtual bool save(const string& name, const RGB24Buffer *buffer);
    virtual bool save(const string& name, const G12Buffer   *buffer);
    virtual bool save(const string& name, const G8Buffer    *buffer);

private:
    int parseBMP (string& name, BMPHeader *header, uint8_t **dataPtr);
};

/**
 * @brief BMPLoader is a shortcut for BMPLoaderBase
 */
typedef BMPLoaderBase BMPLoader;

class BMPLoaderG12 : public BufferLoader<G12Buffer>, public BMPLoaderBase
{
public:
    virtual bool acceptsFile(const string & name) override
    {
        return BMPLoaderBase::acceptsFile(name);
    }

    virtual G12Buffer *load(const string & name) override
    {
        return BMPLoaderBase::loadG12(name);
    }

    virtual std::string name() override { return "BMPLoaderG12"; }
    virtual std::vector<std::string> extentions() override {
        return std::vector<std::string>({ BMPLoaderBase::prefix1 });
    }
};

class BMPLoaderRGB24 : public BufferLoader<RGB24Buffer>, public BMPLoaderBase
{
public:
    virtual bool acceptsFile(const string &name) override
    {
       return BMPLoaderBase::acceptsFile(name);
    }

    virtual RGB24Buffer *load(const string &name) override
    {
        return BMPLoaderBase::loadRGB(name);
    }

    virtual std::string name() override { return "BMPLoaderRGB24"; }
    virtual std::vector<std::string> extentions() override {
        return std::vector<std::string>({ BMPLoaderBase::prefix1 });
    }
};

class BMPSaverRGB24 : public BufferSaver<RGB24Buffer>, public BMPLoaderBase
{
    virtual bool acceptsFile(const std::string & name) {
        return BMPLoaderBase::acceptsFile(name);
    }
    virtual bool save(const RGB24Buffer &buffer, const string& name, int quality = 95) override {
        CORE_UNUSED(quality);
        return BMPLoaderBase::save(name, &buffer);
    }

    virtual std::string name() override { return "BMPSaverRGB24"; }
    virtual std::vector<std::string> extentions() override {
        return std::vector<std::string>({BMPLoaderBase::prefix1});
    }
    virtual ~BMPSaverRGB24() {}
};

} //namespace corecvs
