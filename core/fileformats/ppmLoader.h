/**
 * \file ppmLoader.h
 * \brief This is a header for PPM file format reader
 *
 * \ingroup cppcorefiles
 * \date    Oct 21, 2015
 * \author  alexander
 * \author  pavel.vasilev
 */
#ifndef PPMLOADER_H
#define PPMLOADER_H

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

    bool acceptsFile(std::string name);

    /**
     * Load methods
     *
     * \param   name            File to load.
     * \param [out] metadata    If non-null, it will be filled by data from PPM file
     *
     * \return  null if it fails, else a ptr to the allocated object
     */
    G12Buffer*   loadG16  (const string& name);
    G12Buffer*   loadG12  (const string& name, MetaData* metadata = nullptr, bool loadAs16 = false);
    RGB48Buffer* loadRgb48(const string& name, MetaData* metadata = nullptr);

    /**
     * Save methods
     *
     * \param   name                   Output file name.
     * \param [in]  buffer             Buffer to write to file (graymap .pgm for G12, RGB .ppm for RGB)
     * \param [in]  metadata           (Optional) If non-null writes metadata to file
     * \param [in]  forceTo8bitsShift  (Optional) If non-negative shift given #bits to right to get writtable 8-bits for the output
     *
     * \return  Error code.
     */
    int save(const string& name, RGB24Buffer *buffer, MetaData* metadata = nullptr);
    int save(const string& name, G12Buffer   *buffer, MetaData* metadata = nullptr, int forceTo8bitsShift = -1);
    int save(const string& name, RGB48Buffer *buffer, MetaData* metadata = nullptr, int forceTo8bitsShift = -1);

protected:
    static string prefix1, prefix2;

private:
    int  nextLine   (FILE *fp, char *buf, int sz, MetaData *metadata);
    bool readHeader (FILE *fp, unsigned long int *h, unsigned long int *w, uint16_t *maxval, uint8_t *type, MetaData* metadata);
    bool writeHeader(FILE *fp, unsigned long int h, unsigned long int w, uint8_t type, uint16_t maxval, MetaData* metadata);
};



class PPMLoaderG12 : public BufferLoader<G12Buffer>, public PPMLoader
{
public:
    virtual bool acceptsFile(const string &name) override
    {
        return PPMLoader::acceptsFile(name);
    }

    virtual G12Buffer *load(const string & name) override
    {
        return PPMLoader::loadG12(name);
    }

    virtual std::string name() override { return "PPMLoaderG12"; }
};

class PPMLoaderG16 : public PPMLoaderG12        // we may use G12Buffer container as it has 2bytes/pixel indeed
{
public:
    virtual G12Buffer *load(const string & name) override
    {
        return PPMLoader::loadG16(name);
    }

    virtual std::string name() override { return "PPMLoaderG16"; }
};

class PPMLoaderRGB24 : public BufferLoader<RGB24Buffer>, public PPMLoader
{
public:
    virtual bool acceptsFile(const string & name) override
    {
       return PPMLoader::acceptsFile(name);
    }

    virtual RGB24Buffer *load(const string & name) override;

    virtual std::string name() override { return "PPMLoaderRGB24"; }
};

} //namespace corecvs

#endif // PPMLOADER_H
