#include "tgaLoader.h"
#include "utils/utils.h"

namespace corecvs {


string  TGALoaderBase::prefix1(".tga");



static void doTrace(bool trace, cchar *format, ...)
{
    if (trace) {
        va_list args;
        va_start (args, format);
        vprintf(format, args);
        fflush(stdout);
        va_end (args);
    }
}


int TGAHeader::parseHeader(FILE *fp)
{
    int status = -1;
    size_t result = 0;
    uint8_t *rawheader = new uint8_t[TGAHeader::HEADER_SIZE];
    result = fread(rawheader, sizeof(uint8_t), TGAHeader::HEADER_SIZE, fp);

    idLength        = *((uint8_t  *)(rawheader + 0x00));
    colourMapType   = *((uint8_t  *)(rawheader + 0x01));
    dataTypeCode    = *((uint8_t  *)(rawheader + 0x02));
    colourMapOrigin = *((uint16_t *)(rawheader + 0x03));
    colourMapLength = *((uint16_t *)(rawheader + 0x05));
    colourMapDepth  = *((uint8_t  *)(rawheader + 0x07));
    xOrigin         = *((uint16_t *)(rawheader + 0x08));
    yOrigin         = *((uint16_t *)(rawheader + 0x0A));
    width           = *((uint16_t *)(rawheader + 0x0C));
    height          = *((uint16_t *)(rawheader + 0x0E));
    bitsPerPixel    = *((uint8_t  *)(rawheader + 0x10));
    imageDescriptor = *((uint8_t  *)(rawheader + 0x11));

    doTrace(trace, "TGA of size [%dx%d] %d bpp\n", width, height, bitsPerPixel);
    if (trace) {
        print();
    }

    if (result != TGAHeader::HEADER_SIZE)
    {
        DOTRACE(("Could not read file header of size %d. Is end of file : %s",
                TGAHeader::HEADER_SIZE, feof(fp) ? "true" : "false"));
        goto fail;
    }

    if (dataTypeCode != TYPE_UNCOMPRESSED_RGB)
    {
        doTrace(trace, "Sorry, only TGA's with rgb format is suuported (%s)\n", getName((DataType)dataTypeCode));
        goto fail;
    }

    if (bitsPerPixel != 24 && bitsPerPixel != 32)
    {
        doTrace(trace, "Sorry, only TGA's with 24 or 32 bit depth are supported (%d)\n", bitsPerPixel);
        goto fail;
    }


    status = 0;
fail:
    deletearr_safe(rawheader);
    return status;
}


int TGALoaderBase::parseTGA (string& name, TGAHeader *header, uint8_t **dataPtr)
{
    int status = -1;
    FILE *fp = NULL;
    size_t result = 0;
    uint8_t *data = NULL;
    size_t dataSize = 0;


    fp = fopen(name.c_str(), "rb");
    if (fp == NULL)
       goto fail;

    if (header->parseHeader(fp) != 0)
       goto fail;

    dataSize = (header->bitsPerPixel / 8) * header->width * header->height;

    data = new uint8_t[dataSize];
    if (data == NULL)
       goto fail;

    result = fread(data, sizeof(uint8_t), dataSize, fp);
    if (result != dataSize)
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


TGALoaderBase::TGALoaderBase()
{

}

TGALoaderBase::~TGALoaderBase()
{

}

bool TGALoaderBase::acceptsFile(std::string name)
{
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    return HelperUtils::endsWith(name, prefix1);
}

#if 0
G12Buffer *TGALoaderBase::loadG12(std::string name)
{

}
#endif

RGB24Buffer *TGALoaderBase::loadRGB(std::string name)
{
    uint8_t *data = NULL;
    TGAHeader header;
    header.trace = true;

    RGB24Buffer *toReturn = NULL;

    if (parseTGA(name, &header, &data) != 0)
        goto fail;

    toReturn = new RGB24Buffer(header.height, header.width);

    for (unsigned i = 0; i < header.height; i++)
    {
        uint8_t  *src =  &(data[header.width * (header.bitsPerPixel / 8) * (header.height - 1 - i)]);
        RGBColor *dest = &toReturn->element(i,0);
        for (unsigned j = 0; j < header.width; j++)
        {
            RGBColor color;
            if (header.bitsPerPixel == 24) /*or we can just adjust one pixel at offset*/
            {
                uint32_t b = src[0];
                uint32_t g = src[1];
                uint32_t r = src[2];
                color = RGBColor(r,g,b);
            } else {
                uint32_t b = src[0];
                uint32_t g = src[1];
                uint32_t r = src[2];
                uint32_t a = src[3];
                color = RGBColor(r,g,b,a);
            }
            *dest = color;
            src += (header.bitsPerPixel / 8);
            dest++;
        }
    }
fail:
    if (data)
        delete[] data;
    return toReturn;
}

void TGAHeader::print() {
    cout << "idLength        :" << (int)idLength << endl;
    cout << "colourMapType   :" << (int)colourMapType << endl;
    cout << "dataTypeCode    :" << getName((DataType)dataTypeCode) << endl;
    cout << "colourMapOrigin :" << (int)colourMapOrigin << endl;
    cout << "colourMapLength :" << (int)colourMapLength << endl;
    cout << "colourMapDepth  :" << (int)colourMapDepth << endl;
    cout << "xOrigin         :" << (int)xOrigin << endl;
    cout << "yOrigin         :" << (int)yOrigin << endl;
    cout << "width           :" << (int)width << endl;
    cout << "height          :" << (int)height << endl;
    cout << "bitsPerPixel    :" << (int)bitsPerPixel << endl;
    cout << "imageDescriptor :" << (int)imageDescriptor << endl;
}

} // namespace corecvs



