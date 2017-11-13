#ifndef CRGB24BUFFER_H_
#define CRGB24BUFFER_H_
/**
 * \file rgb24Buffer.h
 * \brief a header for GRGB24buffer.cpp
 *
 * \ingroup cppcorefiles
 * \date Mar 1, 2010
 * \author alexander
 */

#include <stdint.h>

#include "core/utils/global.h"

#include "core/buffers/fixeddisp/fixedPointBlMapper.h"
#include "core/buffers/abstractContiniousBuffer.h"
#include "core/buffers/g12Buffer.h"
#include "core/buffers/g8Buffer.h"
#include "core/buffers/flow/flowBuffer.h"
#include "core/buffers/histogram/histogram.h"
#include "core/geometry/rectangle.h"
#include "core/buffers/rgb24/rgbColor.h"
#include "core/function/function.h"
#include "core/rectification/correspondenceList.h"
#include "core/xml/generated/imageChannel.h"

#include "core/geometry/conic.h"

#include "core/buffers/kernels/fastkernel/readers.h"

namespace corecvs {

typedef AbstractContiniousBuffer<RGBColor, int32_t> RGB24BufferBase;

class RGB24Buffer : public RGB24BufferBase,
                    public FixedPointBlMapper<RGB24Buffer, RGB24Buffer, RGB24BufferBase::InternalIndexType, RGB24BufferBase::InternalElementType>,
                    public           BlMapper<RGB24Buffer, RGB24Buffer, RGB24BufferBase::InternalIndexType, RGB24BufferBase::InternalElementType>

{
public:
//    RGB24Buffer(int32_t h, int32_t w) : CRGB24BufferBase(h, w) {}
    RGB24Buffer(const RGB24Buffer &that) : RGB24BufferBase (that) {}
    RGB24Buffer(RGB24Buffer *that) : RGB24BufferBase (that) {}

    RGB24Buffer(RGB24Buffer *src, int32_t x1, int32_t y1, int32_t x2, int32_t y2) :
        RGB24BufferBase(src, x1, y1, x2, y2) {}

    RGB24Buffer(int32_t h, int32_t w, RGBColor *data) : RGB24BufferBase(h, w, data) {}
    RGB24Buffer(int32_t h, int32_t w, const RGBColor &data) : RGB24BufferBase(h, w, data) {}


    RGB24Buffer(int32_t h, int32_t w, bool shouldInit = true) : RGB24BufferBase (h, w, shouldInit) {}
    RGB24Buffer(Vector2d<int32_t> size, bool shouldInit = true) : RGB24BufferBase (size, shouldInit) {}
    RGB24Buffer(Vector2d<int32_t> size, const RGBColor &data  ) : RGB24BufferBase (size, data) {}

    /*Helper Constructors form the relative types*/
    RGB24Buffer(G12Buffer *buffer) : RGB24BufferBase (buffer->h, buffer->w, false)
    {
        drawG12Buffer(buffer);
    }

    /*Helper Constructors form the relative types*/
    RGB24Buffer(G8Buffer *buffer) : RGB24BufferBase (buffer->h, buffer->w, false)
    {
        drawG8Buffer(buffer);
    }

    /**
     * This function is used as a hack to interface the C-style g12Buffer
     *
     **/
    RGB24Buffer() {}

    void drawG12Buffer(G12Buffer *src, int32_t y = 0, int32_t x = 0);
    void drawG8Buffer(G8Buffer *src, int32_t y = 0, int32_t x = 0);
    void drawFlowBuffer(FlowBuffer *src, int32_t y = 0, int32_t x = 0);
    void drawFlowBuffer1(FlowBuffer *src, double colorScaler = 20.0, int32_t y = 0, int32_t x = 0);
    void drawFlowBuffer2(FlowBuffer *src, double colorShift = 0.0, double colorScaler = 20.0, int32_t y = 0, int32_t x = 0);
    void drawFlowBuffer3(FlowBuffer *src, double colorScaler = 20.0, int32_t y = 0, int32_t x = 0);
    void drawCorrespondenceList(CorrespondenceList *src, double colorScaler = 20.0, int32_t y = 0, int32_t x = 0);

    /**
     *  Draw a rectangle
     **/
    void drawRectangle(const Rectangle<int32_t> &rect, RGBColor color, int style = 0);
    void drawRectangle(int x, int y, int w, int h, RGBColor color, int style = 0);
    void drawRectangle(const Rectangled &rect, RGBColor color, int style = 0);

    //void drawDisplacementBuffer(DisplacementBuffer *src, double step);


    /**
     * This function is used to draw a pixel in the buffer
     *
     * \param y
     * \param x
     * \param color
     **/
    void drawPixel ( int x, int y, RGBColor color);

    /**
     * This function is used to draw a sort of marker over the buffer
     *
     * \param y
     * \param x
     * \param color
     *
     **/
    void drawCrosshare1 ( int x, int y, RGBColor color);
    void drawCrosshare1 (const Vector2dd &point, RGBColor color);


    /**
     * This function is used to draw a sort of marker over the buffer
     *
     * \param y
     * \param x
     * \param color
     *
     **/
    void drawCrosshare2 ( int x, int y, RGBColor color);

    /**
     * This function is used to draw a sort of marker over the buffer
     *
     * \param y
     * \param x
     * \param color
     *
     **/
    void drawCrosshare3 ( int x, int y, RGBColor color);
    void drawCrosshare3 (const Vector2dd &point, RGBColor color);



    //void drawHistogram(Histogram* hist, int x, int y, int h, int w, uint16_t flags = FLAGS_INCLUDE_MARGIN);

    enum {
        FLAGS_INCLUDE_MARGIN  = 0x1,
        FLAGS_LIMIT_DOWNSCALE = 0x2


    };
    void drawHistogram1024x512(Histogram *hist, int x, int y, uint16_t flags = FLAGS_INCLUDE_MARGIN, int hw = 1024, int hh = 512);

    void drawLineSimple (int x1, int y1, int x2, int y2, RGBColor color );
    void drawLine(int x1, int y1, int x2, int y2, RGBColor color );

    void drawHLine(int x1, int y1, int x2, RGBColor color );
    void drawVLine(int x1, int y1, int y2, RGBColor color );


//    void drawCircle(int x, int y, int rad, RGBColor color );
//    void drawCircle(const Circle2d &circle, RGBColor color );

    void drawArc   (int x, int y, int rad, RGBColor color );
    void drawArc1  (int x, int y, int rad, RGBColor color );

    void drawArc   (const Circle2d &circle, RGBColor color );


    /* Some alternatives */
    void drawLine(double x1, double y1, double x2, double y2, RGBColor color );
    void drawLine(const Vector2dd &v1, const Vector2dd &v2, RGBColor color );

    //void drawLineSimple (int x1, int y1, int x2, int y2, int color );
    //void drawLineSafe (int x1, int y1, int x2, int y2, int color );

    /**
     *   Draw Function
     *
     **/
    void drawFunction(
            double zx, double zy,
            double zh, double zw,
            FunctionArgs &f);

    void drawIsolines(
            double zx, double zy,
            double zh, double zw,
            double steps,
            FunctionArgs &f);
    /**
     * Draw Double Buffer
     **/
    enum DoubleDrawStyle {
        STYLE_RAINBOW,
        STYLE_GRAY,
        STYLE_LOG,
        STYLE_ZBUFFER
    };

    void drawDoubleBuffer(const AbstractBuffer<double> &in, int style = STYLE_RAINBOW);

    void fillWithYUYV(uint8_t *data);
    void fillWithUYVU(uint8_t *data);
    void fillWithYUVFormat(uint8_t *yuyv, bool fillAsUYVY = false);

    //void fillWith420P (uint8_t *y, uint8_t *u, uint8_t *v, int ly, int lu, int lv);

    void dropValueAndSatuation(void);
    void dropValue();

    G12Buffer *toG12Buffer();

    G8Buffer* getChannel(ImageChannel::ImageChannel channel);

    template<class SelectorPrediate>
    Vector3dd getMeanValue(int x1, int y1, int x2, int y2, const SelectorPrediate &predicate)
    {
        /* Some sanity check. Could use rectangle operations for this */
        if (x1 < 0) x1 = 0;
        if (y1 < 0) y1 = 0;
        if (x2 < 0) x2 = 0;
        if (y2 < 0) y2 = 0;

        if (x1 > w) x1 = w;
        if (y1 > h) y1 = h;
        if (x2 > w) x2 = w;
        if (y2 > h) y2 = h;

        if (x1 > x2) {int tmp = x1; x1 = x2; x2 = tmp;}
        if (y1 > y2) {int tmp = y1; y1 = y2; y2 = tmp;}

        int count = 0;
        Vector3dd sum = Vector3dd(0.0);

        for (int i = y1; i < y2; i++)
        {
            for (int j = x1; j < x2; j++)
            {
                if (!predicate(i,j)) {
                    continue;
                }
                count++;
                sum += element(i,j).toDouble();

            }
        }

        if (count == 0) {
            return Vector3dd(0.0);
        }
        return sum / count;

    }

    virtual ~RGB24Buffer() {}
    static double diffL2 (RGB24Buffer *buffer1, RGB24Buffer *buffer2);
    void diffBuffer(RGB24Buffer *that, int *diffPtr);
    static RGB24Buffer *diff(RGB24Buffer *buffer1, RGB24Buffer *buffer2, int *diff = NULL);


    class RGBEx : public FixedVectorBase<RGBEx, uint16_t, 4>
    {
    public:
        RGBEx() {}
        RGBEx(const RGBColor &color)
        {
            at(0) = color.r();
            at(1) = color.g();
            at(2) = color.b();
            at(3) = color.a();
        }

        RGBColor toRGBColor() const
        {
            return RGBColor((uint8_t)at(0), (uint8_t)at(1), (uint8_t)at(2), (uint8_t)at(3));
        }
    };

    class RGBEx32 : public FixedVectorBase<RGBEx32, uint32_t, 3>
    {
    public:
        RGBEx32() {}
        RGBEx32(const RGBColor &color)
        {
            at(0) = color.r();
            at(1) = color.g();
            at(2) = color.b();
            at(3) = color.a();
        }

        RGBColor toRGBColor() const
        {
            return RGBColor((uint8_t)at(0), (uint8_t)at(1), (uint8_t)at(2), (uint8_t)at(3));
        }
    };

    /* This should be merged with generic elementBl */
    RGB24Buffer::InternalElementType elementBlSSE(double y, double x)
    {
#ifndef WITH_SSE
       return elementBlFixed(y, x);
#else
        /* floor() is needed here because of values (-1..0] which will be
         * rounded to 0 and cause error */
        RGB24Buffer::InternalIndexType i = (RGB24Buffer::InternalIndexType)floor(y);
        RGB24Buffer::InternalIndexType j = (RGB24Buffer::InternalIndexType)floor(x);

        CORE_ASSERT_TRUE_P(this->isValidCoordBl(y, x),
                ("Invalid coordinate in AbstractContiniousBuffer::elementBl(double y=%lf, double x=%lf) buffer sizes is [%dx%d]",
                   y, x, this->w, this->h));

        /* Fixed point */
        const int FIXED_SHIFT = 11;
        uint32_t value = (1 << FIXED_SHIFT);

        Int32x4 k1 ((int32_t)((x - j) * value));
        Int32x4 k2 ((int32_t)((y - i) * value));
        Int32x4 v  (value);

        FixedVector<Int32x4, 2> in1 = SSEReader2BBBB_QQQQ::read((uint8_t *)&this->element(i  ,j));
        FixedVector<Int32x4, 2> in2 = SSEReader2BBBB_QQQQ::read((uint8_t *)&this->element(i+1,j));

        Int32x4 result =
             (in1[0] * (v - k1) + k1 * in1[1]) * (v - k2) +
             (in2[0] * (v - k1) + k1 * in2[1]) *      k2;
        result >>= (FIXED_SHIFT * 2);

        int32_t data[4];
        result.save(data);
        return RGBColor(data[2], data[1], data[0]);
#endif
    }



    RGB24Buffer::InternalElementType elementBlFixed(double y, double x)
    {
        /* floor() is needed here because of values (-1..0] which will be
         * rounded to 0 and cause error */
        RGB24Buffer::InternalIndexType i = (RGB24Buffer::InternalIndexType)floor(y);
        RGB24Buffer::InternalIndexType j = (RGB24Buffer::InternalIndexType)floor(x);

        CORE_ASSERT_TRUE_P(this->isValidCoordBl(y, x),
                ("Invalid coordinate in AbstractContiniousBuffer::elementBl(double y=%lf, double x=%lf) buffer sizes is [%dx%d]",
                   y, x, this->w, this->h));

        /* Fixed point */
        uint32_t value = 255 * 16;

        uint32_t k1 = (uint32_t)((x - j) * value);
        uint32_t k2 = (uint32_t)((y - i) * value);

        RGBEx32 a = RGBEx32(this->element(i    ,j    ));
        RGBEx32 b = RGBEx32(this->element(i    ,j + 1));
        RGBEx32 c = RGBEx32(this->element(i + 1,j    ));
        RGBEx32 d = RGBEx32(this->element(i + 1,j + 1));


        RGBEx32 result =
             (a * (value - k1) + k1 * b) * (value - k2) +
             (c * (value - k1) + k1 * d) *          k2;
        result /= (value * value);
        return result.toRGBColor();
    }


    /* This should be merged with generic elementBl */
    RGB24Buffer::InternalElementType elementBlDouble(double y, double x)
    {
        /* floor() is needed here because of values (-1..0] which will be
         * rounded to 0 and cause error */
        RGB24Buffer::InternalIndexType i = (RGB24Buffer::InternalIndexType)floor(y);
        RGB24Buffer::InternalIndexType j = (RGB24Buffer::InternalIndexType)floor(x);

        CORE_ASSERT_TRUE_P(this->isValidCoordBl(y, x),
                ("Invalid coordinate in AbstractContiniousBuffer::elementBl(double y=%lf, double x=%lf) buffer sizes is [%dx%d]",
                   y, x, this->w, this->h));

        /* So far use slow version. Generally this sould be done with fixed point */
        double k1 = x - j;
        double k2 = y - i;

        Vector3dd a = this->element(i    ,j    ).toDouble();
        Vector3dd b = this->element(i    ,j + 1).toDouble();
        Vector3dd c = this->element(i + 1,j    ).toDouble();
        Vector3dd d = this->element(i + 1,j + 1).toDouble();


        Vector3dd result =
             (a * (1 - k1) + k1 * b) * (1 - k2) +
             (c * (1 - k1) + k1 * d) *      k2;
        return RGBColor::FromDouble(result);
    }

    RGB24Buffer::InternalElementType elementBl(double y, double x)
    {
        return elementBlSSE(y,x);
    }

    RGB24Buffer::InternalElementType elementBl(const Vector2dd &point)
    {
        return elementBlSSE(point.y(), point.x());
    }

    RGB24Buffer::InternalElementType elementBlPrecomp(const BilinearMapPoint &point)
    {
        RGBEx a = RGBEx(this->element(point.y    , point.x    ));
        RGBEx b = RGBEx(this->element(point.y    , point.x + 1));
        RGBEx c = RGBEx(this->element(point.y + 1, point.x    ));
        RGBEx d = RGBEx(this->element(point.y + 1, point.x + 1));

        RGBEx result = (a * point.k1 + b * point.k2 + c * point.k3 + d * point.k4) / 255;
        return result.toRGBColor();
    }


private:
    /* TODO: Special helper class should be used to store sprites after that this function could be made public */

    void drawSprite(int x, int y, RGBColor color, int d[][2], int pointNum);


};


} //namespace corecvs
#endif /* CRGB24BUFFER_H_ */

