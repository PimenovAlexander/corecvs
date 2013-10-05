#pragma once
/**
 * \file rawLoader.h
 * \brief Add Comment Here
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

class RAWLoader : public BufferLoader<G12Buffer>
{
    static string prefix1;

    typedef enum {
        TYPE_8_BIT,                 /**< Format  8 bits for point      D7  D6  D5  D4  D3  D2  D1  D0 */
        TYPE_12_BIT_LOW,            /**< Format 16/12 bits for point   X   X   X   X   D11 D10 D9  D8  D7  D6  D5  D4  D3  D2  D1  D0 */
        TYPE_12_BIT_HIGH,
        TYPE_16_BIT_LOW,
        TYPE_16_BIT_HIGH,
        TYPE_16_BIT_LOW_ONLY,
        TYPE_16_BIT_HIGH_ONLY,
        TYPE_INVALID
    } RawFileType;

    int defaultH;
    int defaultW;
    RawFileType defaultType;

    void _init(int _defaultH, int _defaultW, RawFileType _defaultType)
    {
        this->defaultH    = _defaultH;
        this->defaultW    = _defaultW;
        this->defaultType = _defaultType;
    }


public:
    RAWLoader(int _defaultH, int _defaultW, RawFileType _defaultType)
    {
        _init(_defaultH, _defaultW, _defaultType);
    }

    RAWLoader()
    {
        _init(480, 640, RAWLoader::TYPE_12_BIT_HIGH);
    }

    virtual ~RAWLoader() {}

    RawFileType typeByParams(int bits, char type);
    uint16_t transform(RAWLoader::RawFileType type, uint8_t byte1, uint8_t byte2);


    virtual bool acceptsFile(string name);
    virtual G12Buffer * load(string name);
};

} //namespace corecvs
