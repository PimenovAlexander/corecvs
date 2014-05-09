#pragma once
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

#include <string>
#include <functional>

#include "global.h"

#include "vector2d.h"
#include "memory/memoryBlock.h"
#include "tbbWrapper.h"
#include "mathUtils.h"                  // randRanged

namespace corecvs {

using std::string;
using std::cout;
using std::endl;
using std::min;
using std::max;

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
 * This class holds the mapping function from coordinate domain
 * to the other coordinate domain.
 *
 * We probably should use only static poly-morphism.
 * So map should be made non-virtual
 **/
template<typename InputIndexType, typename OutputIndexType>
class DeformMap
{
public:

    typedef Vector2d<InputIndexType> InputPoint;
    typedef Vector2d<OutputIndexType> OutputPoint;


    inline OutputPoint map(const InputIndexType & /*y*/, const InputIndexType & /*x*/) {}

    //inline OutputPoint map(const InputPoint &point)
    //{
    //    return map (point.y, point.x);
    //}

    virtual ~DeformMap() {}
};

/**
 * These are useful methods to serialize integer types not depending of the current endianess
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

    enum BufferType {
        NORMAL_BUFFER,
        VIEW_BUFFER,
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
    static int bufferCount;

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
      //, _allocatedSize(0)
        , flags(EMPTY_BUFFER)
    {
        bufferCount++;
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
        memcpy(this->data, that.data, that.sizeInBytes());
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
       fillWithRaw(_data);
    }

    AbstractBuffer(Vector2d<IndexType> size, ElementType *_data)
    {
       _init(size.y(), size.x(), false);
       fillWithRaw(_data);
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

    /**
     * Destructor. Main buffer will be deleted with MemoryBlockRef smart pointer destruction
     *
     * If the buffer is created on top of the other, the best way is
     * to first set data to NULL
     * \todo TODO: Should this be virtual? Usually we do not cast pointers to base type.
     **/
    ~AbstractBuffer()
    {
        if (data != NULL)
        {
            /* Should I really do this? Will this be optimized out for basic types */
            IndexType i;
            IndexType elementNumber = this->numElements();
            for (i = 0; i < elementNumber; i++)
            {
                data[i].~ElementType();
            }
        }
        this->data = NULL;
        bufferCount--;                                      // this must be always as it's incremented at each ctor
    }


template<typename ResultType>
    ResultType *createView(const IndexType y, const IndexType x, const IndexType h, const IndexType w)
    {
        // This is the only legitimate place to use default constructor
        ResultType *toReturn = new ResultType();
        toReturn->flags      = VIEW_BUFFER;
        toReturn->h          = h;
        toReturn->w          = w;
        toReturn->stride     = this->stride;
        toReturn->data       = &(this->element(y, x));
        /**
         * Prevent original buffer from being deleted
         * MemoryBlock magically counts references
         **/
        toReturn->memoryBlock = memoryBlock;
      //toReturn->_allocatedSize = 0;
        return toReturn;
    }


template<typename ResultType>
    ResultType *createView()
    {
        return this->createView<ResultType>(0, 0, this->h, this->w);
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

    /**
     *  Calculate image buffer size in bytes
     **/
    inline size_t sizeInBytes() const
    {
      //return _allocatedSize;
        return this->numElements() * sizeof(ElementType);
    }

    /**
     *  This function fills the rectangle inside the buffer with the value.
     *
     *  You could use this to clean part of the buffer
     **/
    void fillRectangleWith(IndexType y, IndexType x, IndexType rectH, IndexType rectW, const ElementType &value)
    {
        ASSERT_TRUE_P(x >= 0 && y >= 0 && x < w && y < h, ("[%d:%d] origin is not inside the buffer [%dx%d]", x, y, w, h));
        ASSERT_TRUE_P(x + rectW >= 0 && y + rectH >= 0 &&
                      x + rectW <= w && y + rectH <= h, ("[%d:%d] right lower corner is not inside the buffer [%dx%d]", x + rectW, y + rectH, w, h));

        for (IndexType i = y; i < y + rectH; i++)
        {
            ElementType *line = &(element(i, x));
            for (IndexType j = 0; j < rectW; j++)
            {
                *line++ = value;
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
        /*for (IndexType i = 0; i < h; i++)
            for (IndexType j = 0; j < w; j++)
                this->element(i,j) = value; */
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
           memcpy(this->data, _data, this->sizeInBytes());
           return;
       }
       size_t rawLength = sizeof(ElementType) * this->w;
       for (int i = 0; i < (int)this->h; i++)
       {
           memcpy(&this->element(i, 0), &_data[i * this->w], rawLength);
       }
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
        if (other.stride == this->stride && other.w == this->w) {
            memcpy(this->data, other.data, sizeof(ElementType) * copyH * stride);
            return;
        }
        /* Otherwise copy line by line */
        for (int i = 0; i < copyH; i++) {
            memcpy(&this->element(i, 0), &other.element(i, 0), sizeof(ElementType) * copyW);
        }
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
        printf("Internal error with input [%d x %d] (%d, %d) -> (%d, %d)\n", src->w, src->h, x1, y1, x2, y2);
        fflush(stdout);

        ASSERT_TRUE_P(src != NULL, ("src == NULL"));
        IndexType tmp;
        if (x1 > x2) { tmp = x1; x1 = x2; x2 = tmp; }
        if (y1 > y2) { tmp = y1; y1 = y2; y2 = tmp; }
        if (x1 < 0)  { x2 += (-x1); x1 = 0; }
        if (y1 < 0)  { y2 += (-y1); y1 = 0; }

        if (x2 >= src->w) { x1 -= (x2 - (src->w - 1)); x2 = src->w - 1; }
        if (y2 >= src->h) { y1 -= (y2 - (src->h - 1)); y2 = src->h - 1; }

        _init(y2 - y1, x2 - x1);

        ASSERT_TRUE_P((x1 > 0) && (y1 > 0) && (x2 > x1) && (y2 > y1),
                ("Internal error with input [%d x %d] (%d, %d) -> (%d, %d)", src->w, src->h, x1, y1, x2, y2));
        ASSERT_TRUE_P((src->w > x2) && (src->h > y2),
                ("Internal error with input [%d x %d] (%d, %d) -> (%d, %d)", src->w, src->h, x1, y1, x2, y2));

        for (IndexType i = 0; i < h; i++) {
            memcpy(&element(i, 0), &(src->element(y1 + i, x1)), sizeof(ElementType) * w);
        }
    }


    /*AbstractBufferIterator<ElementType> *getIterator()
    {
        return BufferIterator<ElementType, IndexType>(this);
    };*/

    template<typename ReturnType, typename ConvElementType, typename ConvIndexType>
    void innerCycleDoConvolve(ReturnType *toReturn, AbstractKernel<ConvElementType, ConvIndexType> *kernel, int i)
    {
        for (IndexType j = 0; j < w; j++)
        {
            toReturn->element(i,j) =  kernel->template multiplyAtPoint<ElementType, IndexType>(this, i,j);
        }
    }


    template<typename ReturnType, typename ConvElementType, typename ConvIndexType>
    class ParallelDoConvolve
    {
        ReturnType *toReturn;
        AbstractKernel<ConvElementType, ConvIndexType> *kernel;
    public:
        ParallelDoConvolve(
                ReturnType *_toReturn,
                AbstractKernel<ConvElementType, ConvIndexType> *_kernel) :
        toReturn(_toReturn), kernel(_kernel)
        {}

        void operator()( const BlockedRange<IndexType>& r ) const
        {
            for (IndexType i = r.begin(); i != r.end(); i++)
            {
                for (IndexType j = 0; j < toReturn->w; j++)
                {
                    toReturn->element(i,j) = kernel->template multiplyAtPoint<ElementType, IndexType>(this, i,j);
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
    ReturnType* doConvolve(AbstractKernel<ConvElementType, ConvIndexType> *kernel)
    {
        IndexType i;
        ReturnType *toReturn = new ReturnType(h, w);
        parallelable_for(0, h, ParallelDoConvolve<ReturnType, ConvElementType, ConvIndexType>(toReturn, kernel, this));

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

        void operator()( const BlockedRange<IndexType>& r ) const
        {
            IndexType j;

            for (IndexType i = r.begin(); i != r.end(); i++)
            {
                ElementType *line = &buf->element(i,0);
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
            const ElementType *thisElemRunner = &(this->element(i,0));
            const ElementType *thatElemRunner = &(that.element(i,0));
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
                    ASSERT_FAIL_P(("Buffers don't match"));
#endif
                    return false;
                }
                thatElemRunner++;
                thisElemRunner++;
            }
        }
        return true;
    }

    bool isEqualTrace(const AbstractBuffer &that) const
    {
        if (that.h != this->h || that.w != this->w)
            return false;
        int diffs = 0;
        for (int i = 0; i < this->h; i++)
        {
            const ElementType *thisElemRunner = &(this->element(i,0));
            const ElementType *thatElemRunner = &(that.element(i,0));
            for (int j = 0; j < this->w; j++)
            {
                if (*thatElemRunner != *thisElemRunner)
                {
                    cout << "Differ Pos y="<< i << " x=" << j << " ";
                    cout << "Fst = " <<  *thisElemRunner << " sec= " << *thatElemRunner << endl;
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

template<typename operation>
    void binaryOperationInPlace(const AbstractBuffer &that, const operation &oper = operation())
    {
        IndexType h = min(this->h, that.h);
        IndexType w = min(this->w, that.w);
        for (IndexType i = 0; i < h; i++)
        {
            ElementType *thisElemRunner = &(this->element(i,0));
            const ElementType *thatElemRunner = &(that.element(i,0));
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
            ElementType *thisElemRunner = &(this->element(i,0));
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
            ElementType *thisElementRunner = &(this->element(i,0));
            for (IndexType j = 0; j < w; j++)
            {
                *thisElementRunner = map.operator()(*thisElementRunner);
                thisElementRunner++;
            }
        }
    }

template<typename operation>
    void touchOperationElementwize (operation &map)
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

    bool dump(ostream& s, bool binaryMode) const
    {
        AbstractBufferParams::dump(s, sizeof(ElementType), binaryMode);
        if (binaryMode) {
            s.write((char *)data, sizeInBytes());
        }
        return !s.bad();
    }

    bool load(istream& s, bool binaryMode)
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
            memcpy(&(toReturn->element(h - 1 - i, 0)), &(this->element(i, 0)), w * sizeof(ElementType));
        }
        return toReturn;
    }

  /** Mirrors current buffer vertically */
  void mirrorVertical()
  {
      for (IndexType i = 0; i < h / 2; i++)
      {
          memcpy(&(this->element(h - 1 - i, 0)), &(this->element(i, 0)), w * sizeof(ElementType));
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
            ElementType *thisElemRunner = &(this->element(i,0));
            typename OtherType::InternalElementType *thatElemRunner = &(toReturn->element(h - 1 - i,0));
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
     * The beginning of the allocated memory
     **/
    //size_t _allocatedSize;

    MemoryBlockRef memoryBlock;

    BufferType flags;

private:



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

        bufferCount++;

        if (shouldAlloc)
        {
            size_t allocatedSize = this->sizeInBytes();
            this->memoryBlock.allocate(allocatedSize, DATA_ALIGN_GRANULARITY);
            this->data = new(this->memoryBlock.getAlignedStart(DATA_ALIGN_GRANULARITY)) ElementType[this->numElements()];

            if (shouldInit) {
                CORE_CLEAR_MEMORY(this->data, allocatedSize);
            }
        }
        else
        {
          //this->_allocatedSize = 0;
            this->data = NULL;
        }

//#ifdef ASSERTS
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
//#endif

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

}; // AbstractBuffer

/**
 *  Counter for memory profiling
 **/
template<typename ElementType, typename IndexType>
int AbstractBuffer<ElementType, IndexType>::bufferCount = 0;

} //namespace corecvs
