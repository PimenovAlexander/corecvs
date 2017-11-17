#ifndef ABSTRACTBUFFERPARAMS_H
#define ABSTRACTBUFFERPARAMS_H

/**
 *  TODO: I doubt we need this file.
 **/

namespace corecvs {

/**
 * These are useful methods to serialize integer types not depending on the current endianess
 */
template <typename IntegerType>
ostream& write_integer_bin(ostream& os, IntegerType value)
{
    for (unsigned size = sizeof(IntegerType); size != 0; size--, value >>= 8) {
        os.put(static_cast<char>(value & 0xFF));
    }
    return os;
}

template <typename IntegerType>
istream& read_integer_bin(istream& is, IntegerType& value)
{
    value = 0;
    for (unsigned size = 0; size < sizeof(IntegerType); size++) {
        value |= is.get() << (8 * size);
    }
    return is;
}

/**
 * This is a basis part of the AbstractBuffer template - serializable buffer params.
 * FIX THIS ASAP
 */
template<typename IndexType = int32_t>
class AbstractBufferParams
{
public:
    /** Calculate number of elements in a buffer */
    inline size_t numElements() const   { return (size_t)h * stride; }

//private:
    IndexType     h;
    IndexType     w;
    IndexType     stride;

    static cchar TAB = '\t';
    static const cchar* sFormatStr;


protected:
    AbstractBufferParams() :
         h(0), w(0), stride(0)          {}

    void    setH     (IndexType _h)           { h      = _h; }
    void    setW     (IndexType _w)           { w      = _w; }
    void    setStride(IndexType _s)           { stride = _s; }

    static  bool_t dump(ostream& s, int  h, int  w, int  stride, uint elemSize, bool_t binaryMode);
    static  bool_t load(istream& s, int &h, int &w, int &stride, uint elemSize, bool_t binaryMode);

    static  bool_t write(ostream& s, int    val, bool_t binaryMode);
    static  bool_t read (istream& s, int   &val, bool_t binaryMode);
    static  bool_t write(ostream& s, cchar* str, bool_t binaryMode);
    static  bool_t read (istream& s,  char* str, size_t size, bool_t binaryMode);

template<size_t size>
    static  bool_t read (istream& s, char (&str)[size], bool_t binaryMode) { return read(s, (char*)str, size, binaryMode); }

    bool_t  dump(ostream& s, uint elemSize, bool_t binaryMode) const { return dump(s, h, w, stride, elemSize, binaryMode); }
    bool_t  load(istream& s, uint elemSize, bool_t binaryMode)       { return load(s, h, w, stride, elemSize, binaryMode); }
};


template<typename IndexType>
const cchar* AbstractBufferParams<IndexType>::sFormatStr = "#dims:%d (%d) x %d x %u";


template<typename IndexType>
bool_t AbstractBufferParams<IndexType>::dump(ostream& s, int h, int w, int stride, uint elemSize, bool_t binaryMode)
{
    if (binaryMode)
    {
        write_integer_bin(s, w);
        write_integer_bin(s, stride);
        write_integer_bin(s, h);
        write_integer_bin(s, elemSize);
    }
    else
    {
        char buf[256];
        s.write(buf, snprintf2buf(buf, sFormatStr, w, stride, h, elemSize));
        s << std::endl;
    }
    return !s.bad();
}

template<typename IndexType>
bool_t AbstractBufferParams<IndexType>::load(istream& s, int &h, int &w, int &stride, uint elemSize, bool_t binaryMode)
{
    int width, stridePix, height, elSize;
    size_t pos = s.tellg();

    if (binaryMode)
    {
        read_integer_bin(s, width);
        read_integer_bin(s, stridePix);
        read_integer_bin(s, height);
        read_integer_bin(s, elSize);
    }
    else
    {
        char buf[256];
        read(s, buf, binaryMode);
        if (sscanf(buf, sFormatStr, &width, &stridePix, &height, &elSize) != 4)
            elSize = 0;
    }

    if ((uint)elSize != elemSize)
    {
        s.seekg(pos);                                       // restore pos
        return false;
    }

    h      = height;
    w      = width;
    stride = stridePix;
    return true;
}

template<typename IndexType>
bool_t AbstractBufferParams<IndexType>::write(ostream& s, int val, bool_t binaryMode)
{
    if (binaryMode)
    {
        write_integer_bin(s, val);
    }
    else
    {
        s << val << ' ';
    }
    return !s.bad();
}


template<typename IndexType>
bool_t AbstractBufferParams<IndexType>::read(istream& s, int &val, bool_t binaryMode)
{
    int value;
    size_t pos = s.tellg();

    if (binaryMode)
    {
        read_integer_bin(s, value);
    }
    else
    {
        s >> value;
    }

    if (s.bad())
    {
        s.seekg(pos);                                       // restore pos
        return false;
    }

    val = value;
    return true;
}


template<typename IndexType>
bool_t AbstractBufferParams<IndexType>::write(ostream& s, cchar* str, bool_t binaryMode)
{
    s << str << std::endl;
    CORE_UNUSED(binaryMode);
    return !s.bad();
}


template<typename IndexType>
bool_t AbstractBufferParams<IndexType>::read(istream& s, char* str, size_t size, bool_t binaryMode)
{
    if (str != NULL && size) {
        s.getline(str, size);                               // read one line to the output buffer
    }
    else {
        char buf[256];
        s.getline(buf, CORE_COUNT_OF(buf));                 // skip one line
    }
    CORE_UNUSED(binaryMode);
    return !s.bad();
}

} // namespace

#endif // ABSTRACTBUFFERPARAMS_H
