/**
 * \file rawLoader.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */
#include <string>

#include "core/utils/utils.h"
#include "core/fileformats/rawLoader.h"

namespace corecvs {

string RAWLoaderBase::prefix1(".raw");

bool RAWLoaderBase::acceptsFile(string name)
{
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

    std::vector<string> metaVector = HelperUtils::stringSplit(name, ',');
    if (metaVector.empty()) {
        return false;
    }
    return HelperUtils::endsWith(metaVector.front(), prefix1);
}

RAWLoaderBase::RawFileType RAWLoaderBase::typeByParams(int bits, char type)
{
    if (bits == 8)
        return TYPE_8_BIT;
    if (bits == 12)
    {
        if (type == 'l')
            return TYPE_12_BIT_LOW;
        if (type == 'h')
            return TYPE_12_BIT_HIGH;
        if (type == 's')
            return TYPE_12_BIT_SBS;
    }

    if (bits == 16)
    {
        if (type == 'l')
            return TYPE_16_BIT_LOW;
        if (type == 'h')
            return TYPE_16_BIT_HIGH;
        if (type == 'b')
            return TYPE_16_BIT_LOW_ONLY;
        if (type == 't')
            return TYPE_16_BIT_HIGH_ONLY;
    }

    if (bits == 24) {
        return TYPE_24_BIT_RGB;
    }

    return TYPE_INVALID;
}

int RAWLoaderBase::typeToBytesNum(RAWLoaderBase::RawFileType type) {
    switch (type)
    {
        case RAWLoaderBase::TYPE_12_BIT_HIGH :
        case RAWLoaderBase::TYPE_12_BIT_LOW :
        case RAWLoaderBase::TYPE_16_BIT_HIGH :
        case RAWLoaderBase::TYPE_16_BIT_LOW :
        case RAWLoaderBase::TYPE_16_BIT_HIGH_ONLY :
        case RAWLoaderBase::TYPE_16_BIT_LOW_ONLY :
            return 2;
        case RAWLoaderBase::TYPE_8_BIT:
            return 1;
        case RAWLoaderBase::TYPE_24_BIT_RGB:
        case RAWLoaderBase::TYPE_12_BIT_SBS:
            return 3;

    }
    return 0;
}

int RAWLoaderBase::typeToBytesDen(RAWLoaderBase::RawFileType type) {
    switch (type)
    {
        case RAWLoaderBase::TYPE_12_BIT_HIGH :
        case RAWLoaderBase::TYPE_12_BIT_LOW :
        case RAWLoaderBase::TYPE_16_BIT_HIGH :
        case RAWLoaderBase::TYPE_16_BIT_LOW :
        case RAWLoaderBase::TYPE_16_BIT_HIGH_ONLY :
        case RAWLoaderBase::TYPE_16_BIT_LOW_ONLY :
        case RAWLoaderBase::TYPE_24_BIT_RGB:
            return 1;
        case RAWLoaderBase::TYPE_12_BIT_SBS:
            return 2;

    }
    return 0;
}


uint16_t RAWLoaderBase::transform(RAWLoaderBase::RawFileType type, uint8_t byte1, uint8_t byte2)
{
    switch (type)
    {
        case RAWLoaderBase::TYPE_12_BIT_HIGH :
            return ((uint16_t)byte1 << 4) | ((uint16_t)byte2 >> 4);
        case RAWLoaderBase::TYPE_12_BIT_LOW :
            return ((uint16_t)byte1 >> 4) | ((uint16_t)byte2 << 4);
        case RAWLoaderBase::TYPE_16_BIT_HIGH :
            return ((uint16_t)byte1 << 8) | ((uint16_t)byte2);
        case RAWLoaderBase::TYPE_16_BIT_LOW :
            return ((uint16_t)byte1)      | ((uint16_t)byte2 << 8);
        case RAWLoaderBase::TYPE_16_BIT_HIGH_ONLY :
            return ((uint16_t)byte1 << 4);
        case RAWLoaderBase::TYPE_16_BIT_LOW_ONLY :
            return ((uint16_t)byte2 << 4);
        default:
            CORE_ASSERT_TRUE(false, "Internal Error");
    }
    return 0;
}

G12Buffer* RAWLoaderBase::loadG12(string name)
{
    FILE* fp = NULL;
    G12Buffer *toReturn = NULL;
    uint8_t *data = NULL;
    long counter = 0;

    int h;
    int w;
    int bits;
    int bytesnum = 1;
    int bytesden = 1;

    char typeChar;
    RawFileType type;

    std::string meta;
    std::string filename = name.substr(name.rfind('/') + 1);
    std::string cleanName = name;

    std::vector<string> metaVector = HelperUtils::stringSplit(name, ',');
    if (metaVector.size() > 1) {
        meta = metaVector.back();
        cleanName = metaVector.front();
    }
    
    if (meta.empty()) {
        meta = filename;
    }


    SYNC_PRINT(("RAWLoaderBase::loadG12(): Getting metainfo from meta %s\n", meta.c_str()));
    if (sscanf(meta.c_str(), "%dx%d_%d%c_", &w, &h, &bits, &typeChar ) != 4)
    {
        h = defaultH;
        w = defaultW;
        type = defaultType;
    } else {
        SYNC_PRINT(("RAWLoaderBase::loadG12(): Parsed [h=%d, w=%d bpp=%d type=%c]\n", h, w, bits, typeChar));
        type = typeByParams(bits, typeChar);
    }

    if (type == TYPE_INVALID)
    {
        SYNC_PRINT(("RAWLoaderBase::loadG12(): Wrong raw name pattern: %s\n", name.c_str()));
        goto cleanup;
    }

    bytesnum = typeToBytesNum(type);
    bytesden = typeToBytesDen(type);
    SYNC_PRINT(("RAWLoaderBase::loadG12(): Parsed [each %d bytes contains %d pixels]\n", bytesnum, bytesden));


    //Open file for Reading in Binary Mode
    fp = fopen(cleanName.c_str(), "rb");

    if (fp == NULL) {
        SYNC_PRINT(("RAWLoaderBase::loadG12(): Image %s does not exist \n", name.c_str()));
        goto cleanup;
    }

    fseek(fp, 0, SEEK_END);
    counter = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (counter * bytesden != h * w * bytesnum)
    {
        SYNC_PRINT(("RAWLoaderBase::loadG12(): Image file %s does not contain a valid ST image realsize=%d expected=%d\n",
                    name.c_str(), h * w * bytesnum, counter * bytesden));
        goto cleanup;
    }

    data = new uint8_t[h * w * bytesnum];
    if (data == NULL)
    {
        SYNC_PRINT(("RAWLoaderBase::loadG12(): Could not allocate memory"));
        goto cleanup;
    }
    if (fread(data, bytesnum, h * w, fp) == 0)
        throw "RAWLoaderBase::loadG12(): fread() call failed";

    toReturn = new G12Buffer(h, w, false);

    if (type == TYPE_12_BIT_SBS)
    {
        uint8_t *offset = data;
        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j += 2)
            {
                uint16_t byte1 = *offset; offset++;
                uint16_t byte2 = *offset; offset++;
                uint16_t byte3 = *offset; offset++;

                uint16_t g12v1 = ((byte1       ) << 4) | ((byte2 & 0xF0) >> 4);
                uint16_t g12v2 = ((byte3       ) << 4) | ((byte2 & 0x0F)     );

                toReturn->element(i, j    ) = g12v1;
                toReturn->element(i, j + 1) = g12v2;

            }
        }

    } else if (bytesnum == 1)
    {
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
                toReturn->element(i,j) = data[i * w + j] << 4;
    } else if (bytesnum == 2) {
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
            {
                uint16_t byte1 = data[(i * w + j) * 2];
                uint16_t byte2 = data[(i * w + j) * 2 + 1];

                toReturn->element(i,j) = transform(type, (uint8_t)byte1, (uint8_t)byte2);
            }
    } else {
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
            {
                uint16_t r = data[(i * w + j) * 3];
                uint16_t g = data[(i * w + j) * 3 + 1];
                uint16_t b = data[(i * w + j) * 3 + 2];
                toReturn->element(i,j) = ((r + g + b) / 3) << 4;
            }
    }

cleanup:
    if (fp != NULL)
        fclose(fp);
    if (data != NULL)
        delete[] data;
    return toReturn;
}

RGB24Buffer *RAWLoaderBase::loadRGB24(std::string name)
{
    FILE* fp = NULL;
    RGB24Buffer *toReturn = NULL;
    uint8_t *data = NULL;
    long counter = 0;

    int h;
    int w;
    int bits;
    int bytesnum = 1;
    int bytesden = 1;

    char typeChar;
    RawFileType type;

    std::string meta;
    std::string filename = name.substr(name.rfind('/') + 1);
    std::string cleanName = name;

    std::vector<string> metaVector = HelperUtils::stringSplit(name, ',');
    if (metaVector.size() > 1) {
        meta = metaVector.back();
        cleanName = metaVector.front();
    }

    if (meta.empty()) {
        meta = filename;
    }


    SYNC_PRINT(("RAWLoaderBase::loadRGB24(): Getting metainfo from meta %s\n", meta.c_str()));
    if (sscanf(meta.c_str(), "%dx%d_%d%c_", &w, &h, &bits, &typeChar ) != 4)
    {
        h = defaultH;
        w = defaultW;
        type = defaultType;
    } else {
        SYNC_PRINT(("RAWLoaderBase::loadRGB24(): Parsed [h=%d, w=%d bpp=%d type=%c]\n", h, w, bits, typeChar));
        type = typeByParams(bits, typeChar);
    }

    if (type == TYPE_INVALID)
    {
        SYNC_PRINT(("RAWLoaderBase::loadRGB24(): Wrong raw name pattern: %s\n", name.c_str()));
        goto cleanup;
    }

    bytesnum = typeToBytesNum(type);
    bytesden = typeToBytesDen(type);

    //Open file for Reading in Binary Mode
    fp = fopen(cleanName.c_str(), "rb");

    if (fp == NULL) {
        SYNC_PRINT(("RAWLoaderBase::loadRGB24(): Image %s does not exist \n", name.c_str()));
        goto cleanup;
    }

    fseek(fp, 0, SEEK_END);
    counter = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (counter * bytesden != h * w * bytesnum)
    {
        SYNC_PRINT(("RAWLoaderBase::loadRGB24(): Image file %s does not contain a valid ST image realsize=%d expected=%d\n",
                    name.c_str(), h * w * bytesnum, counter * bytesden));
        goto cleanup;
    }

    data = new uint8_t[h * w * bytesnum];
    if (data == NULL)
    {
        SYNC_PRINT(("Could not allocate memory"));
        goto cleanup;
    }
    if (fread(data, bytesnum, h * w, fp) == 0)
        throw "fread() call failed";

    toReturn = new RGB24Buffer(h, w, false);

    if (type == TYPE_12_BIT_SBS)
    {
        uint8_t *offset = data;
        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j += 2)
            {
                uint16_t byte1 = *offset; offset++;
                uint16_t byte2 = *offset; offset++;
                uint16_t byte3 = *offset; offset++;

                uint16_t g12v1 = ((byte1       ) << 4) | ((byte2 & 0xF0) >> 4);
                uint16_t g12v2 = ((byte3       ) << 4) | ((byte2 & 0x0F)     );


                toReturn->element(i, j    ) = RGBColor::Gray(g12v1 >> 4);
                toReturn->element(i, j + 1) = RGBColor::Gray(g12v2 >> 4);

            }
        }

    } else if (bytesnum == 1)
    {
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
                toReturn->element(i,j) = RGBColor::Gray(data[i * w + j]);
    } else if (bytesnum == 2) {
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
            {
                uint16_t byte1 = data[(i * w + j) * 2];
                uint16_t byte2 = data[(i * w + j) * 2 + 1];

                toReturn->element(i,j) = RGBColor::Gray(transform(type, (uint8_t)byte1, (uint8_t)byte2) >> 4);
            }
    } else {
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
            {
                uint16_t r = data[(i * w + j) * 3];
                uint16_t g = data[(i * w + j) * 3 + 1];
                uint16_t b = data[(i * w + j) * 3 + 2];
                toReturn->element(i,j) = RGBColor(r, g, b);
            }
    }

cleanup:
    if (fp != NULL)
        fclose(fp);
    if (data != NULL)
        delete[] data;
    return toReturn;
}

} //namespace corecvs

