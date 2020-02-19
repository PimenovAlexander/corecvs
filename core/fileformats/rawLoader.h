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


#include "utils/global.h"
#include "fileformats/bufferLoader.h"
#include "buffers/rgb24/rgb24Buffer.h"
#include "buffers/g12Buffer.h"

namespace corecvs {

using std::string;

class RAWLoaderBase
{
public:
    static string prefix1;

    typedef enum {
        TYPE_8_BIT,                 /**< Format  8 bits for point      D7  D6  D5  D4  D3  D2  D1  D0 */
        TYPE_12_BIT_LOW,            /**< Format 16/12 bits for point   X   X   X   X   D11 D10 D9  D8  D7  D6  D5  D4  D3  D2  D1  D0 */
        TYPE_12_BIT_HIGH,
        TYPE_12_BIT_SBS,        /**< Format side by side                                                                                */
                                /**  23                           16| 15                           8| 7                              |  */
                                /** | D11 D10 D9  D8  D7  D6  D5  D4|  D3  D2  D1  D0 F3  F2  F1  F0| F11 F10 F9  F8  F7  F6  F5  F4 |  */
        TYPE_16_BIT_LOW,
        TYPE_16_BIT_HIGH,
        TYPE_16_BIT_LOW_ONLY,
        TYPE_16_BIT_HIGH_ONLY,
        TYPE_24_BIT_RGB,
        TYPE_INVALID
    } RawFileType;

    void _init(int _defaultH, int _defaultW, RawFileType _defaultType)
    {
        this->defaultH    = _defaultH;
        this->defaultW    = _defaultW;
        this->defaultType = _defaultType;
    }


public:
    int defaultH;
    int defaultW;
    RawFileType defaultType;


    RAWLoaderBase(int _defaultH, int _defaultW, RawFileType _defaultType)
    {
        _init(_defaultH, _defaultW, _defaultType);
    }

    RAWLoaderBase()
    {
        _init(480, 640, RAWLoaderBase::TYPE_12_BIT_HIGH);
    }

    ~RAWLoaderBase() {}

    int typeToBytesNum(RawFileType type);
    int typeToBytesDen(RawFileType type);

    RawFileType typeByParams(int bits, char type);
    uint16_t transform(RawFileType type, uint8_t byte1, uint8_t byte2);


    bool acceptsFile(string name);
    G12Buffer   * loadG12(string name);
    RGB24Buffer * loadRGB24(string name);


    std::string name() { return "RAWLoader"; }

};

class RAWLoaderG12 : public BufferLoader<G12Buffer>, public RAWLoaderBase
{
public:
    virtual bool acceptsFile(const string & name) override
    {
        return RAWLoaderBase::acceptsFile(name);
    }

    virtual G12Buffer *load(const string & name) override
    {
        return RAWLoaderBase::loadG12(name);
    }

    virtual std::string name() override { return "RAWLoaderG12"; }
    virtual std::vector<std::string> extentions() override {
        return std::vector<std::string>({ RAWLoaderBase::prefix1 });
    }
};

class RAWLoaderRGB24 : public BufferLoader<RGB24Buffer>, public RAWLoaderBase
{
public:
    virtual bool acceptsFile(const string & name) override
    {
       return RAWLoaderBase::acceptsFile(name);
    }

    virtual RGB24Buffer *load(const string & name) override
    {
        return RAWLoaderBase::loadRGB24(name);
    }

    virtual std::string name() override { return "RAWLoaderRGB24"; }
    virtual std::vector<std::string> extentions() override {
        return std::vector<std::string>({ RAWLoaderBase::prefix1 });
    }
};



} //namespace corecvs
