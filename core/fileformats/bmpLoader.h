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

#include "global.h"

#include "bufferLoader.h"
#include "g12Buffer.h"
#include "rgb24Buffer.h"

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


class BMPLoader : public BufferLoader<G12Buffer>
{
    static string prefix1;

public:
    bool trace = false;

    BMPLoader();
    virtual ~BMPLoader();

    virtual bool acceptsFile(string name);

    virtual G12Buffer   * load   (string name);
    virtual RGB24Buffer * loadRGB(string name);

    virtual bool save(string name, RGB24Buffer *buffer);
    virtual bool save(string name, G12Buffer   *buffer);
    virtual bool save(string name, G8Buffer   *buffer);

private:
    int parseBMP (string& name, BMPHeader *header, uint8_t **dataPtr);
};


} //namespace corecvs
