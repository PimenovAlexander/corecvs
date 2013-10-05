/**
 * \file bmpLoader.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */

#include "bmpLoader.h"

#include <string>
//#include <cstring>
#include <stdio.h>

namespace corecvs {

string  BMPLoader::prefix1(".bmp");
uint8_t BMPHeader::HEADER_SIGNATURE[2] = {'B','M'};

BMPLoader::BMPLoader()
{
}

bool BMPLoader::acceptsFile(string name)
{
    return (name.compare(name.length() - prefix1.length(), prefix1.length(), prefix1) == 0);
}

int BMPHeader::parseHeader(FILE *fp)
{
    int status = -1;
    size_t result = 0;
    uint8_t *header = NULL;

    header = new uint8_t[BMPHeader::HEADER_SIZE];
    result = fread(header, sizeof(uint8_t), BMPHeader::HEADER_SIZE, fp);

    if (result != BMPHeader::HEADER_SIZE)
    {
        DOTRACE(("Could not read file header of size %d. Is end of file : %s",
                BMPHeader::HEADER_SIZE, feof(fp) ? "true" : "false"));
        goto fail;
    }

    if (header[0] != BMPHeader::HEADER_SIGNATURE[0] ||
        header[1] != BMPHeader::HEADER_SIGNATURE[1])
        goto fail;

    bmpSize = *((uint32_t *)(header + 0x2));
    headerSize = *((uint32_t *)(header + 0xA));
    w       = *((uint32_t *)(header + 0x12));
    h       = *((uint32_t *)(header + 0x16));
    planes  = *((uint16_t *)(header + 0x1A));
    bpp     = *((uint16_t *)(header + 0x1C));
    dataLen = *((uint32_t *)(header + 0x22));

    if (planes != 1)
    {
        DOTRACE(("Sorry, only BMP's with 1 plane are supported"));
        goto fail;
    }

    if (bpp != 24)
    {
        DOTRACE(("Sorry, only BMP's with 24 bit depth are supported"));
        goto fail;
    }

    lineLength = w * (bpp / 8);
    lineLength = (lineLength & 0x3) ? (lineLength | 0x3) + 1 : lineLength;

    if (headerSize != BMPHeader::HEADER_SIZE)
    {
        DOTRACE(("BMP Header seems to be malformed inner size is %d expected %d",
                headerSize, BMPHeader::HEADER_SIZE));
    }

    if (dataLen != bmpSize - headerSize)
    {
        DOTRACE(("BMP data about the size of the pixel array is contradictory\n"
                 "bmp size says %d and header says %d.\n We will choose maximium\n",
                  bmpSize - headerSize, dataLen));
        if ((bmpSize - headerSize) > dataLen)
            dataLen = bmpSize - headerSize;
    }

    if (dataLen != lineLength * h)
    {
        DOTRACE(("BMP data about the size of the pixel array is contradictory\n"
                 "bmp size says %d and width and bpp says %d.\n Choosing minimum\n",
                 dataLen, lineLength * h));
        dataLen = dataLen < lineLength * h ? dataLen : lineLength * h;
    }

    status = 0;
fail:
    if (header)
        delete[] header;
    return status;
}


int BMPLoader::parseBMP (string& name, BMPHeader *header, uint8_t **dataPtr)
{
    int status = -1;
    FILE *fp = NULL;
    size_t result = 0;
    uint8_t *data = NULL;


    fp = fopen(name.c_str(), "rb");
    if (fp == NULL)
       goto fail;

    if (header->parseHeader(fp) != 0)
       goto fail;

    data = new uint8_t[header->dataLen];
    if (data == NULL)
       goto fail;

    result = fread(data, sizeof(uint8_t), header->dataLen, fp);
    if (result != header->dataLen)
       goto fail;

    *dataPtr = data;
    data = NULL;
    status = 0;
fail:
    if (fp != NULL)
        fclose(fp);
    if (data != NULL)
        delete data;
    return status;
}


G12Buffer * BMPLoader::load(string name)
{
    uint8_t *data = NULL;
    BMPHeader header;

    G12Buffer *toReturn = NULL;

    if (parseBMP(name, &header, &data) != 0)
        goto fail;

    toReturn = new G12Buffer(header.h, header.w);

    for (unsigned i = 0; i < header.h; i++)
    {
        uint8_t  *src = &(data[header.lineLength * (header.h - i - 1)]);
        uint16_t *dest = &toReturn->element(i,0);
        for (unsigned j = 0; j < header.w; j++)
        {
            uint8_t b = src[0];
            uint8_t g = src[1];
            uint8_t r = src[2];

            uint16_t gray = RGBColor(r,g,b).luma12();
            *dest = gray;
            src += 3;
            dest++;
        }
    }
fail:
    if (data)
        delete[] data;
    return toReturn;
}


RGB24Buffer * BMPLoader::loadRGB(string name)
{
    uint8_t *data = NULL;
    BMPHeader header;

    RGB24Buffer *toReturn = NULL;

    if (parseBMP(name, &header, &data) != 0)
        goto fail;

    toReturn = new RGB24Buffer(header.h, header.w);

    for (unsigned i = 0; i < header.h; i++)
    {
        uint8_t  *src =  &(data[header.lineLength * (header.h - i - 1)]);
        RGBColor *dest = &toReturn->element(i,0);
        for (unsigned j = 0; j < header.w; j++)
        {
            uint32_t b = src[0];
            uint32_t g = src[1];
            uint32_t r = src[2];
            *dest = RGBColor(r,g,b);
            src+=3;
            dest++;
        }
    }
fail:
    if (data)
        delete[] data;
    return toReturn;
}


BMPLoader::~BMPLoader()
{
    // TODO Auto-generated destructor stub
}


bool BMPLoader::save(string name, G12Buffer *buffer)
{
    RGB24Buffer *toSave = new RGB24Buffer(buffer);
    bool result = save(name, toSave);
    delete toSave;
    return result;
}

bool BMPLoader::save(string name, G8Buffer *buffer)
{
    RGB24Buffer *toSave = new RGB24Buffer(buffer->getSize());
    toSave->drawG8Buffer(buffer);
    bool result = save(name, toSave);
    delete toSave;
    return result;
}

/**
 * TODO: Add error handling
 *
 * */
bool BMPLoader::save(string name, RGB24Buffer *buffer)
{
    ASSERT_TRUE(buffer != NULL, "Null buffer could not be saved");
    FILE *fp = fopen(name.c_str(), "wb");
    if (fp == NULL)
        return false;

    int w = buffer->w;
    int h = buffer->h;

    /*Line length in BMP is always 32bit aligned*/
    int lineLength = w * 3;
    lineLength = (lineLength % 4) ? (lineLength | 0x3) + 1 : lineLength;
    int fileSize = BMPHeader::HEADER_SIZE + h * lineLength;

    uint8_t *data = new uint8_t[fileSize];
    if (!data)
    {
        fclose(fp);
        return false;
    }

    memset(data, 0, BMPHeader::HEADER_SIZE /*fileSize*/);

    data[0x0] = BMPHeader::HEADER_SIGNATURE[0];
    data[0x1] = BMPHeader::HEADER_SIGNATURE[1];
    *((uint32_t *)(data + 0x2)) =  h * lineLength + BMPHeader::HEADER_SIZE;
    data[0x6] = 0;
    data[0x7] = 0;
    *((uint32_t *)(data + 0xA))  = BMPHeader::HEADER_SIZE;
    *((uint32_t *)(data + 0xE))  = 0x28;
    *((uint32_t *)(data + 0x12)) = w;
    *((uint32_t *)(data + 0x16)) = h;
    *((uint16_t *)(data + 0x1A)) = 1;
    *((uint16_t *)(data + 0x1C)) = 24;
    *((uint32_t *)(data + 0x1E)) = 0;
    *((uint32_t *)(data + 0x22)) = lineLength * h;
    *((uint32_t *)(data + 0x26)) = 0x0000B013;
    *((uint32_t *)(data + 0x2A)) = 0x0000B013;
    *((uint32_t *)(data + 0x2E)) = 0x0;
    *((uint32_t *)(data + 0x32)) = 0x0;

    for (int i = 0; i < h; i++)
    {
        uint8_t *offset = data + BMPHeader::HEADER_SIZE + (lineLength * (h - 1 - i));   // lines are flipped vertically there
        int j;
        for (j = 0; j < w; j++)
        {
            RGBColor pixel = buffer->element(i, j);
            *offset++ = pixel.b();
            *offset++ = pixel.g();
            *offset++ = pixel.r();
        }
        // Put in predictable padding values
        for (int k = j * 3; k < lineLength; k++)
        {
            *offset++ = 0;
        }
    }

    fwrite(data, sizeof(uint8_t), fileSize, fp);
    fclose(fp);

    delete[] data;
    return true;
}

} //namespace corecvs
