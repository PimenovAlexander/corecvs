#ifndef TGALOADER_H
#define TGALOADER_H
/**
 * \file tgaLoader.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */
#include <string>
#include <stdint.h>

#include "core/utils/global.h"

#include "core/fileformats/bufferLoader.h"
#include "core/buffers/g12Buffer.h"
#include "core/buffers/rgb24/rgb24Buffer.h"

namespace corecvs {


class TGAHeaderData {
public:
    uint8_t  idLength;
    uint8_t  colourMapType;
    uint8_t  dataTypeCode;
    uint16_t colourMapOrigin;
    uint16_t colourMapLength;
    uint8_t  colourMapDepth;
    uint16_t xOrigin;
    uint16_t yOrigin;
    uint16_t width;
    uint16_t height;
    uint8_t  bitsPerPixel;
    uint8_t  imageDescriptor;
};

class TGAHeader : public TGAHeaderData {
public:
    bool trace = true;

public:

    enum DataType {
          TYPE_NO_IMAGE               =  0, // -  No image data included.
          TYPE_UNCOMPRESSED_COLOR_MAP =  1, //  -  Uncompressed, color-mapped images.
          TYPE_UNCOMPRESSED_RGB       =  2, // -  Uncompressed, RGB images.
          TYPE_UNCOMPRESSED_BW        =  3, //-  Uncompressed, black and white images.
          TYPE_RLE_COLOR_MAP          =  9, //-  Runlength encoded color-mapped images.
          TYPE_RLE_RGB                = 10, //-  Runlength encoded RGB images.
          TYPE_COMPRESSED_BW          = 11, //-  Compressed, black and white images.
          TYPE_COMPRESSED_COLOR_MAP_1 = 32, //-  Compressed color-mapped data, using Huffman, Delta, and runlength encoding.
          TYPE_COMPRESSED_COLOR_MAP_2 = 33 // -  Compressed color-mapped data, using Huffman, Delta, and runlength encoding.  4-pass quadtree-type process.
    };

    static inline const char *getName(const DataType &value)
    {
        switch (value)
        {
         case TYPE_NO_IMAGE               : return "TYPE_NO_IMAGE"; break ;
         case TYPE_UNCOMPRESSED_COLOR_MAP : return "TYPE_UNCOMPRESSED_COLOR_MAP"; break ;
         case TYPE_UNCOMPRESSED_RGB       : return "TYPE_UNCOMPRESSED_RGB"; break ;
         case TYPE_UNCOMPRESSED_BW        : return "TYPE_UNCOMPRESSED_BW"; break ;
         case TYPE_RLE_COLOR_MAP          : return "TYPE_RLE_COLOR_MAP"; break ;
         case TYPE_RLE_RGB                : return "TYPE_RLE_RGB"; break ;
         case TYPE_COMPRESSED_BW          : return "TYPE_COMPRESSED_BW"; break ;
         case TYPE_COMPRESSED_COLOR_MAP_1 : return "TYPE_COMPRESSED_COLOR_MAP_1"; break ;
         case TYPE_COMPRESSED_COLOR_MAP_2 : return "TYPE_COMPRESSED_COLOR_MAP_2"; break ;
         default : return "Not in range"; break ;

        }
        return "Not in range";
    }

    static const unsigned HEADER_SIZE = 18;
    int parseHeader(FILE *fp);
    void print();
};


class TGALoaderBase
{
protected:
    static string prefix1;

public:
    bool trace = true;

    TGALoaderBase();
    virtual ~TGALoaderBase();

    bool acceptsFile(string name);

//    virtual G12Buffer   * loadG12(string name);
    virtual RGB24Buffer * loadRGB(string name);

private:
    int parseTGA (string& name, TGAHeader *header, uint8_t **dataPtr);
};

class TGALoaderRGB24 : public BufferLoader<RGB24Buffer>, public TGALoaderBase
{
public:
    virtual bool acceptsFile(const string &name) override
    {
       return TGALoaderBase::acceptsFile(name);
    }

    virtual RGB24Buffer *load(const string &name) override
    {
        return TGALoaderBase::loadRGB(name);
    }

    virtual std::string name() override { return "TGALoaderRGB24"; }
    virtual std::vector<std::string> extentions() override {
        return std::vector<std::string>({ TGALoaderBase::prefix1 });
    }
};




} // namespace corecvs




#endif // TGALOADER_H
