#ifndef WURASTERISER_H
#define WURASTERISER_H

#include <cstdlib>
#include <climits>
#include <algorithm>
#include <cfloat>
#include <iostream>

#include "core/math/mathUtils.h"

class WuRasterizer {
    // integer part of x
    static float fpart(float x) {
        return x - (int)x;
    }

    static float rfpart(float x) {
        return 1. - fpart(x);
    }

    template<class BufferType>
    static void handleEndPont(BufferType &buffer, int x, int y, int gradient, bool steep, const typename BufferType::InternalElementType &color) {
        int h = buffer.getH();
        int w = buffer.getW();

        int xend = round(x);
        float yend = y + gradient * (float(xend) - x);
        float xgap = rfpart(x + 0.5);
        int xpxl1 = xend; // this will be used in the main loop

        int ypxl1 = (int)yend;

        if (steep) {
            if (xpxl1 >= 0 && xpxl1 < h) {
                if (ypxl1 >= 0 && ypxl1 < w) {
                    float factor = rfpart(yend) * xgap;
                    buffer.element(ypxl1, xpxl1) = buffer.element(ypxl1, xpxl1) * (1.0f - factor);
                    auto color1 = color;
                    color1 = color1 * factor;
                    buffer.element(ypxl1, xpxl1) = buffer.element(ypxl1, xpxl1) + color1;
                }
                if ((ypxl1 + 1) >= 0 && (ypxl1 + 1) < w) {
                    float factor = fpart(yend) * xgap;
                    buffer.element(ypxl1 + 1, xpxl1) = buffer.element(ypxl1 + 1, xpxl1) * (1.0f - factor);
                    auto color1 = color;
                    color1 = color1 * factor;
                    buffer.element(ypxl1 + 1, xpxl1) = buffer.element(ypxl1 + 1, xpxl1) + color1;
                }
            }
        }
        else {
            if (xpxl1 >= 0 && xpxl1 < w) {
                if (ypxl1 >= 0 && ypxl1 < h) {
                    float factor = rfpart(yend) * xgap;
                    buffer.element(xpxl1, ypxl1) = buffer.element(xpxl1, ypxl1) * (1.0f - factor);
                    auto color1 = color;
                    color1 = color1 * factor;
                    buffer.element(xpxl1, ypxl1) = buffer.element(xpxl1, ypxl1) + color1;
                }
                if ((ypxl1 + 1) >= 0 && (ypxl1 + 1) < h) {
                    float factor = fpart(yend) * xgap;
                    buffer.element(xpxl1, ypxl1 + 1) = buffer.element(xpxl1, ypxl1 + 1) * (1.0f - factor);
                    auto color1 = color;
                    color1 = color1 * factor;
                    buffer.element(xpxl1, ypxl1 + 1) = buffer.element(xpxl1, ypxl1 + 1) + color1;
                }
            }
        }
    }

public:
    WuRasterizer();

    /**
     * This function implements the Wu's algorithm of drawing of rasterized line.
     * More details on the Wu's algorithm here - https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm
     *
     * NB! This function doesn't perform clipping it just draws nothing in case of coordinates outside of the area
     * You must clip yourself
     */
    template<class BufferType>
    static void drawLine(BufferType &buffer, float x0, float y0, float x1, float y1, const typename BufferType::InternalElementType &color) {
        // Mirror coordinates first, to make them consistent with order of indices in buffer
        std::swap(x0, y0);
        std::swap(x1, y1);

        bool steep = (fabs(y1 - y0) > fabs(x1 - x0));
        int h = buffer.getH();
        int w = buffer.getW();

        if (steep) {
            std::swap(x0, y0);
            std::swap(x1, y1);
        }

        if (x0 > x1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }

        float dx = x1 - x0;
        float dy = y1 - y0;
        float gradient = 0.;
        if (std::abs(dx) < FLT_EPSILON) {
            gradient = 1.0;
        }
        else {
            gradient = dy / dx;
        }

        int xpxl1 = round(x0), xpxl2 = round(x1);
        float intery = y0 + gradient * (float(round(x0)) - x0) + gradient; // first y-intersection for the main loop

        handleEndPont(buffer, x0, y0, gradient, steep, color);
        handleEndPont(buffer, x1, y1, gradient, steep, color);

        // main loop
        for (int x = xpxl1 + 1; x <= xpxl2 - 1; x++) {
            if (steep) {
                if (x >= 0 && x < h) {
                    int y = (int)intery;
                    if (y >= 0 && y < w) {
                        buffer.element(y, x) = buffer.element(y, x) * (1.0f - rfpart(intery));
                        auto color1 = color;
                        color1 = color1 * rfpart(intery);
                        buffer.element(y, x) = buffer.element(y, x) + color1;
                    }
                    if ((y + 1) >= 0 && (y + 1) < w) {
                        buffer.element(y + 1, x) = buffer.element(y + 1, x) * (1.0f - fpart(intery));
                        auto color1 = color;
                        color1 = color1 * fpart(intery);
                        buffer.element(y + 1, x) = buffer.element(y + 1, x) + color1;
                    }
                }
                intery = intery + gradient;
            }
            else {
                if (x >= 0 && x < w) {
                    int y = (int)intery;
                    if (y >= 0 && y < h) {
                        buffer.element(x, y) = buffer.element(x, y) * (1.0f - rfpart(intery));
                        auto color1 = color;
                        color1 = color1 * rfpart(intery);
                        buffer.element(x, y) = buffer.element(x, y) + color1;
                    }
                    if ((y + 1) >= 0 && (y + 1) < h) {
                        buffer.element(x, y + 1) = buffer.element(x, y + 1) * (1.0f - fpart(intery));
                        auto color1 = color;
                        color1 = color1 * fpart(intery);
                        buffer.element(x, y + 1) = buffer.element(x, y + 1) + color1;
                    }
                }
                intery = intery + gradient;
            }
        }
    }
};

#endif // WURASTERISER_H
