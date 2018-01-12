/**
 * \file ppmLoader.h
 * \brief This is a header for PPM file format reader
 *
 * \ingroup cppcorefiles
 * \date    Oct 21, 2015
 * \author  alexander
 * \author  pavel.vasilev
 */
#ifndef PPMLOADER_H_
#define PPMLOADER_H_

#include <string>
#include <memory>

#include "core/utils/global.h"

#include "core/fileformats/bufferLoader.h"
#include "core/buffers/g12Buffer.h"
#include "core/buffers/rgb24/rgbTBuffer.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/metamap.h"

namespace corecvs {

class PPMLoader
{
public:
    PPMLoader() {}
    virtual ~PPMLoader() {}

    virtual bool acceptsFile(string name);

    virtual G12Buffer * loadG12(string name);
    virtual G12Buffer * loadG16(string name);

    /**
     * Load method overload.
     *
     * \author  pavel.vasilev
     * \date    Oct 21, 2015
     *
     * \param   name            File to load.
     * \param [out] metadata    If non-null, this will be filled by any metadata encountered in PPM
     *                          file.
     *
     * \return  null if it fails, else a G12Buffer*.
     */
    G12Buffer *  loadMeta                (const string& name, MetaData* metadata = nullptr);

    G12Buffer*   g12BufferCreateFromPGM  (const string& name, MetaData* metadata = nullptr);
    G12Buffer*   g16BufferCreateFromPPM  (const string& name, MetaData* metadata = nullptr);

    RGB48Buffer* loadRGB                 (const string& name, MetaData* metadata = nullptr);
    RGB48Buffer* rgb48BufferCreateFromPPM(const string& name, MetaData* metadata = nullptr);

    /**
     * Save method overloads.
     *
     * \author  pavel.vasilev
     * \date    Oct 21, 2015
     *
     * \param   name                   Output file name.
     * \param [in]  buffer             Buffer to write to file. Internally writes graymap .pgm for G12 and
     *                                 RGB .ppm for RGB.
     * \param [in]  metadata           (Optional) If non-null, writes metadata to file.
     * \param [in]  forceTo8bitsShift  (Optional) If non-negative, shift given #bits to right to get writtable 8-bits for the output.
     *
     * \return  Error code.
     */
    int save(const string& name, RGB24Buffer *buffer, MetaData* metadata = nullptr);
    int save(const string& name, G12Buffer   *buffer, MetaData* metadata = nullptr, int forceTo8bitsShift = -1);
    int save(const string& name, RGB48Buffer *buffer, MetaData* metadata = nullptr, int forceTo8bitsShift = -1);

    // TODO: remove this. 
    int saveG16(const string& name, G12Buffer *buffer);

private:
    static string prefix1, prefix2;

    int  nextLine(FILE *fp, char *buf, int sz, MetaData *metadata);
    bool readHeader(FILE *fp, unsigned long int *h, unsigned long int *w, uint16_t *maxval, uint8_t *type, MetaData* metadata);
    bool writeHeader(FILE *fp, unsigned long int h, unsigned long int w, uint8_t type, uint16_t maxval, MetaData* metadata);
};



class PPMLoaderG12 : public BufferLoader<G12Buffer>, public PPMLoader
{
public:
    virtual bool acceptsFile(string name) override
    {
        return PPMLoader::acceptsFile(name);
    }

    virtual G12Buffer *load(string name) override
    {
        return PPMLoader::loadG12(name);
    }

    virtual std::string name() override { return "PPMLoaderG12"; }
};

class PPMLoaderG16 : public PPMLoaderG12        // we may use G12Buffer container as it has 2bytes/pixel indeed
{
public:
    virtual G12Buffer *load(string name) override
    {
        return PPMLoader::loadG16(name);
    }

    virtual std::string name() override { return "PPMLoaderG16"; }
};

class PPMLoaderRGB24 : public BufferLoader<RGB24Buffer>, public PPMLoader
{
public:
    virtual bool acceptsFile(string name) override
    {
       return PPMLoader::acceptsFile(name);
    }

    virtual RGB24Buffer *load(string name) override
    {
        G12Buffer *buffer = PPMLoader::loadG12(name);
        if (buffer == NULL) {
            return NULL;
        }
        RGB24Buffer *result = new RGB24Buffer(buffer);
        delete_safe(buffer);
        return result;
    }

    virtual std::string name() override { return "PPMLoaderRGB24"; }
};

} //namespace corecvs

#endif 
