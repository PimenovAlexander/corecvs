#ifndef WURASTERISER_H
#define WURASTERISER_H

#include <cstdlib>
#include <climits>
#include <algorithm>
#include <cfloat>

class WuRasterizer {
  // integer part of x
  static int ipart(float x) {
    return floor(x);
  }

  static int round(float x) {
    return ipart(x + 0.5);
  }

  static float fpart(float x) {
    return x - floor(x);
  }

  static float rfpart(float x) {
    return 1. - fpart(x);
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
  static void drawLine (BufferType &buffer, float x0, float y0, float x1, float y1, const typename BufferType::InternalElementType &color) {
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

      // handle first endpoint
      int xend = round(x0);
      float yend = y0 + gradient * (float(xend) - x0);
      float xgap = rfpart(x0 + 0.5);
      int xpxl1 = xend; // this will be used in the main loop
      int ypxl1 = ipart(yend);
      if (steep) {
          if (xpxl1 >= 0 && xpxl1 < h) {
              if (ypxl1 >= 0 && ypxl1 < w) {
                  buffer.element(ypxl1, xpxl1) = color * rfpart(yend) * xgap;
              }
              if ((ypxl1 + 1) >= 0 && (ypxl1 + 1) < w) {
                  buffer.element(ypxl1 + 1, xpxl1) = color * fpart(yend) * xgap;
              }
          }
      }
      else {
          if (xpxl1 >= 0 && xpxl1 < w) {
              if (ypxl1 >= 0 && ypxl1 < h) {
                  buffer.element(xpxl1, ypxl1) = color * rfpart(yend) * xgap;
              }
              if ((ypxl1 + 1) >= 0 && (ypxl1 + 1) < h) {
                  buffer.element(xpxl1, ypxl1 + 1) = color * fpart(yend) * xgap;
              }
          }
      }

      float intery = yend + gradient; // first y-intersection for the main loop

      // handle second endpoint
      xend = round(x1);
      yend = y1 + gradient * (float(xend) - x1);
      xgap = fpart(x1 + 0.5);
      int xpxl2 = xend; //this will be used in the main loop
      int ypxl2 = ipart(yend);
      if (steep) {
          if (xpxl2 >= 0 && xpxl2 < h) {
              if (ypxl2 >= 0 && ypxl2 < w) {
                  buffer.element(ypxl2, xpxl2) = color * rfpart(yend) * xgap;
              }
              if ((ypxl2 + 1) >= 0 && (ypxl2 + 1) < w) {
                  buffer.element(ypxl2 + 1, xpxl2) = color * fpart(yend) * xgap;
              }
          }
      }
      else {
          if (xpxl2 >= 0 && xpxl2 < w) {
              if (ypxl2 >= 0 && ypxl2 < h) {
                  buffer.element(xpxl2, ypxl2) = color * rfpart(yend) * xgap;
              }
              if ((ypxl2 + 1) >= 0 && (ypxl2 + 1) < h) {
                  buffer.element(xpxl2, ypxl2 + 1) = color * fpart(yend) * xgap;
              }
          }
      }

      // main loop
      for (int x = xpxl1 + 1; x <= xpxl2 - 1; x++) {
          int y = ipart(intery);
          if (steep && x >= 0 && x < h) {
              if( y >= 0 && y < w ) {
                  buffer.element(y, x) = color * rfpart(intery);
              }
              if( (y + 1) >= 0 && (y + 1) < w ) {
                  buffer.element(y + 1, x) = color * fpart(intery);
              }
              intery = intery + gradient;
          }
          else if (x >= 0 && x < w) {
              if (y >= 0 && y < h ) {
                  buffer.element(x, y) = color * rfpart(intery);
              }
              if ((y + 1) >= 0 && (y + 1) < h ) {
                  buffer.element(x, y + 1) = color * fpart(intery);
              }
              intery = intery + gradient;
          }
      }
  }

};

#endif // WURASTERISER_H
