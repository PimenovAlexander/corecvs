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

#include "global.h"

#include "abstractContiniousBuffer.h"
#include "g12Buffer.h"
#include "g8Buffer.h"
#include "flowBuffer.h"
#include "histogram.h"
#include "rectangle.h"
#include "rgbColor.h"
#include "function.h"
#include "correspondanceList.h"
namespace corecvs {


#define FLAGS_INCLUDE_MARGIN 0x1


typedef AbstractContiniousBuffer<RGBColor, int32_t> CRGB24BufferBase;

class RGB24Buffer : public CRGB24BufferBase
{
public:
//    RGB24Buffer(int32_t h, int32_t w) : CRGB24BufferBase(h, w) {}
    RGB24Buffer(RGB24Buffer &that) : CRGB24BufferBase (that) {}
    RGB24Buffer(RGB24Buffer *that) : CRGB24BufferBase (that) {}

    RGB24Buffer(RGB24Buffer *src, int32_t x1, int32_t y1, int32_t x2, int32_t y2) :
        CRGB24BufferBase(src, x1, y1, x2, y2) {}

    RGB24Buffer(int32_t h, int32_t w, RGBColor *data) : CRGB24BufferBase(h, w, data) {}
    RGB24Buffer(int32_t h, int32_t w, const RGBColor &data) : CRGB24BufferBase(h, w, data) {}


    RGB24Buffer(int32_t h, int32_t w, bool shouldInit = true) : CRGB24BufferBase (h, w, shouldInit) {}
    RGB24Buffer(Vector2d<int32_t> size, bool shouldInit = true) : CRGB24BufferBase (size, shouldInit) {}

    /*Helper Constructors form the relative types*/
    RGB24Buffer(G12Buffer *buffer) : CRGB24BufferBase (buffer->h, buffer->w, false)
    {
        drawG12Buffer(buffer);
    }

    /*Helper Constructors form the relative types*/
    RGB24Buffer(G8Buffer *buffer) : CRGB24BufferBase (buffer->h, buffer->w, false)
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
    void drawCorrespondanceList(CorrespondanceList *src, double colorScaler = 20.0, int32_t y = 0, int32_t x = 0);

    //void drawRectangle(const Rectangle<int32_t> &rect, RGBColor color, int style = 0);

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




    //void drawHistogram(Histogram* hist, int x, int y, int h, int w, uint16_t flags = FLAGS_INCLUDE_MARGIN);

    void drawHistogram1024x512(Histogram *hist, int x, int y, uint16_t flags = FLAGS_INCLUDE_MARGIN);

    void drawLineSimple (int x1, int y1, int x2, int y2, RGBColor color );
    void drawLine(int x1, int y1, int x2, int y2, RGBColor color );


    void drawCircle(int x, int y, int rad, RGBColor color );
    void drawArc (int x, int y, int rad, RGBColor color );
    void drawArc1(int x, int y, int rad, RGBColor color );


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


    void fillWithYUYV (uint8_t *yuyv);
    G12Buffer *toG12Buffer();

    enum ChannelID {
        CHANNEL_R,
        CHANNEL_G,
        CHANNEL_B,
        CHANNEL_GRAY
    };
    G8Buffer* getChannel(ChannelID channel);

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

private:
    /* TODO: Special helper class should be used to store sprites after that this function could be made public */

    void drawSprite(int x, int y, RGBColor color, int d[][2], int pointNum);


};


} //namespace corecvs
#endif /* CRGB24BUFFER_H_ */

