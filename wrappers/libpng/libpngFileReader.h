#ifndef LIBPNGFILEREADER_H
#define LIBPNGFILEREADER_H

#include <string>
#include <stdint.h>

#include "global.h"

#include "bufferLoader.h"
#include "g12Buffer.h"
#include "rgb24Buffer.h"

namespace corecvs {

using std::string;



class PNGLoader
{
    public:
        virtual RGB24Buffer * loadRGB(string name);
        virtual bool save(string name, RGB24Buffer *buffer);
};


} //namespace corecvs
#endif // LIBPNGFILEREADER_H

