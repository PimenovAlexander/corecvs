#include "buffers/runtimeTypeBuffer.h"

#include <iomanip>

namespace corecvs {

void RuntimeTypeBuffer::load(std::istream &is)
{
    is >> (*this);
}

void RuntimeTypeBuffer::save(std::ostream &os) const
{
    os << (*this);
}

}

template< typename T, typename U >
T average( const T* data, const size_t rowStep, const int cols, const int rows )
{
    U sum = 0;
    for ( int j = 0; j < rows; j++, data += rowStep )
    {
        for ( int i = 0; i < cols; i++ )
            sum += data[ i ];
    }

    return ( T )( sum / ( cols * rows ) );
}

template< typename T, typename U, int factor >
T average( const T* data, const size_t rowStep )
{
    U sum = 0;
    for ( int j = 0; j < factor; j++, data += rowStep )
    {
        for ( int i = 0; i < factor; i++ )
            sum += data[ i ];
    }

    return ( T )( sum / ( factor * factor ) );
}

template< typename T, typename U, int factor >
void convert( T* newData, const T* data, const size_t newCols, const size_t newRows, const size_t cols, const size_t rows )
{
    int remCols = cols % factor;
    int remRows = rows % factor;

    if ( !remCols )
        remCols = factor;

    if ( !remRows )
        remRows = factor;

    const size_t newRowStep = cols * factor;
    for ( size_t j = 0; j < newRows - 1; j++, newData += newCols, data += newRowStep )
    {
        size_t i = 0;
        // all elements in a row, except the last
        for ( ; i < newCols - 1; i++ )
            newData[ i ] = average< T, U, factor >( data + i * factor, cols );

        // last element in each row
        newData[ i ] = average< T, U >( data + i * factor, cols, remCols, factor );
    }

    // elements in last row
    size_t i = 0;
    for ( ; i < newCols - 1; i++, data += factor )
        newData[ i ] = average< T, U >( data, cols, factor, remRows );
    
    // last element in last row
    newData[ i ] = average< T, U >( data, cols, remCols, remRows );
}

void corecvs::RuntimeTypeBuffer::downsample( int factor )
{
    switch ( factor )
    {
    case 1:
        return;
    case 2:
    case 4:
    case 8:
        break;
    default:
        CORE_ASSERT_FAIL_P( ( "RuntimeTypeBuffer::downsample(%d): unsupported factor, must be 1, 2, 4 or 8", factor ) );
        return;
    }

    size_t newCols = cols / factor;
    size_t newRows = rows / factor;
    
    if ( !newCols )
        newCols = 1;

    if ( !newRows )
        newRows = 1;

    if ( type == corecvs::BufferType::U8 )
    {
        uint8_t* newData = ( uint8_t* )malloc( sizeof( uint8_t ) * newCols * newRows );
        switch ( factor )
        {
        case 2:
            convert< uint8_t, uint, 2 >( newData, data, newCols, newRows, cols, rows );
            break;
        case 4:
            convert< uint8_t, uint, 4 >( newData, data, newCols, newRows, cols, rows );
            break;
        case 8:
            convert< uint8_t, uint, 8 >( newData, data, newCols, newRows, cols, rows );
            break;
        }

        free( data );
        data = newData;
    }

    if ( type == corecvs::BufferType::F32 )
    {
        float* newData = ( float* )malloc( sizeof( float ) * newCols * newRows );
        switch ( factor )
        {
        case 2:
            convert< float, float, 2 >( newData, ( float* )data, newCols, newRows, cols, rows );
            break;
        case 4:
            convert< float, float, 4 >( newData, ( float* )data, newCols, newRows, cols, rows );
            break;
        case 8:
            convert< float, float, 8 >( newData, ( float* )data, newCols, newRows, cols, rows );
            break;
        }

        free( data );
        data = ( uint8_t* )newData;
    }

    cols = newCols;
    rows = newRows;  
}

std::istream& operator>>(std::istream &is, corecvs::RuntimeTypeBuffer &b)
{
    size_t R, C;
    std::string type;
    int buffer_type = corecvs::BufferType::U8;
    is >> R >> C >> type;
    if(type == "F32")
    {
        buffer_type = corecvs::BufferType::F32;
    }

    b = corecvs::RuntimeTypeBuffer(R, C, buffer_type);
    switch(b.type)
    {
        case corecvs::BufferType::U8:
            for(size_t i = 0; i < R; ++i)
                for(size_t j = 0; j < C; ++j)
                {
                    int v;
                    is >> v;
                    b.at<uint8_t>(i, j) = v;
                }
            break;
        case corecvs::BufferType::F32:
            for(size_t i = 0; i < R; ++i)
                for(size_t j = 0; j < C; ++j)
                {
                    is >> b.at<float>(i, j);
                }
            break;
    }
    return is;
}

std::ostream& operator<<(std::ostream &os, const corecvs::RuntimeTypeBuffer &b)
{
    size_t R = b.rows, C = b.cols;
    std::string type = "U8";
    if(b.type == corecvs::BufferType::F32)
    {
        type = "F32";
    }

    os << R << " " << C << " " << type << std::endl;
    switch(b.type)
    {
        case corecvs::BufferType::U8:
            for(size_t i = 0; i < R; ++i)
            {
                for(size_t j = 0; j < C; ++j)
                {
                    int v = b.at<uint8_t>(i, j);
                    os << std::setw(5) << v;
                }
                os << std::endl;
            }
            break;
        case corecvs::BufferType::F32:
            os << std::setprecision(15) << std::scientific;
            for(size_t i = 0; i < R; ++i)
            {
                for(size_t j = 0; j < C; ++j)
                {
                    os << std::setw(20) << b.at<float>(i, j) << "\t";
                }
                os << std::endl;
            }
            break;
    }
    return os;
}

#include "buffers/g12Buffer.h"
#include "buffers/g8Buffer.h"
#include "buffers/rgb24/rgb24Buffer.h"

namespace corecvs {

/* Converters */
G8Buffer *RuntimeTypeBuffer::toG8Buffer()
{
    if (!isValid())
        return NULL;

    G8Buffer *buffer = new G8Buffer((int)rows, (int)cols, false);
    if (type == BufferType::U8)
    {
        buffer->fillWithRaw(data);
    }
    else if (type == BufferType::F32)
    {
        for (size_t i = 0; i < rows; i++)
        {
            for (size_t j = 0; j < cols; j++)
            {
                buffer->element((int)i, (int)j) = (uint8_t)(at<float>(i, j));
            }
        }
    }
    return buffer;
}

G12Buffer *RuntimeTypeBuffer::toG12Buffer(double min, double max)
{
    if (!isValid())
        return NULL;

    G12Buffer *buffer = new G12Buffer((int)rows, (int)cols, false);
    if (type == BufferType::U8)
    {
        for (size_t i = 0; i < rows; i++)
        {
            for (size_t j = 0; j < cols; j++)
            {
                buffer->element((int)i, (int)j) = at<uint8_t>(i, j) << 4;
            }
        }
    }
    else if (type == BufferType::F32)
    {
        for (size_t i = 0; i < rows; i++)
        {
            for (size_t j = 0; j < cols; j++)
            {
                int maxVal = G12Buffer::BUFFER_MAX_VALUE;
                double value = lerpLimit(0, maxVal, at<float>(i, j), min, max);
                buffer->element((int)i, (int)j) = (uint16_t)value;
            }
        }
    }
    return buffer;
}

RGB24Buffer *RuntimeTypeBuffer::toRGB24Buffer(double min, double max)
{
    if (!isValid())
        return NULL;

    RGB24Buffer *buffer = new RGB24Buffer((int)rows, (int)cols, false);
    if (type == BufferType::U8)
    {
        for (size_t i = 0; i < rows; i++)
        {
            for (size_t j = 0; j < cols; j++)
            {
                buffer->element((int)i, (int)j) = RGBColor::gray(at<uint8_t>(i, j));
            }
        }
    }
    else if (type == BufferType::F32)
    {
        for (size_t i = 0; i < rows; i++)
        {
            for (size_t j = 0; j < cols; j++)
            {
                double value = lerpLimit(0, 0xFF, at<float>(i, j), min, max);
                buffer->element((int)i, (int)j) = RGBColor::gray(value);
            }
        }
    }
    return buffer;
}

} //namespace corecvs
