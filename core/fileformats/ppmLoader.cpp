/**
 * \file ppmLoader.cpp
 * \brief Add Comment Here
 *
 *  TODO : Remove doubling code in PPM and PPM6 loader ASAP
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */
#include <string>

#include "global.h"

#include "ppmLoader.h"

string PPMLoader::prefix1(".pgm");
string PPMLoader::prefix2(".ppm");

bool PPMLoader::acceptsFile(string name)
{
    return (
        name.compare(name.length() - prefix1.length(), prefix1.length(), prefix1) == 0 ||
        name.compare(name.length() - prefix2.length(), prefix2.length(), prefix2) == 0
        );
}

G12Buffer* PPMLoader::load(string name)
{
    return loadMeta(name, nullptr);
}

G12Buffer* PPMLoader::loadMeta(const string& name, MetaData *metadata)
{
    if (metadata != nullptr) {
        DOTRACE(("Will load the file %s as PPM with metadata\n ", name.c_str()));
    }
    else {
        DOTRACE(("Will load the file %s as PPM (ignoring any available metadata)\n ", name.c_str()));
    }

    G12Buffer *toReturn = g12BufferCreateFromPGM(name, metadata);
    return toReturn;
}

RGB48Buffer* PPMLoader::loadRGB(const string& name, MetaData *metadata)
{
    RGB48Buffer *toReturn = rgb48BufferCreateFromPPM(name, metadata);
    return toReturn;
}

G12Buffer* PPMLoader::g12BufferCreateFromPGM(const string& name, MetaData *meta)
{
    // open file for reading in binary mode
    FILE *fp = fopen(name.c_str(), "rb");
    if (fp == nullptr)
        return nullptr;

    // PPM headers variable declaration
    unsigned long h, w;
    uint8_t type;
    uint16_t maxval;
    if (!readHeader(fp, &h, &w, &maxval, &type, meta) || (type != 5 && type != 6))
    {
        fclose(fp);
        return nullptr;
    }

    bool calcWhite = false;
    int white = 0;
    // if no metadata is present, create some
    // if metadata is null, don't
    if (meta != nullptr)
    {
        // create an alias to metadata
        MetaData &metadata = *meta;
        // get significant bit count
        if (metadata["bits"].empty())
        {
            metadata["bits"].push_back(1);
        }
        while (maxval >> int(metadata["bits"][0]))
        {
            metadata["bits"][0]++;
        }
        if (metadata["white"].empty())
            calcWhite = true;
    }

    G12Buffer *result = new G12Buffer(h, w, false);

    // image size in bytes
    uint64_t size = (maxval < 0x100 ? 1 : 2) * w * h;

    // for reading we don't need to account for possible system byte orders, so just use a 8-bits buffer
    uint8_t *charImage = new uint8_t[size];

    if (fread(charImage, 1, size, fp) == 0)
    {
        SYNC_PRINT(("fread() call failed on %s", name.c_str()));
        goto done;
    }

    if (maxval < 0x100)             // 1-byte case
    {
        for (unsigned k = 0, i = 0; i < h; ++i)
            for (unsigned j = 0; j < w; ++j, ++k)
            {
                uint8_t pix = result->element(i, j) = charImage[k];
                if (calcWhite && pix > white)
                    white = pix;
            }
    }
    else                            // 2-bytes case
    {
        // here we need to calculate shift to compress data into a 12-bits buffer
        int shiftCount;
        for (shiftCount = 0; (maxval >> shiftCount) > G12Buffer::BUFFER_MAX_VALUE; shiftCount++);

        for (unsigned k = 0, i = 0; i < h; ++i)
        {
            for (unsigned j = 0; j < w; ++j, ++k)
            {
                uint16_t pix = result->element(i, j) = ((charImage[2 * k + 0]) << 8 | (charImage[2 * k + 1])) >> shiftCount;

                CORE_ASSERT_FALSE((pix >= (1 << G12Buffer::BUFFER_BITS)), "Internal error in image loader\n");
                if (calcWhite && pix > white)
                    white = pix;
            }
        }
    }

    if (calcWhite) {
        meta->at("white").push_back(white);
    }
    //SYNC_PRINT(("g12BufferCreateFromPGM() bits:%d white:%d\n", (int)meta->at("bits")[0], (int)meta->at("white")[0]));

done:
    fclose(fp);
    deletearr_safe(charImage);
    return result;
}

RGB48Buffer* PPMLoader::rgb48BufferCreateFromPPM(const string& name, MetaData *meta)
{
    // PPM headers variable declaration
    unsigned long int i, j;
    unsigned long int h, w;
    uint8_t type;
    unsigned short int maxval;
    int8_t c;

    // open file for reading in binary mode
    FILE *fp = fopen(name.c_str(), "rb");
    if (fp == nullptr)
    {
        SYNC_PRINT(("PPMLoader::rgb48BufferCreateFromPPM(): can't open file <%s>\n", name.c_str()));
        return nullptr;
    }

    if (!readHeader(fp, &h, &w, &maxval, &type, meta) || (type != 6))
    {
        SYNC_PRINT(("PPMLoader::rgb48BufferCreateFromPPM(): can't read header <%s>\n", name.c_str()));
        fclose(fp);
        return nullptr;
    }

    bool calcWhite = false;
    int white = 0;
    // if no metadata is present, create some
    // if metadata is null, don't
    if (meta != nullptr)
    {
        // create an alias to metadata
        MetaData &metadata = *meta;
        // get significant bit count
        if (metadata["bits"].empty())
        {
            metadata["bits"].push_back(1);
        }
        while (maxval >> int(metadata["bits"][0]))
        {
            metadata["bits"][0]++;
        }
        if (metadata["white"].empty())
            metadata["white"].push_back(maxval);
    }

    RGB48Buffer *result = new RGB48Buffer(h, w, false);

    // image size in bytes
    uint64_t size = (maxval < 0x100 ? 1 : 2) * w * h * 3;

    // for reading we don't need to account for possible system byte orders, so just use a 8bit buffer
    uint8_t *charImage = new uint8_t[size];
    if (charImage == nullptr)
    {
        CORE_ASSERT_FAIL_P(("out of memory on allocate %d bytes", (int)size));
        goto done;
    }

    if (fread(charImage, 1, size, fp) == 0)
    {
        CORE_ASSERT_FAIL("fread() call failed");
        goto done;
    }

    if (maxval <= 0xff)
    {
        // 1-byte case
        for (i = 0; i < h; i++)
            for (j = 0; j < w; j++)
            {
                for (c = 0; c < 3; c++)
                {
                    result->element(i, j)[2 - c] = (charImage[i * w * 3 + j * 3 + c]);
                }
            }
    }
    else
    {
        // 2-byte case
        for (i = 0; i < h; i++)
        {
            for (j = 0; j < w * 2; j++)
            {
                for (c = 2; c >= 0; c--)
                {
                    int offset = (i * w + j) * 6;
                    result->element(i, j)[2 - c] = ((charImage[offset + 0]) << 8 |
                                                    (charImage[offset + 1]));
                }
            }
        }

    }

    if (calcWhite)
    {
        if (meta->at("white").empty())
        {
            meta->at("white").push_back(white);
        }
        else
        {
            meta->at("white")[0] = white;
        }
    }

done:
    fclose(fp);
    deletearr_safe(charImage);
    return result;
}

int PPMLoader::nextLine(FILE *fp, char *buf, int sz, MetaData *metadata)
{
    // read 1-st character
    while (fread(buf, 1, 1, fp))
    {
        if (buf[0] != '#' && buf[0] != '\n' && buf[0] != '\r')
        {
            // not a comment/newline, rewind and read whole line
            fseek(fp, -1, SEEK_CUR);
            if (sz > 0 && fgets(buf, sz, fp) == nullptr)
            {
                SYNC_PRINT(("fgets() call failed on %s:%d\n", __FILE__, __LINE__));
            }
            return 0;
        }
        else
        {
            // comment or newline, try to parse
            // if newline, clear buffer and return OK
            if (buf[0] == '\n' || buf[0] == '\r')
            {
                buf[0] = 0;
                return 0;
            }

            if (fgets(buf, sz, fp) == nullptr)
                return 1;

            // try to read metadata
            char param[256];
            int n = 0;
            // read param name
            if (metadata != nullptr && sscanf(buf, " @meta %s\t@values %d\t", param, &n) == 2)
            {
                char* numbers = strrchr(buf, '\t') + 1;
                MetaValue values;
                // read n param values
                for (int i = 0; i < n; i++)
                {
                    double v = 0;
                    if (numbers == nullptr) {
                        SYNC_PRINT(("Invalid metadata format on <%s> for param <%s> values:%d\n", buf, param, n));
                        break;
                    }
                    sscanf(numbers, "%lf", &v);
                    numbers = strchr(numbers, ' ') + 1;
                    values.push_back(v);
                }
                metadata->insert(std::pair<string, MetaValue>(param, values));
            }

            CORE_CLEAR_MEMORY(buf, sz);
        }
    }

    return 1;
}

bool PPMLoader::readHeader(FILE *fp, unsigned long int *h, unsigned long int *w, uint16_t *maxval, uint8_t *type, MetaData* metadata)
{
    char header[255];

    // check PPM type (currently only supports 5 or 6)
    if (fgets(header, sizeof(header), fp) == nullptr || (header[0] != 'P') || (header[1] < '5') || (header[1] > '6'))
    {
        SYNC_PRINT(("Image is not a supported PPM\n"));
        return false;
    }

    // type contains a TYPE uint8_t, in our case 5 or 6
    *type = header[1] - '0';

    // get dimensions
    if (nextLine(fp, header, sizeof(header), metadata) != 0)
        return false;

    // parse dimensions
    if (sscanf(header, "%lu%lu", w, h) != 2)
    {
        // try to parse dimensions in Photoshop-like format (when a newline is used instead of whitespace or tabulation)
        if (sscanf(header, "%lu", w) != 1)
        {
            SYNC_PRINT(("Image dimensions could not be read from line %s\n", header));
            return false;
        }
        else
        {
            // first dimension has been read, try to read the second
            int error = nextLine(fp, header, sizeof header, metadata);
            if (error || sscanf(&header[0], "%lu", h) != 1) // duplicate code can be gotten rid of with a goto (not sure it's worth doing)
            {
                SYNC_PRINT(("Image dimensions could not be read from line %s\n", header));
                return false;
            }
        }
    }

    // get colour depth (metric?)
    if (nextLine(fp, header, sizeof header, metadata) != 0)
        return false;

    if (sscanf(&header[0], "%hu", maxval) != 1)
    {
        SYNC_PRINT(("Image metric could not be read form line %s\n", header));
        return false;
    }

    // we assume that no comments exist after the colour depth header line to avoid misinterpretation of '#' first data value

    DOTRACE(("Image is P%d PPM [%lu %lu] max=%u\n", *type, *h, *w, *maxval));
    return true;
}

bool PPMLoader::writeHeader(FILE *fp, unsigned long int h, unsigned long int w, uint8_t type, uint16_t maxval, MetaData* meta)
{
    if (!fp || !h || !w || type < 5 || type > 6)
        return false;

    MetaData &metadata = *meta;

    fprintf(fp, "P%u\n", type);

    // TODO: test this!
    if (meta != NULL)
    {
        for (MetaData::iterator i = metadata.begin(); i != metadata.end(); i++)
        {
            fprintf(fp, "# @meta %s\t@values %i\t", i->first.c_str(), (int)i->second.size());
            for (uint j = 0; j < i->second.size(); j++)
                fprintf(fp, "%f ", i->second[j]);
            fprintf(fp, "\n");
        }
    }

    fprintf(fp, "%lu %lu\n", w, h);
    fprintf(fp, "%hu\n", maxval);
    return true;
}

int PPMLoader::save(const string& name, RGB24Buffer *buffer, MetaData* metadata)
{
    if (buffer == NULL)
        return -1;

    FILE *fp = fopen(name.c_str(), "wb");
    if (fp == NULL)
    {
        CORE_ASSERT_FAIL("Image could not be written");
        return -1;
    }

    int h = buffer->h;
    int w = buffer->w;

    writeHeader(fp, h, w, 6, 0xff, metadata);

    uint8_t *charImage = new uint8_t[3 * w * h];

    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
        {
            uint8_t elemval[3] = {
                buffer->element(i, j).r(),
                buffer->element(i, j).g(),
                buffer->element(i, j).b()
            };
            for (int offset = i * w + j, k = 0; k < 3; k++)
                charImage[offset * 3 + k] = elemval[k];
        }

    fwrite(charImage, 3, h * w, fp);

    deletearr_safe(charImage);
    fclose(fp);
    return 0;
}

int PPMLoader::save(const string& name, G12Buffer *buffer, MetaData* metadata, int forceTo8bitsShift)
{
    if (buffer == NULL)
        return -1;

    FILE *fp = fopen(name.c_str(), "wb");
    if (fp == NULL)
    {
        CORE_ASSERT_FAIL("Image could not be written");
        return -1;
    }

    int h = buffer->h;
    int w = buffer->w;
    int bytes = 2;
    int bits = 12;
    if (metadata != nullptr)
    {
        MetaData &meta = *metadata;
        if ((bits = meta["bits"][0]) == 0)
        {
            bits = 12;
            bytes = 2;
        }
        if (bits > 8)
            bytes = 2;
    }

    int  maxVal =  forceTo8bitsShift >= 0 ? 0xff : (1 << bits) - 1;
    size_t size = (forceTo8bitsShift >= 0 ? 1 : bytes) * w * h;

    writeHeader(fp, h, w, 5, maxVal, metadata);

    uint8_t *charImage = new uint8_t[size];
    if (bytes == 2)
    {
        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                int offset = i * w + j;
                uint16_t pixel = buffer->element(i, j);
                if (forceTo8bitsShift >= 0) {
                    uint16_t pix = pixel >> forceTo8bitsShift;
                    charImage[offset] = pix > 0xff ? 0xff : (pix & 0xff);
                }
                else {
                    charImage[offset * 2] = (pixel >> 8) & 0xFF;
                    charImage[offset * 2 + 1] = pixel & 0xFF;
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                charImage[i * w + j] = buffer->element(i, j) & 0xFF;
            }
        }
    }

    fwrite(charImage, 1, size, fp);
    fclose(fp);
    deletearr_safe(charImage);
    return 0;
}

int PPMLoader::save(const string& name, RGB48Buffer *buffer, MetaData* metadata, int forceTo8bitsShift)
{
    if (buffer == NULL)
        return -1;

    FILE *fp = fopen(name.c_str(), "wb");
    if (fp == NULL)
    {
        //CORE_ASSERT_FAIL("Image could not be written");
        return -1;
    }

    int h = buffer->h;
    int w = buffer->w;

    int maxval = 0;
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
        {
            auto &pixel = buffer->element(i, j);
            if (pixel.r() > maxval) maxval = pixel.r();
            if (pixel.g() > maxval) maxval = pixel.g();
            if (pixel.b() > maxval) maxval = pixel.b();
        }

    int bits = 1;
    while (maxval >> bits)
        bits++;

    int bytes = (bits + 7) / 8;
    uint16_t maxVal = (1 << bits) - 1;

    writeHeader(fp, h, w, 6, forceTo8bitsShift >= 0 ? 255 : maxVal, metadata);

    size_t size = 3 * (forceTo8bitsShift >= 0 ? 1 : bytes) * w * h;
    uint8_t *charImage = new uint8_t[size];

    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
        {
            uint16_t elemval[3] = {
                buffer->element(i, j).r(),
                buffer->element(i, j).g(),
                buffer->element(i, j).b()
            };
            int offset = 3 * (i * w + j);

            if (bytes == 2) {
                for (int k = 0; k < 3; k++)
                {
                    if (forceTo8bitsShift >= 0) {
                        uint16_t pix = elemval[k] >> forceTo8bitsShift;
                        charImage[offset + k] = pix > 0xff ? 0xff : (pix & 0xff);
                    }
                    else {
                        charImage[(offset + k) * 2] = (elemval[k] & 0xff00) >> 8;
                        charImage[(offset + k) * 2 + 1] = (elemval[k] & 0xff);
                    }
                }
            }
            else {
                for (int k = 0; k < 3; k++) {
                    charImage[offset + k] = elemval[k] & 0xff;
                }
            }
        }

    fwrite(charImage, 1, size, fp);
    fclose(fp);
    deletearr_safe(charImage);
    return 0;
}

// TODO: get rid of G16-specific methods
int PPMLoader::saveG16(const string& name, G12Buffer *buffer)
{
    if (buffer == NULL)
        return -1;

    int h = buffer->h;
    int w = buffer->w;

    FILE *fp = fopen(name.c_str(), "wb");
    if (fp == NULL) {
        return -1;
    }

    fprintf(fp, "P5\n");
    fprintf(fp, "############################################\n");
    fprintf(fp, "# The original source buffer had 16 bit.\n");
    fprintf(fp, "############################################\n");
    fprintf(fp, "%d %d\n", w, h);
    fprintf(fp, "%d\n", 0xFFFF);

    uint8_t *charImage = new uint8_t[2 * w * h];
    int i, j;
    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            int offset = i * w + j;
            charImage[offset * 2] = (buffer->element(i, j) >> 8) & 0xFF;
            charImage[offset * 2 + 1] = buffer->element(i, j) & 0xFF;
        }
    }
    fwrite(charImage, 2, h * w, fp);
    deletearr_safe(charImage);
    fclose(fp);
    return 0;
}

/* Temporary 16bit load */

G12Buffer* PPMLoader::g16BufferCreateFromPPM(const string& name, MetaData* metadata)
{
    //Open file for Reading in Binary Mode
    FILE *fp = fopen(name.c_str(), "rb");
    if (fp == NULL)
    {
        SYNC_PRINT(("Image %s does not exist\n", name.c_str()));
        return NULL;
    }

    unsigned long h, w;
    uint8_t type;
    uint16_t maxval;
    if (!readHeader(fp, &h, &w, &maxval, &type, metadata)) {
        return NULL;
    }

    G12Buffer *result = new G12Buffer(h, w, false);

    // image size in bytes
    uint64_t size = (maxval < 0x100 ? 1 : 2) * w * h;

    // for reading we don't need to account for possible system byte orders, so just use a 8-bits buffer
    uint8_t *charImage = new uint8_t[size];

    if (fread(charImage, 1, size, fp) == 0)
    {
        SYNC_PRINT(("fread() call failed on %s", name.c_str()));
        goto done;
    }

    if (maxval < 0x100)
    {
        for (unsigned k = 0, i = 0; i < h; ++i)
            for (unsigned j = 0; j < w; ++j)
                result->element(i, j) = (charImage[k++]) << 8;
    }
    else
    {
        int shiftCount = 0;
        for (shiftCount = 0; (maxval >> shiftCount) > (1 << 16); shiftCount++);

        for (unsigned k = 0, i = 0; i < h; ++i)
        {
            for (unsigned j = 0; j < w; ++j, ++k)
            {
                result->element(i, j) = ((charImage[k * 2]) << 8 | (charImage[k * 2 + 1])) >> shiftCount;

                CORE_ASSERT_FALSE((result->element(i, j) > G12Buffer::BUFFER_MAX_VALUE), "Internal error in image loader\n");
            }
        }
    }

done:
    fclose(fp);
    deletearr_safe(charImage);
    return result;
}
