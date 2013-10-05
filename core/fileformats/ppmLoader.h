#pragma once
/**
 * \file ppmLoader.h
 * \brief This is a header for PPM file format reader
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */

#include <string>

#include "global.h"

#include "bufferLoader.h"
#include "g12Buffer.h"

namespace corecvs {

using std::string;

class PPMLoader : public BufferLoader<G12Buffer>
{
    static string prefix1;

    bool readHeader(FILE *fp, unsigned long int *h, unsigned long int *w, unsigned short int *maxval, int *type);

public:
    PPMLoader() {}
    virtual ~PPMLoader() {}

    virtual bool acceptsFile(string name);
    virtual G12Buffer * load(string name);

    G12Buffer* g12BufferCreateFromPPM (string& name);
    G12Buffer* g16BufferCreateFromPPM (string& name);

    int save(string name, G12Buffer *buffer);
    int saveG16(string name, G12Buffer *buffer);

};

} //namespace corecvs
