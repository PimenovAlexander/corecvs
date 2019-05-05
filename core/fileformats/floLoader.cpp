#include "core/fileformats/floLoader.h"
#include "core/utils/utils.h"
#include "stdint.h"
#include <fstream>


namespace corecvs {

using namespace std;
const std::string FLOLoader::extention1 = ".flo";
const float FLOLoader::MAGIC_NUMBER = 202021.25;
const float FLOLoader::FLO_INFINITY = 1e9;


std::string ListFlowSaver::extention1 = ".fl";


bool FLOLoader::acceptsFile(const string &name)
{
    std::string lowercase = name;
    std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), ::tolower);

    return HelperUtils::endsWith(lowercase, extention1);
}

bool FLOSaver::acceptsFile(const string &name)
{
    std::string lowercase = name;
    std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), ::tolower);

    return HelperUtils::endsWith(lowercase, FLOLoader::extention1);
}


/**
 * // ".flo" file format used for optical flow evaluation
 * //
 * // Stores 2-band float image for horizontal (u) and vertical (v) flow components.
 * // Floats are stored in little-endian order.
 * // A flow value is considered "unknown" if either |u| or |v| is greater than 1e9.
 * //
 * //  bytes  contents
 * //
 * //  0-3     tag: "PIEH" in ASCII, which in little endian happens to be the float 202021.25
 * //          (just a sanity check that floats are represented correctly)
 * //  4-7     width as an integer
 * //  8-11    height as an integer
 * //  12-end  data (width*height*2*4 bytes total)
 * //          the float values for u and v, interleaved, in row order, i.e.,
 * //          u[row0,col0], v[row0,col0], u[row0,col1], v[row0,col1], ...
 * //
 *
 **/
FloatFlowBuffer *FLOLoader::load(const string &name)
{
    FloatFlowBuffer *toReturn = NULL;

    FILE *fp = fopen(name.c_str(), "rb");
    if (fp == nullptr)
    {
        SYNC_PRINT(("FLOLoader::load(): can't open file <%s>", name.c_str()));
        return nullptr;
    }

    float magicData = 0.0f;
    uint32_t h, w;

    size_t result = fread(&magicData, sizeof(float), 1, fp);

    if (result != 1) {
        SYNC_PRINT (("FLOLoader::load(): Unable to read header\n"));
        return NULL;
    }

    if (magicData != MAGIC_NUMBER) {
        SYNC_PRINT (("FLOLoader::load(): Unable to read magic number from the header of <%s> epected %lf got %lf\n", name.c_str(), MAGIC_NUMBER, magicData));
        return NULL;
    }

    result = 0;
    result += fread(&w, sizeof(uint32_t), 1, fp);
    result += fread(&h, sizeof(uint32_t), 1, fp);

    if (result != 2) {
        SYNC_PRINT (("FLOLoader::load(): Unable to read flow size from header\n"));
        goto close_file;
    }

    SYNC_PRINT(("FLOLoader::load(): Flow Buffer size is [%dx%d]\n", w, h));
    toReturn = new FloatFlowBuffer(h, w);

    for (uint32_t i = 0; i < h; i++)
    {
        for (uint32_t j = 0; j < w; j++)
        {
            float u,v;

            int loaded = 0;
            loaded += fread(&u, sizeof(float), 1, fp);
            loaded += fread(&v, sizeof(float), 1, fp);

            if (loaded != 2) {
                SYNC_PRINT(("FLOLoader::load(): Unable to read file.\n"));
                goto close_file;
            }

            if (u > FLOLoader::FLO_INFINITY || u > FLOLoader::FLO_INFINITY) {
                toReturn->element(i,j) = FloatFlow(false);
            } else {
                toReturn->element(i,j) = FloatFlow(u,v);
            }
        }
    }

close_file:

    fclose(fp);
    return toReturn;
}

bool FLOSaver::save(const FloatFlowBuffer &buffer, const string &name, int /*quality*/)
{
    FILE *fp = fopen(name.c_str(), "wb");
    if (fp == nullptr)
    {
        SYNC_PRINT(("can't open file <%s>", name.c_str()));
        return false;
    }

    float magicData = FLOLoader::MAGIC_NUMBER;
    uint32_t h = buffer.h;
    uint32_t w = buffer.w;

    fwrite(&magicData, sizeof(float), 1, fp);
    fwrite(&w, sizeof(uint32_t), 1, fp);
    fwrite(&h, sizeof(uint32_t), 1, fp);

    for (uint32_t i = 0; i < h; i++)
    {
        for (uint32_t j = 0; j < w; j++)
        {
            float u,v;
            if (buffer.isElementKnown(i,j)) {
                FloatFlow f = buffer.element(i,j);
                u = f.vector.x();
                v = f.vector.y();
            } else {
                u = FLOLoader::FLO_INFINITY * 10;
                v = FLOLoader::FLO_INFINITY * 10;
            }

            fwrite(&u, sizeof(float), 1, fp);
            fwrite(&v, sizeof(float), 1, fp);
        }
    }

    fclose(fp);

    return false;
}

bool ListFlowSaver::acceptsFile(const string &name)
{
    std::string lowercase = name;
    std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), ::tolower);

    return HelperUtils::endsWith(lowercase, extention1);
}

bool ListFlowSaver::save(const FloatFlowBuffer &buffer, const string &fileName, int /*quality*/)
{
    std::ofstream file;
    file.open(fileName, ios::out);
    file.imbue(std::locale("C"));

    if (file.fail())
    {
        SYNC_PRINT(("ListFlowSaver::save(): Can't open flow file <%s> for writing\n", fileName.c_str()));
        return false;
    }

    file << buffer.w << " " << buffer.h << endl;

    for (int32_t i = 0; i < buffer.h; i++)
    {
        for (int32_t j = 0; j < buffer.w; j++)
        {
            if (buffer.isElementKnown(i,j))
            {
                file << (float)j << " " << (float)i << " "
                     << (j + buffer.element(i,j).vector.x()) << " " << (i + buffer.element(i,j).vector.y()) << endl;
            }
        }
    }

    file.close();
    return true;
}

bool ListFlowLoader::acceptsFile(const string &name)
{
    return ListFlowSaver().acceptsFile(name);
}

FloatFlowBuffer *ListFlowLoader::load(const string &fileName)
{
    std::ifstream file;
    file.open(fileName, std::ifstream::in);
    if (file.fail())
    {
        SYNC_PRINT(("ListFlowSaver::load(): Can't open flow file <%s> for writing\n", fileName.c_str()));
        return NULL;
    }
    file.imbue(std::locale("C"));

    int h = 0;
    int w = 0;

    file >> w;
    file >> h;

    FloatFlowBuffer *toReturn = new FloatFlowBuffer(h,w);
    while (!file.eof())
    {
        string line;
        HelperUtils::getlineSafe (file, line);

        if (line.empty())
            continue;

        istringstream work(line);
        float  u_prev, v_prev, u_curr, v_curr;
        work >> u_prev;
        work >> v_prev;
        work >> u_curr;
        work >> v_curr;

        if (toReturn->isValidCoord(fround(u_prev), fround(v_prev)))
        {
            toReturn->element(fround(v_prev), fround(u_prev)) = FloatFlow(u_curr - u_prev, v_curr - v_prev);
        }
    }
    return toReturn;
}


} // namespace corecvs
