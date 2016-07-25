#ifndef LIBJPEGFILEREADER_H
#define LIBJPEGFILEREADER_H

#include "rgb24Buffer.h"
#include "bufferFactory.h"


class LibjpegFileReader : public corecvs::BufferLoader<corecvs::RGB24Buffer>
{
    static string prefix1;
    static string prefix2;
public:
    static int registerMyself()
    {
        corecvs::BufferFactory::getInstance()->registerLoader(new LibjpegFileReader());
        return 0;
    }

    virtual bool acceptsFile(string name) override;
    virtual corecvs::RGB24Buffer *load(string name) override;

    LibjpegFileReader();
    virtual ~LibjpegFileReader();
};

#endif // LIBJPEGFILEREADER_H
