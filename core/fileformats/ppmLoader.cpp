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

#include "core/utils/global.h"
#include "core/utils/utils.h"
#include "core/fileformats/ppmLoader.h"
#include "core/buffers/converters/debayer.h"
#include "core/utils/log.h"

namespace corecvs {

string PPMLoader::prefix1(".pgm");
string PPMLoader::prefix2(".ppm");

bool PPMLoader::acceptsFile(string name)
{
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

    return HelperUtils::endsWith(name, prefix1) ||
           HelperUtils::endsWith(name, prefix2);
}

G12Buffer* PPMLoader::loadG16(const string& name)
{
    if (HelperUtils::endsWith(name, prefix1))   // == pgm
    {
        return loadG12(name, nullptr, true/*loadAs16*/);
    }

    FILE *In = fopen(name.c_str(), "rb");
    if (In == NULL) {
        L_ERROR_P("couldn't open file: %s", name.c_str());
        return nullptr;
    }

    uint32_t h = 0, w = 0;
    if (fread(&h, sizeof(h), 1, In) != 1 ||
        fread(&w, sizeof(w), 1, In) != 1 ||
        h > 5000 || w > 5000)
    {
        L_ERROR_P("invalid image size %dx%d at file: %s", w, h, name.c_str());
        fclose(In);
        return nullptr;
    }

    G12Buffer *result = new G12Buffer(h, w, false);
    if (result)
    for (uint32_t i = 0; i < h; ++i) {
        for (uint32_t j = 0; j < w; ++j) {
            uint16_t d = 0;
            if (fread(&d, sizeof(d), 1, In) != 1) {
                j = w, i = h;
                break;
            }
            result->element(i, j) = d;
        }
    }
    fclose(In);
    return result;
}

G12Buffer* PPMLoader::loadG12(const string& name, MetaData *meta, bool loadAs16)
{
    L_DEBUG_P("name:%s meta:%s as16:%d", name.c_str(), (meta ? "fill" : "ignore"), loadAs16);

    FILE *fp = fopen(name.c_str(), "rb");
    if (fp == nullptr) {
        L_ERROR_P("couldn't open file: %s", name.c_str());
        return nullptr;
    }

    // PPM headers variable declaration
    unsigned long h, w;
    uint8_t type;
    uint16_t maxval;
    if (!readHeader(fp, &h, &w, &maxval, &type, meta) || (type != 5 && type != 6))
    {
        L_ERROR_P("invalid header at file: %s", name.c_str());
        fclose(fp);
        return nullptr;
    }
    if (type == 6)      // file has 3 channels instead of 1
    {
        L_ERROR_P("invalid type (%d) of the PPM file: %s", type, name.c_str());
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

    if (fread(charImage, 1, size, fp) != size)
    {
        L_ERROR_P("couldn't read %u bytes from file: %s", (unsigned)size, name.c_str());
        delete_safe(charImage);
        delete_safe(result);
        goto done;
    }

    if (maxval < 0x100)             // 1-byte case
    {
        cint shift = loadAs16 ? 8 : 0;
        for (unsigned k = 0, i = 0; i < h; ++i)
            for (unsigned j = 0; j < w; ++j)
            {
                uint8_t pix = result->element(i, j) = charImage[k++] << shift;
                if (calcWhite && pix > white)
                    white = pix;
            }
    }
    else                            // 2-bytes case
    {
        cint MaxValue = loadAs16 ? (1 << 16) - 1 : (G12Buffer::BUFFER_MAX_VALUE);
        // here we need to calculate shift to compress data into a 12-bits buffer
        int shiftCount;
        for (shiftCount = 0; (maxval >> shiftCount) > MaxValue; shiftCount++);

        for (unsigned k = 0, i = 0; i < h; ++i)
        {
            for (unsigned j = 0; j < w; ++j, ++k)
            {
                uint16_t pix = result->element(i, j) =
                    ((charImage[2 * k + 0]) << 8 | (charImage[2 * k + 1])) >> shiftCount;

                CORE_ASSERT_FALSE((pix > MaxValue), "Internal error in image loader\n");
                if (calcWhite && pix > white)
                    white = pix;
            }
        }
    }

    if (calcWhite) {
        meta->at("white").push_back(white);
    }

done:
    fclose(fp);
    deletearr_safe(charImage);
    return result;
}

RGB48Buffer* PPMLoader::loadRgb48(const string& name, MetaData *meta)
{
    // PPM headers variable declaration
    unsigned long int h, w;
    uint8_t type;
    unsigned short int maxval;

    FILE *fp = fopen(name.c_str(), "rb");
    if (fp == nullptr)
    {
        L_ERROR_P("can't open file <%s>", name.c_str());
        return nullptr;
    }

    if (!readHeader(fp, &h, &w, &maxval, &type, meta) || (type != 6))
    {
        L_ERROR_P("invalid header at <%s>", name.c_str());
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
        CORE_ASSERT_FAIL_P(("out of memory on allocate %" PRISIZE_T " bytes", size));
        goto done;
    }

    if (fread(charImage, 1, size, fp) != size)
    {
        L_ERROR_P("couldn't read file: %s", name.c_str());
        delete_safe(charImage);
        delete_safe(result);
        goto done;
    }

    if (maxval <= 0xff)
    {
        // 1-byte case
        for (uint k = 0, i = 0; i < h; ++i)
            for (uint j = 0; j < w; ++j, ++k)
            {
                auto& out = result->element(i, j);
                out[2] = (charImage[k * 3 + 0]);
                out[1] = (charImage[k * 3 + 1]);
                out[0] = (charImage[k * 3 + 2]);
            }
    }
    else
    {
        // 2-byte case
        for (uint k = 0, i = 0; i < h; ++i)
            for (uint j = 0; j < w; ++j, ++k)
            {
                auto& out = result->element(i, j);
                out[2] = (charImage[k * 6 + 0] << 8) | charImage[k * 6 + 1];
                out[1] = (charImage[k * 6 + 2] << 8) | charImage[k * 6 + 3];
                out[0] = (charImage[k * 6 + 4] << 8) | charImage[k * 6 + 5];
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
                L_ERROR_P("couldn't read file: error on fgets");
            }
            return 0;
        }

        // comment or newline, try to parse, if newline, clear buffer and return OK
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
                    L_ERROR_P("Invalid metadata format on <%s> for param <%s> values:%d", buf, param, n);
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
    return 1;
}

bool PPMLoader::readHeader(FILE *fp, unsigned long int *h, unsigned long int *w, uint16_t *maxval, uint8_t *type, MetaData* metadata)
{
    char header[255];

    // check PPM type (currently only supports 5 or 6)
    if (fgets(header, sizeof(header), fp) == nullptr || (header[0] != 'P') || (header[1] < '5') || (header[1] > '6'))
    {
        L_ERROR_P("invalid header at PPM file");
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
            L_ERROR_P("Image dimensions could not be read from line: %s", header);
            return false;
        }

        // first dimension has been read, try to read the second
        int error = nextLine(fp, header, sizeof header, metadata);
        if (error || sscanf(&header[0], "%lu", h) != 1) // duplicate code can be gotten rid of with a goto (not sure it's worth doing)
        {
            L_ERROR_P("Image dimensions could not be read from line: %s", header);
            return false;
        }
    }

    // get colour depth (metric?)
    if (nextLine(fp, header, sizeof header, metadata) != 0)
        return false;

    if (sscanf(&header[0], "%hu", maxval) != 1)
    {
        L_ERROR_P("Image metric could not be read form line: %s", header);
        return false;
    }

    // we assume that no comments exist after the color depth header line to avoid misinterpretation of '#' first data value

    L_INFO_P("Image is P%d PPM [%lu %lu] max=%u", *type, *w, *h, *maxval);
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
            fprintf(fp, "# @meta %s\t@values %" PRISIZE_T "\t", i->first.c_str(), i->second.size());
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
        L_ERROR_P("couldn't open file for writting <%s>", name.c_str());
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
        L_ERROR_P("couldn't open file for writting <%s>", name.c_str());
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
    size_t size = (forceTo8bitsShift >= 0 ?    1 : bytes) * w * h;

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
        L_ERROR_P("couldn't open file for writting <%s>", name.c_str());
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

RGB24Buffer *PPMLoaderRGB24::load(const string &name)
{
    MetaData meta;
    std::unique_ptr<G12Buffer> buffer(PPMLoader::loadG12(name, &meta));
    if (!buffer)
        return nullptr;

    if (!meta["bits"].empty() && meta["bits"][0] == 12)     // PGM has 12 bits
    {
        if (buffer->getSize() == Vector2d32(3840, 2748) ||
            buffer->getSize() == Vector2d32(480, 340))      // PGM with these params is the Bayer image
        {
            L_INFO_P("name:<%s> detected Bayer %dx%dx12", name.c_str(), buffer->w, buffer->h);

            auto params = Debayer::Parameters::BestDefaultsByExt(prefix1);
            params.setNumBitsOut(meta["bits"][0]);
            return Debayer::Demosaic(buffer.get(), params);
        }
    }

    return new RGB24Buffer(buffer.get());  // simple conversion to gray image
}

} //namespace corecvs
