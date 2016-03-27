#ifndef ABSTRACTBUFFER_H
#define ABSTRACTBUFFER_H
/**
 * \file abstractBuffer.h
 * \brief a header for AbstractBuffer.cpp
 *
 *
 * \ingroup cppcorefiles
 * \date Oct 28, 2009
 * \author alexander
 *
 * This file contains a base class for a rectangular buffer
 *
 *
 * \todo TODO: Check int type usage, no need to use int32_t where it is not crucial
 */
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <string>
#include <functional>
#include <type_traits>
#include <memory>                       // shared_ptr

#include "global.h"

#include "vector2d.h"
#include "memory/memoryBlock.h"
#include "tbbWrapper.h"                 // BlockedRange
#include "mathUtils.h"                  // randRanged

#if defined(WIN32)
# define aligned_alloc(a, sz)   _aligned_malloc(sz, a)
# define aligned_free(p)        _aligned_free(p)
#else
//# define aligned_alloc(a, sz) // is present at clib
# define aligned_free(p)        free(p)
#endif

namespace corecvs {

struct AlignedMemoryBlock
{
    AlignedMemoryBlock(size_t size, size_t align)
    {
        align = std::max(align, (size_t)1);
        CORE_ASSERT_TRUE_S(CORE_IS_POW2N(align));
        mData = aligned_alloc(align, ((size + align - 1) / align) * align);
    }

    AlignedMemoryBlock() : mData(nullptr)
    {}

    ~AlignedMemoryBlock()
    {
        aligned_free(mData);
        mData = nullptr;
    }

    void* getAlignedStart()
    {
        return mData;
    }

private:
    //AlignedMemoryBlock(const AlignedMemoryBlock&);

    void* mData;
};


struct AMBReference
{
    template<typename D>
    AMBReference(size_t size, size_t align, const D &deleter)
        : mSharedPtrBlock(new AlignedMemoryBlock(size, align), deleter)
    {}

    AMBReference(size_t size, size_t align) : mSharedPtrBlock(new AlignedMemoryBlock(size, align))
    {}

    AMBReference() : mSharedPtrBlock(nullptr)
    {}

    void* getAlignedStart()
    {
        return mSharedPtrBlock->getAlignedStart();
    }

private:
    std::shared_ptr<AlignedMemoryBlock> mSharedPtrBlock;
};

using std::string;
using std::cout;
using std::endl;

/**
 * This class holds the mapping function that is applied to the buffer element
 * to get another buffer element.
 *
 *
 **/
template<typename InputType, typename OutputType>
class GenericMap
{
public:
    /***
     * map function that transforms the element of the buffer
     */
    OutputType map(InputType &in) { return in; }
};

/* */
template<typename ElementType>
class BufferMap : public GenericMap<ElementType, ElementType>
{
};

/**
 * Preliminary definitions
 */
template<typename ElementType, typename IndexType>
class AbstractBuffer;

template<typename ElementType, typename IndexType>
class AbstractKernel;

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
class AbstractBufferParams
{
public:
    /** Calculate number of elements in a buffer */
    inline int numElements() const      { return h * stride; }

//private:
    int     h;
    int     w;
    int     stride;

    static  cchar TAB = '\t';

protected:
    AbstractBufferParams() :
         h(0), w(0), stride(0)          {}

    void    setH     (int _h)           { h      = _h; }
    void    setW     (int _w)           { w      = _w; }
    void    setStride(int _s)           { stride = _s; }

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

/**
 * This is a common buffer template.
 *
 *
 */
template<typename ElementType, typename IndexType = int32_t>
class AbstractBuffer : public AbstractBufferParams
{
public:
    /**
     * Type of the internal data type
     **/
    typedef ElementType InternalElementType;
    typedef IndexType   InternalIndexType;
    static const bool TRIVIALLY_COPY_CONSTRUCTIBLE =
#if __GNUG__ && __GNUC__ < 5
        __has_trivial_copy(ElementType);
#else
        std::is_trivially_copy_constructible<ElementType>::value;
#endif
    static const bool TRIVIALLY_DESTRUCTABLE = std::is_trivially_destructible<ElementType>::value;
    static const bool TRIVIALLY_DEFAULT_CONSTRUCTIBLE =
#if __GNUG__ && __GNUC__ < 5
        __has_trivial_constructor(ElementType);
#else
        std::is_trivially_constructible<ElementType>::value;
#endif

    enum BufferType {
        NORMAL_BUFFER,
        VIEW_BUFFER,
        EXTERNAL_DATA_BUFFER,
        EMPTY_BUFFER
    };

    /**
     * This constant represents the default alignment of the line in the buffer.
     * It is 16 bytes now to work with SSE and possibly with OPENCL.
     *
     **/
#if defined(WITH_SSE) /*|| defined(WITH_OPENCL)*/           // WITH_OPENCL is never defined for core projects to simplify projects deps
    static const int DATA_ALIGN_GRANULARITY = 0xF;          // alignment by 16 bytes
#else
    static const int DATA_ALIGN_GRANULARITY = 0xF;
#endif

    /**
     * Static member that will be used to prevent and debug memory leaks
     **/
    static atomic_int bufferCount;

    /**
     * The height of the buffer.
     *
     * \todo TODO: This field is public do far, but that is dangerous.
     **/
    //IndexType h;

    /**
     * The width of the buffer
     **/
    //IndexType w;

    /**
     * The stride of the buffer
     *
     * \todo TODO: This field is public do far, but that is dangerous.
     * TODO: Could happen it would be nice to have this in bytes
     **/
    //IndexType stride;

    /**
     * The given stride value to have automatically chosen stride of the buffer
     **/
    static const IndexType STRIDE_AUTO = (IndexType)0;

    /**
     * Sometimes underling buffer should not be deleted
     *
     **/
    //bool shouldDelete;


    /**
     * The properly aligned pointer to an array with data elements, memory of which is kept by the protected field "memoryBlock"
     **/
    ElementType *data;


    /**
     * So far this type of constructor is used only for view creation
     * Should refactor to make it private
     **/
    AbstractBuffer() : AbstractBufferParams()
        , data(NULL)
        , memoryBlock()
        , flags(EMPTY_BUFFER)
    {
        atomic_inc_and_fetch(&bufferCount);
    }

    /**
     *  Constructor that generates an empty buffer filled with zero values
     *
     *  \param h
     *      Height of the buffer
     *  \param w
     *      Width of the buffer
     *  \param shouldInit
     *      true if you need to clear the buffer with zeros
     **/
    AbstractBuffer(IndexType h, IndexType w, bool shouldInit = true, IndexType stride = STRIDE_AUTO)
    {
        _init(h, w, stride, shouldInit);
    }

    /**
     *  Constructor that generates an empty buffer filled with zero values
     *
     *  \param size
     *      Vector with buffer sizes (x is width, y is height)
     *  \param shouldInit
     *      true if you need to clear the buffer with zeros
     *  \param stride
     *      size of stride, STRIDE_AUTO for automatic stride
     *
     **/
    AbstractBuffer(Vector2d<IndexType> size, bool shouldInit = true, IndexType stride = STRIDE_AUTO)
    {
        _init(size.y(), size.x(), stride, shouldInit);
    }

    /**
     * Copy constructor.
     *
     * This makes an exact copy saving the stride
     */
    AbstractBuffer(const AbstractBuffer &that)
    {
        _init(that.getH(), that.getW(), that.getStride(), false);
        _copy(this->data, that.data, h, w, stride, stride);
//        memcpy(this->data, that.data, that.sizeInBytes());
    }

    /**
     * Constructor form the pointer to the buffer
     *
     * Not true - This makes an exact copy saving the stride
     */
    AbstractBuffer(const AbstractBuffer *that)
    {
//        _init(that->h, that->w, that->stride, false);
//        memcpy(this->data, that->data, that->sizeInBytes());
        _init(that->getH(), that->getW(), false);
        fillWith(*that);
    }


    /**
     * Constructor from static data
     *
     */
    AbstractBuffer(IndexType h, IndexType w, ElementType *_data)
    {
       _init(h, w, false);
       if (_data)
           fillWithRaw(_data);
    }

    AbstractBuffer(Vector2d<IndexType> size, ElementType *_data)
    {
       _init(size.y(), size.x(), false);
       if (_data)
           fillWithRaw(_data);
    }

    AbstractBuffer(IndexType h, IndexType w, IndexType stride, ElementType* data)
    {
        _init(h, w, stride, false);
        if (data)
            fillWithRaw(data);
    }

    AbstractBuffer(Vector2d<IndexType> size, IndexType stride, ElementType* data)
        : AbstractBuffer(size.y(), size.x(), stride, data)
    {
    }

#if 0
    /**
     * Constructor from data provided by arguments.
     *
     * \note - you should be very careful passing the right type
     */
    AbstractBuffer(IndexType h, IndexType w,  ...)
    {
        _init(h, w, false);
        va_list marker;
        va_start( marker, w );
        const ElementType value = va_arg(marker, ElementType);
        fillWithArgs(value, marker);
        va_end( marker );
    }
#endif

    AbstractBuffer(IndexType h, IndexType w, const ElementType &data, IndexType stride = STRIDE_AUTO)
    {
        _init(h, w, stride, false);
        fillWith(data);
    }

    AbstractBuffer(Vector2d<IndexType> size, const ElementType &data, IndexType stride = STRIDE_AUTO)
    {
        _init(size.y(), size.x(), stride, false);
        fillWith(data);
    }


    /*
     * It is slow and should not be used frequently
     */
    AbstractBuffer(const std::vector<std::vector<ElementType>> &vec, IndexType h, IndexType w, IndexType stride = STRIDE_AUTO)
    {
        std::vector<ElementType> el;
        el.reserve(w * h);
        for (IndexType i = 0; i < h; ++i)
        {
            for (IndexType j = 0; j < w; ++j)
            {
                el.emplace_back(vec[i][j]);
            }
        }
        _init(h, w, stride, false);
        fillWithRaw(&el[0]);
    }

    explicit operator std::vector<std::vector<ElementType>> () const
    {
        std::vector<std::vector<ElementType>> el(h);
        for (IndexType i = 0; i < h; ++i)
        {
            for (IndexType j = 0; j < w; ++j)
            {
                el[i].emplace_back(element(i, j));
            }
        }
        return el;
    }

    /*
     * NOTE: YOU SHOULD NEVER USE IT FOR SERIALIZING HUGE DATA
     */
    template<typename V>
    void accept(V& visitor)
    {
        auto vec = (std::vector<std::vector<ElementType>>)(*this);
        int w = this->w, h = this->h, stride = this->stride;
        visitor.visit(w, (IndexType)0, "width");
        visitor.visit(h, (IndexType)0, "height");
        visitor.visit(stride, w, "stride");
        visitor.visit(vec, "data");
        if (w == this->w && h == this->h && stride == this->stride && data)
        {
            std::vector<ElementType> vec2;
            for (IndexType i = 0; i < h; ++i)
                for (IndexType j = 0; j < w; ++j)
                    vec2.emplace_back(vec[i][j]);
            fillWithRaw(&vec2[0]);
            return;
        }
        *this = AbstractBuffer(vec, h, w, stride);
    }

    /**
     * Destructor. Main buffer will be deleted with MemoryBlockRef smart pointer destruction
     *
     * If the buffer is created on top of the other, the best way is
     * to first set data to NULL
     * \todo TODO: Should this be virtual? Usually we do not cast pointers to base type.
     **/
    ~AbstractBuffer()
    {
        this->data = NULL;
        atomic_dec_and_fetch(&bufferCount);                 // this must be always as it's incremented at each ctor
    }


template<typename ResultType>
    ResultType *createView(const IndexType y, const IndexType x, const IndexType h, const IndexType w)
    {
        // This is the only legitimate place to use default constructor
        ResultType *toReturn = new ResultType();

        toReturn->h          = h;
        toReturn->w          = w;
        toReturn->stride     = this->stride;

        toReturn->data       = &(this->element(y, x));
        toReturn->flags      = VIEW_BUFFER;
        /**
         * Prevent original buffer from being deleted
         * MemoryBlock magically counts references
         **/
        toReturn->memoryBlock = memoryBlock;
        return toReturn;
    }


template<typename ResultType>
    ResultType *createView()
    {
        return this->createView<ResultType>(0, 0, this->h, this->w);
    }


    /** This allows to build a buffer on top of the exising buffer. User is fully responsible for memory management **/
template<typename ResultType>
    static ResultType* CreateBuffer(IndexType _h, IndexType _w, IndexType _stride, ElementType *_data)
    {
        ResultType* toReturn = new ResultType();
        toReturn->_init(_h, _w, _stride, false, false);
        toReturn->data = _data;
        toReturn->flags = EXTERNAL_DATA_BUFFER;
        return toReturn;
    }


    /**
     * The element getter.
     *
     * A common practice in this project is for the y coordinate to come first
     **/
    inline ElementType &element(const IndexType y, const IndexType x)
    {
        return data[y * stride + x];
    }

    inline const ElementType &element(const IndexType y, const IndexType x) const
    {
        return data[y * stride + x];
    }

    /**
     * The element getter
     **/
    inline ElementType &element(const Vector2d<IndexType> &p)
    {
        return element(p.y(), p.x());
    }

    inline const ElementType &element(const Vector2d<IndexType> &p) const
    {
        return element(p.y(), p.x());
    }

    /**
     *  Checks if coordinate lies within the buffer area
     **/
    inline bool isValidCoord(const IndexType y,const  IndexType x) const
    {
        return (x >= 0) && (x < w) && (y >= 0) && (y < h);
    }

    /**
     *  Checks if coordinate lies within the buffer area
     **/
    inline bool isValidCoord(const Vector2d<IndexType> &p) const
    {
        return isValidCoord(p.y(), p.x());
    }

    /**
     *   Safe way to set element. Slow, however
     **/
    inline void setElement(const IndexType y, const IndexType x, const ElementType &e)
    {
        if (isValidCoord(y, x)) {
            element(y, x) = e;
        }
    }

    /**
     * Checks if this buffer has the same size as the other
     */
    inline bool hasSameSize (const  AbstractBuffer<ElementType, IndexType> *other) const
    {
        return (this->h == other->h) && (this->w == other->w);
    }

    /**
     * Checks if this buffer has the same size as the parameters given
     */
    inline bool hasSameSize (const IndexType &otherH, const IndexType &otherW) const
    {
        return (this->h == otherH) && (this->w == otherW);
    }

    /**
     *  Height getter
     **/
    inline IndexType getH() const
    {
        return h;
    }

    /**
     *  Stride getter
     **/
    inline IndexType getStride() const
    {
        return stride;
    }

    /**
     *  Width getter
     **/
    inline IndexType getW() const
    {
        return w;
    }

    /**
     *  Buffer size getter
     **/
    inline Vector2d<IndexType> getSize() const
    {
        return Vector2d<IndexType>(w, h);
    }

    /**
     *  Checking for successfull allocation
     **/
    inline bool isAllocated() const
    {
        return data != NULL;
    }

    inline int numElements() const
    {
        return h * stride;
    }

    /**
     *  Calculate image buffer size in bytes
     **/
    inline size_t sizeInBytes() const
    {
        return this->numElements() * sizeof(ElementType);
    }

    /**
     *  Calculate memory for all the structures in the object
     **/
    inline size_t memoryFootprint() const
    {
        return this->memoryBlock.getTotalObjectSize(this->sizeInBytes(), DATA_ALIGN_GRANULARITY) + sizeof(this);
    }


    /**
     *  This function fills the rectangle inside the buffer with the value.
     *
     *  You could use this to clean part of the buffer
     **/
    void fillRectangleWith(IndexType y, IndexType x, IndexType rectH, IndexType rectW, const ElementType &value)
    {
        CORE_ASSERT_TRUE_P(x >= 0 && y >= 0 && x < w && y < h, ("[%d:%d] origin is not inside the buffer [%dx%d]", x, y, w, h));
        CORE_ASSERT_TRUE_P(x + rectW >= 0 && y + rectH >= 0 &&
                           x + rectW <= w && y + rectH <= h, ("[%d:%d] right lower corner is not inside the buffer [%dx%d]", x + rectW, y + rectH, w, h));

        for (IndexType i = y; i < y + rectH; i++)
        {
            ElementType *line = &(element(i, x));
            for (IndexType j = 0; j < rectW; j++)
            {
                if (TRIVIALLY_COPY_CONSTRUCTIBLE && TRIVIALLY_DESTRUCTABLE)
                    *line++ = value;
                else
                {
                    line->~ElementType();
                    new (line++) ElementType(value);
                }
            }
        }
    }

    /**
     *  This function fills the whole buffer with the value.
     *
     *  You could use this to clean the buffer
     **/
    void fillWith(const ElementType &value)
    {
        fillRectangleWith(0, 0, h, w, value);
    }

    /**
     *  This function fills the whole buffer with the values from the given array
     *
     *  TODO: this function doesn't call the copy constructor. I believe it should.
     **/
    void fillWithRaw(ElementType *_data)
    {
       if (this->w == this->stride)
       {
           _copy(data, _data, w * h);
           return;
       }
       _copy(data, _data, h, w, stride, w);
    }

    /**
    *  This function fills the whole buffer with the values from the other buffer
    *
    *  TODO: this function doesn't call the copy constructor. I believe it should.
    **/
    inline void fillWith(const AbstractBuffer &other)
    {
        int copyH = CORE_MIN(this->h, other.h);
        int copyW = CORE_MIN(this->w, other.w);

        /* If buffers have same horizontal geometry use fast method*/
        if (TRIVIALLY_COPY_CONSTRUCTIBLE)
        {
               if (other.stride == this->stride && other.w == this->w)
            {
                memcpy(this->data, other.data, sizeof(ElementType) * copyH * stride);
                return;
            }
            for (int i = 0; i < copyH; i++)
            {
                memcpy(&this->element(i, 0), &other.element(i, 0), sizeof(ElementType) * copyW);
            }
            return;
        }
        _copy(data, other.data, copyH, copyW, stride, other.stride);
    }

    /**
     *  You could use this to clean the buffer
     *
     *  \note you should be very careful passing variables of the right type
     **/

    void fillWithArgs(const ElementType value, va_list marker)
    {
        this->element(0, 0) = value;
        for (IndexType i = 0; i < h; i++)
            for (IndexType j = 0; j < w; j++)
                if (i != 0 || j != 0)
                    this->element(i,j) = va_arg(marker, ElementType);
    }

    void fillWithArgs(const ElementType value, ...)
    {
        va_list  marker;
        va_start(marker, value);
        fillWithArgs(value, marker);
        va_end(marker);
    }

    void fillLineWithArgs(IndexType line, const ElementType value, ...)
    {
       va_list marker;
       va_start(marker, value);

       this->element(line, 0) = value;
       for (IndexType j = 1; j < w; j++)
           this->element(line,j) = va_arg(marker, ElementType);
       va_end(marker);
    }

    /** Fills the buffer by random values within the given range */
    void fillWithRands(ElementType valueMax /*= ElementType::max()*/, ElementType valueMin = ElementType(0))
    {
        srand(rand());
        for (IndexType i = 0; i < h; i++)
            for (IndexType j = 0; j < w; j++)
                this->element(i,j) = (ElementType)randRanged(valueMax, valueMin);
    }

    /**
     * Constructor that copies an area
     */
    AbstractBuffer(AbstractBuffer *src, IndexType x1, IndexType y1, IndexType x2, IndexType y2)
    {
        SYNC_PRINT(("Internal error with input [%d x %d] (%d, %d) -> (%d, %d)\n", src->w, src->h, x1, y1, x2, y2));

        CORE_ASSERT_TRUE_P(src != NULL, ("AbstractBuffer.ctor got src == NULL"));
        IndexType tmp;
        if (x1 > x2) { tmp = x1; x1 = x2; x2 = tmp; }
        if (y1 > y2) { tmp = y1; y1 = y2; y2 = tmp; }
        if (x1 < 0)  { x2 += (-x1); x1 = 0; }
        if (y1 < 0)  { y2 += (-y1); y1 = 0; }

        if (x2 >= src->w) { x1 -= (x2 - (src->w - 1)); x2 = src->w - 1; }
        if (y2 >= src->h) { y1 -= (y2 - (src->h - 1)); y2 = src->h - 1; }

        _init(y2 - y1, x2 - x1);

        CORE_ASSERT_TRUE_P((x1 > 0) && (y1 > 0) && (x2 > x1) && (y2 > y1),
            ("Internal error with input [%d x %d] (%d, %d) -> (%d, %d)", src->w, src->h, x1, y1, x2, y2));
        CORE_ASSERT_TRUE_P((src->w > x2) && (src->h > y2),
            ("Internal error with input [%d x %d] (%d, %d) -> (%d, %d)", src->w, src->h, x1, y1, x2, y2));

        if (TRIVIALLY_COPY_CONSTRUCTIBLE)
        {
            for (IndexType i = 0; i < h; i++)
            {
                memcpy(&element(i, 0), &(src->element(y1 + i, x1)), sizeof(ElementType) * w);
            }
        }
        else
        {
            for (IndexType i = 0; i < h; ++i)
            {
                _copy(data + i * stride, &src->element(y1 + i, x1), w);
            }
        }
    }


    /*AbstractBufferIterator<ElementType> *getIterator()
    {
        return BufferIterator<ElementType, IndexType>(this);
    };*/

    /*
     * Non-maximum supression with avg. complexity < 2.4 ops / pixel and worst-case complexity < 4-4/(w+1) ops / pixel
     * (naive implementation costs 1.3 + 2log(w) avg. and n^2 worst-case)
     *
     * Check out http://homes.esat.kuleuven.be/~konijn/publications/2006/eth_biwi_00446.pdf for details
     *
     * \param windowHalf - half of maximum-search window
     * \param threshold  - threshold for maximum-candidates
     * \param maximas    - vector of local-maximum points (output)
     * \param skip       - skip border
     */
    void nonMaximumSupression(const IndexType &windowHalf, const ElementType &threshold, std::vector<std::pair<IndexType, IndexType>> &maximas, const IndexType &skip = 0)
    {
        maximas.clear();
        IndexType w = windowHalf;
        IndexType s = skip;

        for (auto i = s + w; i + w + s < this->w; i += w + 1)
        {
            for (auto j = s + w; j + w + s < this->h; j += w + 1)
            {
                auto mi = i, mj = j;
                auto mx = this->element(j, i);

                for (auto i2 = i; i2 < i + w + 1; ++i2)
                {
                    for (auto j2 = j; j2 < j + w + 1; ++j2)
                    {
                        if (this->element(j2, i2) > mx)
                        {
                            mx = this->element(j2, i2);
                            mi = i2;
                            mj = j2;
                        }
                    }
                }

                // Now we are sure, that A[mj, mi] is best in [j; j+w]x[i; i+w], need to check if it is best in [mj-w; mj+w]x[mi-w; mi+w]
                bool failed = mx < threshold;

                auto top = mj - w, bottom = std::min(mj + w + 1, j), left = mi - w, right = mi + w + 1;
#define TRY_NMS(tv, bv, lv, rv) \
                if (!failed) \
                { \
                    top = tv; \
                    bottom = bv; \
                    left = lv; \
                    right = rv; \
                    failed |= nonMaximumSupressionHelper(top, bottom, left, right, mx); \
                }

                TRY_NMS(top, bottom, left, right)
                TRY_NMS(j, std::min(mj + w + 1, j + w + 1), mi - w, std::min(mi + w + 1, i))
                TRY_NMS(top, bottom, i + w + 1, mi + w + 1)
                TRY_NMS(j + w + 1, mj + w + 1, mi - w, mi + w)
#undef TRY_NMS
                if (!failed)
                    maximas.push_back(std::make_pair(mi, mj));
            }
        }
    }

    inline bool nonMaximumSupressionHelper(IndexType &top, IndexType &bottom, IndexType &left, IndexType &right, ElementType &mx)
    {
        for (auto j = top; j < bottom; ++j)
        {
            for (auto i = left; i < right; ++i)
            {
                if (this->element(j, i) > mx)
                    return true;
            }
        }
        return false;
    }



    template<typename ReturnType, typename ConvElementType, typename ConvIndexType>
    void innerCycleDoConvolve(ReturnType *toReturn, AbstractKernel<ConvElementType, ConvIndexType> *kernel, int i)
    {
        for (IndexType j = 0; j < w; j++)
        {
            toReturn->element(i,j) =  kernel->template multiplyAtPoint<ElementType, IndexType>(this, i,j);
        }
    }


    template<typename ReturnType, typename SelfType, typename ConvElementType, typename ConvIndexType>
    class ParallelDoConvolve
    {
        SelfType   *buffer;
        ReturnType *toReturn;
        AbstractKernel<ConvElementType, ConvIndexType> *kernel;
        bool        onlyValid;

    public:
        ParallelDoConvolve(
                ReturnType *_toReturn,
                SelfType * _buffer,
                AbstractKernel<ConvElementType, ConvIndexType> *_kernel,
                bool onlyValid) :
         buffer(_buffer), toReturn(_toReturn), kernel(_kernel), onlyValid(onlyValid)
        {}

        void operator()(const BlockedRange<IndexType>& r) const
        {
            int left =  onlyValid ? kernel->x : 0;
            int right = onlyValid ? buffer->w + kernel->x - kernel->w + 1 : buffer->w;
            if (!onlyValid)
            {
                for (IndexType i = r.begin(); i != r.end(); i++)
                {
                    for (IndexType j = left; j < right; j++)
                    {
                        toReturn->element(i,j) = kernel->template multiplyAtPoint<ElementType, IndexType>(buffer, i, j);
                    }
                }
            }
            else
            {
                for (IndexType i = r.begin(); i != r.end(); i++)
                {
                    for (IndexType j = left; j < right; j++)
                    {
                        toReturn->element(i,j) = kernel->template multiplyAtPoint<ElementType, IndexType, true>(buffer, i, j);
                    }
                }
            }
        }
    };

    /**
     *  This function is used to convolve/cross-correlate a buffer with the kernel.
     *  In general convolution is
     *
     *  \f[
     *  (f * g )(t)
     *      = \int_{-\infty}^{\infty} f(\tau)\, g(t - \tau)\, d\tau
     *      = \int_{-\infty}^{\infty} f(t-\tau)\, g(\tau)\, d\tau
     *  \f]
     *
     *  But this function does more of cross-correlation.
     *
     *
     *
     **/
    template<typename ReturnType, typename ConvElementType, typename ConvIndexType>
    void doConvolve(ReturnType *output, AbstractKernel<ConvElementType, ConvIndexType> *kernel, bool onlyValid = false, bool parallel = true)
    {
        /*TODO: Well we need to process this gracefully */
        if (output->h != h || output->w != w)
            return;

        int top    = onlyValid ? kernel->y : 0;
        int bottom = onlyValid ? h + kernel->y - kernel->h + 1 : h;
        parallelable_for(top, bottom, ParallelDoConvolve<ReturnType, AbstractBuffer<ElementType, IndexType>, ConvElementType, ConvIndexType>(output, this, kernel, onlyValid), parallel);
    }

    template<typename ReturnType, typename ConvElementType, typename ConvIndexType>
    ReturnType* doConvolve(AbstractKernel<ConvElementType, ConvIndexType> *kernel, bool onlyValid = false, bool parallel = true)
    {
        ReturnType *toReturn = new ReturnType(h, w);
        doConvolve(toReturn, kernel, onlyValid, parallel);
        return toReturn;
    }

    template<typename ReturnType, typename ConvElementType, typename ConvIndexType>
    class ParallelDoConvolve1
    {
        ReturnType *toReturn;
        AbstractKernel<ConvElementType, ConvIndexType> *kernel;
        AbstractBuffer<ElementType, IndexType> *buf;

    public:
        ParallelDoConvolve1(
            ReturnType *_toReturn,
            AbstractKernel<ConvElementType, ConvIndexType> *_kernel,
            AbstractBuffer<ElementType, IndexType> *_buf) :
        toReturn(_toReturn),
        kernel(_kernel),
        buf(_buf)
        {}

        void operator()(const BlockedRange<IndexType>& r) const
        {
            IndexType j;

            for (IndexType i = r.begin(); i != r.end(); i++)
            {
                ElementType *line = &buf->element(i, 0);
                for (j = kernel->centerX; j < toReturn->w - kernel->w + kernel->centerX; j++)
                {
                    toReturn->element(i,j) =
                            kernel->template multiplyAtPoint<ElementType, IndexType>(line, buf->stride, i, j);
                    line++;
                }
            }
        }
    };

    template<typename ReturnType, typename ConvElementType, typename ConvIndexType>
    ReturnType* doConvolve1(AbstractKernel<ConvElementType, ConvIndexType> *kernel)
    {
        IndexType i,j;
        ReturnType *toReturn = new ReturnType(h, w);
        parallelable_for(kernel->centerY, h - kernel->h + kernel->centerY,
                ParallelDoConvolve1<ReturnType, ConvElementType, ConvIndexType>(toReturn, kernel, this));
        return toReturn;
    }


    /* Scalar types related methods */
    /**
     *  This method tests for equality
     **/
    bool isEqual(const AbstractBuffer &that) const
    {
        if (that.h != this->h || that.w != this->w)
            return false;
        for (int i = 0; i < this->h; i++)
        {
            const ElementType *thisElemRunner = &(this->element(i, 0));
            const ElementType *thatElemRunner = &(that.element(i, 0));
            for (int j = 0; j < this->w; j++)
            {
                if (*thatElemRunner != *thisElemRunner)
                {
#ifdef ASSERTS
                    printf("Buffers have equal sizes [h=%d w=%d]\n", this->h, this->w);

                    printf("[ 0x");
                    for (size_t k = 0; k < sizeof(ElementType); k++)
                        printf("%02X ", ((uint8_t *)(void*)(thisElemRunner))[k]);
                    printf("] vs [ 0x");
                    for (size_t k = 0; k < sizeof(ElementType); k++)
                        printf("%02X ", ((uint8_t *)(void*)(thatElemRunner))[k]);
                    printf("]\nPos y=%d x=%d\n", i , j);
                    CORE_ASSERT_FAIL_P(("Buffers don't match"));
#endif
                    return false;
                }
                thatElemRunner++;
                thisElemRunner++;
            }
        }
        return true;
    }

template<class PrintType = ElementType>
    bool isEqualTrace(const AbstractBuffer &that, ElementType tolerance = 0) const
    {
        if (that.h != this->h || that.w != this->w)
            return false;
        int diffs = 0;
        for (int i = 0; i < this->h; i++)
        {
            const ElementType *thisElemRunner = &(this->element(i, 0));
            const ElementType *thatElemRunner = &(that.element(i, 0));
            for (int j = 0; j < this->w; j++)
            {
                bool testpass = true;
                if (tolerance <= 0) {
                    testpass = (*thatElemRunner == *thisElemRunner);
                } else {
                    ElementType diff = (*thatElemRunner > *thisElemRunner) ? *thatElemRunner - *thisElemRunner : *thisElemRunner - *thatElemRunner;
                    testpass = (CORE_ABS(diff) <= tolerance);
                }

                if (!testpass)
                {
                    cout << "Differ Pos y="<< i << " x=" << j << " ";
                    cout << "Fst = " <<  (PrintType)(*thisElemRunner) << " sec= " << (PrintType)(*thatElemRunner) << endl;
                    diffs++;
                }
                thatElemRunner++;
                thisElemRunner++;
            }
        }
        cout << "Totally " << diffs << " diffs" << endl;
        return diffs == 0;
    }

    /**
     *  This method performs the "oper" operation between this and that buffers and
     *  puts result to this buffer
     *
     *  \param that - the AbstractBuffer to be processed with this to this
     *  \param oper - operation to be performed
     **/

template<typename operation, typename OtherBuffer>
    void binaryOperationInPlace(const OtherBuffer &that, const operation &oper = operation())
    {
        IndexType h = CORE_MIN(this->h, that.h);
        IndexType w = CORE_MIN(this->w, that.w);
        for (IndexType i = 0; i < h; i++)
        {
            ElementType *thisElemRunner = &(this->element(i, 0));
            const typename OtherBuffer::InternalElementType *thatElemRunner = &(that.element(i, 0));
            for (IndexType j = 0; j < w; j++)
            {
                /* Writing like this just to emphasize what is actually called */
                *thisElemRunner = oper.operator()(*thisElemRunner, *thatElemRunner);
                thisElemRunner++;
                thatElemRunner++;
            }
        }
    }

/*template<typename operation>
    void mapOperationElementwize(const ElementType &that)
    {
        for (IndexType i = 0; i < h; i++)
        {
            ElementType *thisElemRunner = &(this->element(i, 0));
            for (IndexType j = 0; j < w; j++)
            {
                *thisElemRunner = operation().operator()(*thisElemRunner, that);
                thisElemRunner++;
            }
        }
    }*/

template<typename operation>
    void mapOperationElementwize (const operation &map)
    {
        for (IndexType i = 0; i < h; i++)
        {
            ElementType *thisElementRunner = &(this->element(i, 0));
            for (IndexType j = 0; j < w; j++)
            {
                *thisElementRunner = map.operator()(*thisElementRunner);
                thisElementRunner++;
            }
        }
    }


    // XXX: overload added in order to use lambdas supplied directly as arguments
template<typename operation>
    void touchOperationElementwize (const operation &map)
    {
        for (IndexType i = 0; i < h; i++)
        {
            ElementType *thisElemRunner = &(this->element(i,0));
            for (IndexType j = 0; j < w; j++)
            {
                map.operator()(i, j, *thisElemRunner);
                thisElemRunner++;
            }
        }
    }

template<typename operation>
    void touchOperationElementwize (operation &map)
    {
        for (IndexType i = 0; i < h; i++)
        {
            ElementType *thisElemRunner = &(this->element(i, 0));
            for (IndexType j = 0; j < w; j++)
            {
                map.operator()(i, j, *thisElemRunner);
                thisElemRunner++;
            }
        }
    }

friend ostream & operator <<(ostream &out, const AbstractBuffer &buffer)
    {
        // typedef AbstractBuffer::InternalIndexType IndexType;
        //
        for (IndexType i = 0; i < buffer.h; i++)
        {
           for (IndexType j = 0; j < buffer.w; j++)
           {
               out << buffer.element(i, j) << " ";
           }
           out << endl;
        }
        return out;
    }

    template<typename T=ElementType, typename I=IndexType>
    typename std::enable_if<AbstractBuffer<T, I>::TRIVIALLY_COPY_CONSTRUCTIBLE, bool>::type
    dump(ostream& s, bool binaryMode) const
    {
        AbstractBufferParams::dump(s, sizeof(ElementType), binaryMode);
        if (binaryMode) {
            s.write((char *)data, sizeInBytes());
        }
        return !s.bad();
    }

    template<typename T=ElementType, typename I=IndexType>
    typename std::enable_if<AbstractBuffer<T, I>::TRIVIALLY_COPY_CONSTRUCTIBLE, bool>::type
    load(istream& s, bool binaryMode)
    {
        if (!AbstractBufferParams::load(s, sizeof(ElementType), binaryMode))
            return false;

        _init(getH(), getW(), getStride(), false);

        if (!binaryMode)
            return true;                                    // loading in text mode is outside

        s.read((char *)data, sizeInBytes());
        return !s.bad();
    }

    /**
     *  Please check that you really don't want to invoke the copy constructor
     *
     **/
template<typename OtherType>
    OtherType *mirrorVerticalDump()
    {
        OtherType *toReturn = new OtherType(this->h, this->w);
        for (IndexType i = 0; i < h; i++)
        {
            _copy(&(toReturn->element(h - 1 - i, 0)), &(this->element(i, 0)), w);
        }
        return toReturn;
    }

  /** Mirrors current buffer vertically */
  void mirrorVertical()
  {
      for (IndexType i = 0; i < h / 2; i++)
      {
          _copy(&(this->element(h - 1 - i, 0)), &(this->element(i, 0)), w);
      }
  }

  /** Rotates current buffer 180 degrees */
  void rotate180()
  {
      for (IndexType i = 0; i < h / 2; i++)
      {
            ElementType *thisElemRunner = &(this->element(i, 0));
            ElementType *thatElemRunner = &(this->element(h - 1 - i, w - 1));
            for (IndexType j = 0; j < w; j++)
            {
                ElementType tmp = *thisElemRunner;
                *thisElemRunner = *thatElemRunner;
                *thatElemRunner = tmp;

                thatElemRunner--;
                thisElemRunner++;
            }
      }
  }

template<typename OtherType>
    OtherType *mirrorVertical()
    {
        OtherType *toReturn = new OtherType(this->h, this->w);
        for (IndexType i = 0; i < h; i++)
        {
            ElementType *thisElemRunner = &(this->element(i, 0));
            typename OtherType::InternalElementType *thatElemRunner = &(toReturn->element(h - 1 - i, 0));
            for (IndexType j = 0; j < w; j++)
            {
                *thatElemRunner = *thisElemRunner;
                thatElemRunner++;
                thisElemRunner++;
            }
        }
        return toReturn;
    }


protected:

    /**
     * The memory block that holds and manages the data
     **/
    AMBReference   memoryBlock;
  //MemoryBlockRef memoryBlock;

    BufferType     flags;

private:

    void    setH     (int _h)
    {
        h = _h;
    }

    void    setW     (int _w)
    {
        w = _w;
    }

    void    setStride(int _stride)
    {
        stride = _stride;
    }

    /**
     *  This is a helper method for constructing.
     *  You should ensure it will never be called from anywhere except the constructing methods.
     *
     *  \param h
     *      Height of the buffer
     *  \param w
     *      Width of the buffer
     *  \param stride
     *      Buffer stride
     *  \param shouldInit
     *      true if the buffer should be filled with 0s
     *      false otherwize
     *  \param shouldAlloc
     *      true if memory should be allocated for the buffer.
     *      If you choose not to allocate - you could make the buffer base on the existing memory.
     *      Do not forget to somehow change constructor then
     *
     **/
    int _init(IndexType h, IndexType w, IndexType stride, bool shouldInit = true, bool shouldAlloc = true)
    {
        this->flags = NORMAL_BUFFER;
        this->setH(h);
        this->setW(w);
        this->setStride(stride != STRIDE_AUTO ? stride : _getStride(w));

        if (shouldAlloc)
        {
            size_t allocatedSize = this->sizeInBytes();
            auto wa = this->w, ha = this->h, sa = this->stride;
            memoryBlock =
                TRIVIALLY_DESTRUCTABLE ?
                    AMBReference(allocatedSize, DATA_ALIGN_GRANULARITY + 1) :
                    AMBReference(allocatedSize, DATA_ALIGN_GRANULARITY + 1, [=](AlignedMemoryBlock* b)
                    {
                        _del((ElementType *)b->getAlignedStart(), ha, wa, sa);
                        delete b;
                    });
            this->data = (ElementType *)memoryBlock.getAlignedStart();

            if (shouldInit || !TRIVIALLY_DEFAULT_CONSTRUCTIBLE) {
                CORE_CLEAR_MEMORY(this->data, allocatedSize);
                _initArray(data, h, w, sa);
            }
#if 0
#ifdef ASSERTS
            /**
             *  Mark the margin zone that is used for alignment with a distinct pattern.
             **/
            for (IndexType i = 0; i < this->getH(); i++)
            {
                uint8_t *start = reinterpret_cast<uint8_t *>(&this->element(i, this->w));
                uint8_t *end   = reinterpret_cast<uint8_t *>(&this->element(i + 1,   0));

                for (unsigned j = 0; start + j < end; j++)
                    start[j] = (uint8_t)(j * 2);
            }
#endif
#endif
        }
        else
        {
          //this->_allocatedSize = 0;
            this->data = NULL;
        }

        atomic_inc_and_fetch(&bufferCount);
        return 0;
    }

    int _getStride(int w)
    {
        /**
         *  Aligning w
         *  if w has ones in the part masked by DATA_STRIDE_GRANULARITY we should increment stride
         *
         *  TODO : should use pointer arithmetics instead
         * */
        int strideGuess = w;
        size_t lineLen  = strideGuess * sizeof(ElementType);
        while (lineLen & DATA_ALIGN_GRANULARITY)
        {
           strideGuess++;
           lineLen += sizeof(ElementType);
        }
        return strideGuess;
    }

    int _init(IndexType h, IndexType w, bool shouldInit = true, bool shouldAlloc = true)
    {
        return _init(h, w, _getStride(w), shouldInit, shouldAlloc);
    }

    static void _copy(ElementType* dst, ElementType* src, IndexType cnt)
    {
        /*
         * Using traits swithc to memcpy if possible
         */
        if (TRIVIALLY_COPY_CONSTRUCTIBLE)
        {
            memcpy(dst, src, cnt * sizeof(ElementType));
        }
        else
        {
            for (IndexType i = 0; i < cnt; ++i)
            {
                if (!TRIVIALLY_DESTRUCTABLE)
                    dst[i].~ElementType();
                new (dst + i) ElementType(src[i]);
            }
        }
    }

    static void _copy(ElementType* dst, ElementType* src, IndexType h, IndexType w, IndexType strideDst, IndexType strideSrc)
    {
        for (IndexType i = 0; i < h; ++i)
            _copy(dst + i * strideDst, src + i * strideSrc, w);
    }
    static void _del(ElementType* ptr, IndexType h, IndexType w, IndexType stride)
    {
        CORE_UNUSED(ptr);
        CORE_UNUSED(h);
        CORE_UNUSED(w);
        CORE_UNUSED(stride);
        /*
         * Using traits switch between "do nothing" (memory is cleared by memoryBlock)
         * and "destruct all elements"
         */
        if (!TRIVIALLY_DESTRUCTABLE)
        {
            for (IndexType i = 0; i < h; ++i)
                for (IndexType j = 0; j < w; ++j)
                    ptr[i * stride + j].~ElementType();
        }
    }

    static void _initArray(ElementType* ptr, IndexType h, IndexType w, IndexType stride)
    {
        /*
         * This initalizes array with default constructor
         */
        if (!TRIVIALLY_DEFAULT_CONSTRUCTIBLE)
        {
            for (IndexType i = 0; i < h; ++i)
                for (IndexType j = 0; j < w; ++j)
                    new (ptr + i * stride + j) ElementType();
        }
        else
        {
            memset(ptr, 0, sizeof(ElementType) * stride * h);
        }
    }

    static void _initArray(const ElementType& el, ElementType* ptr, IndexType h, IndexType w, IndexType stride)
    {
        for (IndexType i = 0; i < h; ++i)
        {
            for (IndexType j = 0; j < w; ++j)
            {
                new (ptr + i * stride + j) ElementType(el);
            }
        }
    }

}; // AbstractBuffer

/**
 *  Counter for memory profiling
 **/
template<typename ElementType, typename IndexType>
int AbstractBuffer<ElementType, IndexType>::bufferCount = 0;

} //namespace corecvs

#endif // ABSTRACTBUFFER_H
