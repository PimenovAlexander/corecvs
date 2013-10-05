/**
 * \file rawLoader.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */
#include <string>

#include "rawLoader.h"

namespace corecvs {

string RAWLoader::prefix1(".raw");

bool RAWLoader::acceptsFile(string name)
{
    return (name.compare(name.length() - prefix1.length(), prefix1.length(), prefix1) == 0);
}

RAWLoader::RawFileType RAWLoader::typeByParams(int bits, char type)
{
    if (bits == 8)
        return TYPE_8_BIT;
    if (bits == 12)
    {
        if (type == 'l')
            return TYPE_12_BIT_LOW;
        if (type == 'h')
            return TYPE_12_BIT_HIGH;
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

    return TYPE_INVALID;
}

uint16_t RAWLoader::transform(RAWLoader::RawFileType type, uint8_t byte1, uint8_t byte2)
{
    switch (type)
    {
        case RAWLoader::TYPE_12_BIT_HIGH :
            return ((uint16_t)byte1 << 4) | ((uint16_t)byte2 >> 4);
        case RAWLoader::TYPE_12_BIT_LOW :
            return ((uint16_t)byte1 >> 4) | ((uint16_t)byte2 << 4);
        case RAWLoader::TYPE_16_BIT_HIGH :
            return ((uint16_t)byte1 << 8) | ((uint16_t)byte2);
        case RAWLoader::TYPE_16_BIT_LOW :
            return ((uint16_t)byte1)      | ((uint16_t)byte2 << 8);
        case RAWLoader::TYPE_16_BIT_HIGH_ONLY :
            return ((uint16_t)byte1 << 4);
        case RAWLoader::TYPE_16_BIT_LOW_ONLY :
            return ((uint16_t)byte2 << 4);
        default:
            ASSERT_TRUE(false, "Internal Error");
    }
    return 0;
}

G12Buffer* RAWLoader::load(string name)
{
    FILE* fp = NULL;
    G12Buffer *toReturn = NULL;
    uint8_t *data = NULL;
    long counter = 0;

    int h;
    int w;
    int bits;
    int bytes = 1;
    char typeChar;
    RawFileType type;

    string filename = name.substr(name.rfind('/') + 1);
    printf("Getting metainfo from filename %s\n", filename.c_str());
    if (sscanf(filename.c_str(), "%dx%d_%d%c_", &w, &h, &bits, &typeChar ) != 4)
    {
        h = defaultH;
        w = defaultW;
        type = defaultType;
    } else {
        type = typeByParams(bits, typeChar);
    }

    if (type == TYPE_INVALID)
    {
        printf("Wrong raw name pattern: %s\n", name.c_str());
        goto cleanup;
    }

    bytes = (type == TYPE_8_BIT) ? 1 : 2;

    //Open file for Reading in Binary Mode
    fp = fopen(name.c_str(), "rb");

    if (fp == NULL) {
        printf("Image %s does not exist \n", name.c_str());
        goto cleanup;
    }

    fseek(fp, 0, SEEK_END);
    counter = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (counter != h * w * bytes)
    {
        printf("Image file %s does not contain a valid ST image \n", name.c_str());
        goto cleanup;
    }

    data = new uint8_t[h * w * bytes];
    if (data == NULL)
    {
        printf("Could not allocate memory");
        goto cleanup;
    }
    if (fread(data, bytes, h * w, fp) == 0)
        throw "fread() call failed";

    toReturn = new G12Buffer(h, w, false);

    if (bytes == 1)
    {
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
                toReturn->element(i,j) = data[i * w + j];
    } else {
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
            {
                uint16_t byte1 = data[(i * w + j) * 2];
                uint16_t byte2 = data[(i * w + j) * 2 + 1];

                toReturn->element(i,j) = transform(type, (uint8_t)byte1, (uint8_t)byte2);
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

