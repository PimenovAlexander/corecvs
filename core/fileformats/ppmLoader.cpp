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

#include "utils/global.h"
#include "utils/utils.h"
#include "fileformats/ppmLoader.h"
#include "buffers/converters/debayer.h"
#include "utils/log.h"

namespace corecvs {

string PPMLoader::extention1(".pgm");
string PPMLoader::extention2(".ppm");

bool PPMLoader::acceptsFile(string name)
{
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

    return HelperUtils::endsWith(name, extention1) ||
           HelperUtils::endsWith(name, extention2);
}

G12Buffer* PPMLoader::loadG16(const string& name)
{
    if (HelperUtils::endsWith(name, extention1))   // == pgm
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

G12Buffer* PPMLoader::loadG12(const string& name, MetaData *meta, bool load16bit, bool bigEndian)
{
    G12Buffer *result = NULL;
    uint64_t size = 0;
    uint8_t *charImage = NULL;

    bool calcWhite = false;
    int white = 0;

    SYNC_PRINT(("PPMLoader::loadG12(%s meta:%s as16:%s)\n", name.c_str(), (meta ? "fill" : "ignore"), load16bit ? "true" : "false"));

    std::ifstream file;
    file.open(name, std::ios::in | std::ios::binary);
    if (file.fail())
    {
        SYNC_PRINT(("PPMLoader::loadG12(): can't open file <%s>\n", name.c_str()));
        return result;
    }

    PPMHeader header;
    readHeader(file, header);
    if (file.bad())
    {
        SYNC_PRINT(("invalid header at file: %s\n", name.c_str()));
        goto close_file;
    }

    if (header.type != TYPE_P5 && header.type != TYPE_P6)
    {
        SYNC_PRINT(("We only support P6 and P5 files: %s\n", name.c_str()));
        goto close_file;

    }

    if (header.type == TYPE_P6)      // file has 3 channels instead of 1
    {
        SYNC_PRINT(("invalid type (%s) of the PPM file: %s\n", getName(header.type), name.c_str()));
        goto close_file;
    }

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
        while (header.maxval >> int(metadata["bits"][0]))
        {
            metadata["bits"][0]++;
        }
        if (metadata["white"].empty())
            calcWhite = true;
    }

    result = new G12Buffer(header.h, header.w, false);

    size = (header.maxval < 0x100 ? 1 : 2) * header.w * header.h;

    // for reading we don't need to account for possible system byte orders, so just use a 8-bits buffer
    charImage = new uint8_t[size];
    file.read((char *)charImage, size);
    if (!file)
    {
        L_ERROR_P("couldn't read %u bytes from file: %s", (unsigned)size, name.c_str());
        delete_safe(charImage);
        delete_safe(result);
        goto done;
    }

    if (header.maxval < 0x100)             // 1-byte case
    {
        cint shift = load16bit ? 8 : 0;
        for (int32_t k = 0, i = 0; i < result->h; i++)
            for (int32_t j = 0; j < result->w; j++)
            {
                uint8_t pix = result->element(i, j) = charImage[k++] << shift;
                if (calcWhite && pix > white)
                    white = pix;
            }
    }
    else                            // 2-bytes case
    {
        cint realMax = load16bit ? (1 << 16) - 1 : (G12Buffer::BUFFER_MAX_VALUE);
        // here we need to calculate shift to compress data into a 12-bits buffer
        int shiftCount;
        for (shiftCount = 0; (header.maxval >> shiftCount) > realMax; shiftCount++);

        int k = 0;
        for (int i = 0; i < result->h; i++)
        {            
            for (int j = 0; j < result->w; j++, k+=2)
            {
                uint8_t p0 = charImage[k + 0];
                uint8_t p1 = charImage[k + 1];

                uint16_t pix = 0;
                if (bigEndian) {
                    pix = (p0 << 8) | p1;
                } else {
                    pix = (p1 << 8) | p0;
                }

                pix >>= shiftCount;
                result->element(i, j) = pix;

                if (pix > realMax) {
                    SYNC_PRINT(("Internal error in image loader %d > %d at (y=%d x=%d)\n", pix, realMax, i, j));
                }
                if (calcWhite && pix > white)
                    white = pix;
            }
        }
    }

    if (calcWhite) {
        meta->at("white").push_back(white);
    }

done:
    deletearr_safe(charImage);
close_file:
    file.close();
    return result;
}

RGB48Buffer* PPMLoader::loadRgb48(const string& name, MetaData *meta)
{
    // PPM headers variable declaration
    unsigned long int h = 0;
    unsigned long int w = 0;
    RGB48Buffer *result = NULL;
    uint8_t type;
    unsigned short int maxval = 0;
    bool calcWhite = false;
    int white = 0;

    std::ifstream file;
    file.open(name, std::ios::in | std::ios::binary);
    if (file.fail())
    {
        SYNC_PRINT(("PPMLoader::loadRgb48(): can't open file <%s>\n", name.c_str()));
        return result;
    }


    PPMHeader header;

    if (!readHeader(file, header) || (type != 6))
    {
        L_ERROR_P("invalid header at <%s>", name.c_str());
        file.close();
        return result;
    }


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

    result = new RGB48Buffer(h, w, false);

    // image size in bytes
    uint64_t size = (maxval < 0x100 ? 1 : 2) * w * h * 3;

    // for reading we don't need to account for possible system byte orders, so just use a 8bit buffer
    uint8_t *charImage = new uint8_t[size];
    if (charImage == nullptr)
    {
        CORE_ASSERT_FAIL_P(("out of memory on allocate %" PRISIZE_T " bytes", size));
        goto done;
    }

    file.read((char *)charImage, size);
    if (!file)
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
    file.close();
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

void PPMLoader::skipComments (std::istream &file)
{
    while (true)
    {
        int c = file.peek();
        // cout << "Peeked:" << c << "(" << ((char)c) << ")" << endl;
        if (std::isspace(c)) {
            c = file.get();
            // cout << "Extraced:" << c << "(" << ((char)c) << ")" << endl;
            continue;
        }
        if (c == '#') {
            std::string comment;
            HelperUtils::getlineSafe(file, comment);
            if (trace) {
                cout << "Comment :" << comment << endl;
            }
            continue;
        }
        return;
    }
    return;
}

/**
 *  PPM header is quite flexible, without strict standard.
 *  For examples on how it could look consult
 *    https://en.wikipedia.org/wiki/Netpbm_format
 *    http://paulbourke.net/dataformats/ppm/
 *
 *
 **/
bool PPMLoader::readHeader(std::istream &file, PPMHeader &header)
{
    char magic1 = ' ';
    char magic2 = ' ';

    file >> magic1;
    file >> magic2;

    if (file.bad()) {
        return false;
    }

    if (magic1 != 'P') {
        SYNC_PRINT(("PPM should start with symbol 'P'"));
        return false;
    }

    do {
        if (magic2 == '3') {
            header.type = TYPE_P3;
            break;
        }
        if (magic2 == '5') {
            header.type = TYPE_P5;
            break;
        }
        if (magic2 == '6') {
            header.type = TYPE_P6;
            break;
        }
        SYNC_PRINT(("We only support PPM with P5 or P6\n"));
        return false;
    } while (false);

    skipComments(file);
    file >> header.w;
    skipComments(file);
    file >> header.h;
    skipComments(file);
    file >> header.maxval;

    while(file.peek() != '\n' && file) {
        file.get();
    }
    file.get(); /*Extracting actual new line*/

    SYNC_PRINT(("Image is P%d PPM [%lu %lu] max=%u\n", header.type == TYPE_P5 ? 5 : 6, header.w, header.h, header.maxval));

#if 0
    // get dimensions
    if (nextLine(fp, header, sizeof(header), metadata) != 0)
        return false;

    // parse dimensions
    SYNC_PRINT(("Header line: %s\n", header));
    if (sscanf(header, "%lu %lu", w, header) != 2)
    {
        // try to parse dimensions in Photoshop-like format (when a newline is used instead of whitespace or tabulation)
        if (sscanf(header, "%lu", w) != 1)
        {
            L_ERROR_P("Image dimensions could not be read from line: %s", header);
            return false;
        }

        // first dimension has been read, try to read the second
        int error = nextLine(fp, header, sizeof header, metadata);
        if (error || sscanf(&header[0], "%lu", header) != 1) // duplicate code can be gotten rid of with a goto (not sure it's worth doing)
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

    L_INFO_P("Image is P%d PPM [%lu %lu] max=%u", header.type, header.w, header.h, header.maxval);

#endif
    return true;
}

bool PPMLoader::writeHeader(std::ostream &file, const PPMHeader &header)
{   
    if ( header.w == 0 ) return false;
    if ( header.type != TYPE_P5 && header.type != TYPE_P6) return false;

    file << "P" << (header.type == TYPE_P5 ? "5" : "6") << endl;

    if (header.metadata != NULL)
    {
        const MetaData &metadata = *header.metadata;

        for (const auto &it : metadata)
        {
            file << "# @meta "<< it.second.size() << "\t@values " << it.second.size() << "\t";
            for (size_t j = 0; j < it.second.size(); j++)
                file << it.second[j] << " ";
            file << endl;
        }
    }

    file << header.w      << " " << header.h << endl;
    file << header.maxval << endl;

    return true;
}

int PPMLoader::save(const string& name, RGB24Buffer *buffer, MetaData* metadata)
{
    if (buffer == NULL)
        return -1;

    std::ofstream file;
    file.open(name, std::ios::out | std::ios::binary);
    if (file.fail())
    {
        SYNC_PRINT(("PPMLoader::save(): couldn't open file for writeing <%s>\n", name.c_str()));
        return -2;
    }

    PPMHeader header;

    header.h = buffer->h;
    header.w = buffer->w;
    header.type = TYPE_P6;
    header.maxval = 0xff;
    header.metadata = metadata;

    writeHeader(file, header);

    uint8_t *charImage = new uint8_t[3 * header.w * header.h];

    for (int i = 0; i < buffer->h; i++)
        for (int j = 0; j < buffer->w; j++)
        {
            uint8_t elemval[3] = {
                buffer->element(i, j).r(),
                buffer->element(i, j).g(),
                buffer->element(i, j).b()
            };
            for (int offset = i * header.w + j, k = 0; k < 3; k++)
                charImage[offset * 3 + k] = elemval[k];
        }

    file.write((char *)charImage, 2 * header.w * header.h);

    deletearr_safe(charImage);
    file.close();
    return 0;
}

int PPMLoader::save(const string& name, G12Buffer *buffer, MetaData* metadata, int forceTo8bitsShift)
{
    if (buffer == NULL)
        return -1;

    std::ofstream file;
    file.open(name, std::ios::out | std::ios::binary);
    if (file.fail())
    {
        SYNC_PRINT(("PPMLoader::save(): couldn't open file for writeing <%s>\n", name.c_str()));
        return -2;
    }

    PPMHeader header;
    header.h = buffer->h;
    header.w = buffer->w;


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
    size_t size = (forceTo8bitsShift >= 0 ?    1 : bytes) * buffer->w * buffer->h;


    header.maxval = maxVal;
    header.metadata = metadata;
    header.type = TYPE_P5;

    writeHeader(file, header);

    uint8_t *charImage = new uint8_t[size];
    if (bytes == 2)
    {
        for (int i = 0; i < buffer->h; i++)
        {
            for (int j = 0; j < buffer->w; j++)
            {
                int offset = i * header.w + j;
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
        for (int i = 0; i < buffer->h; i++)
        {
            for (int j = 0; j < buffer->w; j++)
            {
                charImage[i * header.w + j] = buffer->element(i, j) & 0xFF;
            }
        }
    }

    file.write((char *)charImage, size);
    file.close();
    deletearr_safe(charImage);
    return 0;
}

int PPMLoader::save(const string& name, RGB48Buffer *buffer, MetaData* metadata, int forceTo8bitsShift)
{
    if (buffer == NULL)
        return -1;

    std::ofstream file;
    file.open(name, std::ios::out | std::ios::binary);
    if (file.fail())
    {
        SYNC_PRINT(("PPMLoader::save(): couldn't open file for writeing <%s>\n", name.c_str()));
        return -2;
    }

    PPMHeader header;
    header.h = buffer->h;
    header.w = buffer->w;

    int maxval = 0;
    for (unsigned long i = 0; i < header.h; i++)
        for (unsigned long j = 0; j < header.w; j++)
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

    header.maxval = forceTo8bitsShift >= 0 ? 255 : maxVal;
    header.metadata = metadata;
    header.type = TYPE_P6;

    writeHeader(file, header);

    size_t size = 3 * (forceTo8bitsShift >= 0 ? 1 : bytes) * header.w * header.h;
    uint8_t *charImage = new uint8_t[size];

    for (int i = 0; i < buffer->h; i++)
        for (int j = 0; j < buffer->w; j++)
        {
            uint16_t elemval[3] = {
                buffer->element(i, j).r(),
                buffer->element(i, j).g(),
                buffer->element(i, j).b()
            };
            int offset = 3 * (i * header.w + j);

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

    file.write((char *)charImage, size);
    file.close();
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

            auto params = Debayer::Parameters::BestDefaultsByExt(extention1);
            params.setNumBitsOut(meta["bits"][0]);
            return Debayer::Demosaic(buffer.get(), params);
        }
    }

    return new RGB24Buffer(buffer.get());  // simple conversion to gray image
}

} //namespace corecvs
