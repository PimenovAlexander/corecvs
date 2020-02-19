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

#include "utils/global.h"

#include "fileformats/bufferLoader.h"
#include "buffers/g12Buffer.h"
#include "buffers/rgb24/rgbTBuffer.h"
#include "buffers/rgb24/rgb24Buffer.h"
#include "fileformats/metamap.h"

namespace corecvs {

class PPMLoader
{
public:
    PPMLoader() {}
    virtual ~PPMLoader() {}

    bool acceptsFile(std::string name);

    enum PPMType {
        TYPE_P6,
        TYPE_P5,
        TYPE_P3
    };

    static inline const char *getName(const PPMType &value)
    {
        switch (value)
        {
         case TYPE_P3 : return "TYPE_P3"; break ;
         case TYPE_P5 : return "TYPE_P5"; break ;
         case TYPE_P6 : return "TYPE_P6"; break ;
         default : return "Not in range"; break ;
        }
        return "Not in range";
    }

    struct PPMHeader {
        unsigned long int h;
        unsigned long int w;
        uint16_t maxval;
        PPMType type;
        MetaData* metadata = NULL;
    };

    /**
     * Load methods
     *
     * \param   name            File to load.
     * \param [out] metadata    If non-null, it will be filled by data from PPM file
     *
     * \return  null if it fails, else a ptr to the allocated object
     */
    G12Buffer*   loadG16  (const string& name);
    G12Buffer*   loadG12  (const string& name, MetaData* metadata = nullptr, bool load16bit = false, bool bigEndian = true);
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
    static string extention1;
    static string extention2;

private:
    /* Depricated */
    int  nextLine   (FILE *fp, char *buf, int sz, MetaData *metadata);   
    void skipComments(std::istream &file);

public:
    bool trace = false;

    bool readHeader (std::istream &file,       PPMHeader &header);
    bool writeHeader(std::ostream &file, const PPMHeader &header);
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
    virtual std::vector<std::string> extentions() { return {extention1, extention2}; }

};

class PPMLoaderG16 : public PPMLoaderG12        // we may use G12Buffer container as it has 2bytes/pixel indeed
{
public:
    virtual G12Buffer *load(const string & name) override
    {
        return PPMLoader::loadG16(name);
    }

    virtual std::string name() override { return "PPMLoaderG16"; }
    virtual std::vector<std::string> extentions() { return {extention1, extention2}; }

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
    virtual std::vector<std::string> extentions() { return {extention1, extention2}; }

};

} //namespace corecvs

#endif // PPMLOADER_H
