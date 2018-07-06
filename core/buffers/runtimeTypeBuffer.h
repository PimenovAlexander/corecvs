#ifndef RUNTIMETYPEBUFFER_H
#define RUNTIMETYPEBUFFER_H

#include "core/utils/global.h"

#include <cstring>
#include <string>
#include <iostream>
#include <cassert>
#include <stdint.h>

#ifndef CV_8U
#define CV_8U 0
#endif

#ifndef CV_32F
#define CV_32F 5
#endif

namespace corecvs
{
    class RuntimeTypeBuffer;
}

std::ostream& operator<<(std::ostream &os, const corecvs::RuntimeTypeBuffer &b);
std::istream& operator>>(std::istream &is, corecvs::RuntimeTypeBuffer &b);


namespace corecvs
{
namespace BufferType
{
    enum RuntimeBufferDataType
    {
        U8,
        F32
    };
}


class G8Buffer;
class G12Buffer;
class RGB24Buffer;
/*
 * XXX: This is a small wrapper simplifying interfacing with different result type
 *      from different keypoint descriptors;
 *      This does not use AbstractBuffer 'cause I hate^W think this is too much
 */
class RuntimeTypeBuffer
{
public:
    RuntimeTypeBuffer() : data(0), rows(0), cols(0), sz(0), type(BufferType::U8) {}
    RuntimeTypeBuffer(const size_t &rows, const size_t &cols, const int &type =BufferType::U8) : rows(rows), cols(cols), sz(getTypeSize(type)), type(type)
    {
        allocate();
    }
    RuntimeTypeBuffer(const size_t &rows, const size_t &cols, const void* data, const int &type = BufferType::U8) : rows(rows), cols(cols), sz(getTypeSize(type)), type(type)
    {
        allocate();
        copy((uint8_t*)data);
    }

    ~RuntimeTypeBuffer()
    {
        free(data);
    }

    RuntimeTypeBuffer(const RuntimeTypeBuffer &b) : rows(b.rows), cols(b.cols), sz(b.sz), type(b.type)
    {
        allocate();
        copy(b.data);
    }

    RuntimeTypeBuffer& operator=(const RuntimeTypeBuffer &b)
    {
        if(this == &b)
            return *this;
        free(data);
        rows = b.rows;
        cols = b.cols;
        sz = b.sz;
        type = b.type;

        allocate();
        copy(b.data);
        return *this;
    }

    bool isValid() const
    {
        return data != 0;
    }

    size_t getRowSize() const
    {
        return sz * cols;
    }
    static size_t getTypeSize(int type)
    {
        switch(type)
        {
            case BufferType::U8:
                return 1;
            case BufferType::F32:
                return 4;
            default:
                CORE_ASSERT_FAIL_P(("RuntimeTypeBuffer::getTypeSize(%d): bad type", type));
                return 0;
        }
    }
    static size_t getTypeFromCvType(int type)
    {
        switch(type)
        {
            case CV_8U:
                return BufferType::U8;
            case CV_32F:
                return BufferType::F32;
            default:
                CORE_ASSERT_FAIL_P(("RuntimeTypeBuffer::getCvType(%d): bad type", type));
                return 0;
        }
    }
    size_t getCvType() const
    {
        switch(type)
        {
            case BufferType::U8:
                return CV_8U;
            case BufferType::F32:
                return CV_32F;
            default:
                CORE_ASSERT_FAIL_P(("RuntimeTypeBuffer::getCvType(): incorrect type %d", type));
                return 0;
        }
    }
    size_t getElSize() const
    {
        return sz;
    }

    size_t getDataSize() const
    {
        return rows * getRowSize();
    }

    template<typename T>
    T& at(const size_t &i, const size_t &j)
    {
        return *(T*)(data + sz * (i * cols + j));
    }
    template<typename T>
    const T& at(const size_t &i, const size_t &j) const
    {
        return *(T*)(data + sz * (i * cols + j));
    }
    template<typename T>
    T* row(const size_t &i)
    {
        return (T*)(data + sz * (i * cols ));
    }
    // FIXME
    template<typename T>
    T* row(const size_t &i) const
    {
        return (T*)(data + sz * (i * cols));
    }
    inline size_t getRows() const
    {
        return rows;
    }
    inline size_t getCols() const
    {
        return cols;
    }
    int getType() const
    {
        return type;
    }

    void downsample( int factor );

    void load(std::istream &is);
    void save(std::ostream &os) const;
    //void load(const std::string &filename);
    //void save(const std::string &filename) const;

    friend std::ostream& ::operator<<(std::ostream &os, const corecvs::RuntimeTypeBuffer &b);
    friend std::istream& ::operator>>(std::istream &is, corecvs::RuntimeTypeBuffer &b);

    /* Obvious converters */
    /** We need an infrastructure for data conversion based on fastkernels **/

    G8Buffer  *toG8Buffer();
    G12Buffer *toG12Buffer(double min = 0.0, double max = 1.0);
    /**
     * TODO: We probably we need to allow to select channel
     **/
    RGB24Buffer *toRGB24Buffer(double min = 0.0, double max = 1.0);

private:
    void allocate()
    {
        size_t bytes = getDataSize();
        data = (uint8_t*)malloc(bytes);
    }
    void copy(uint8_t *src)
    {
        //SYNC_PRINT(("RuntimeTypeBuffer:copy(%p -> %p [%" PRISIZE_T "]):called\n", src, data, getDataSize()));
        if (src != NULL) { /*memcpy: (ISO/IEC 9899:1999): 7.1.4 */
            memcpy(data, src, getDataSize());
        }
    }
    uint8_t *data;
    size_t  rows;
    size_t  cols;
    size_t  sz;
    int     type;
};
}

#endif
