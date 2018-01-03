#ifndef BRESENHAMRASTERIZER_H
#define BRESENHAMRASTERIZER_H


class BresenhamRasterizer
{
public:
    BresenhamRasterizer();

    /**
     * This function implements the Bresenham algorithm.
     * More details on the Bresenham algorithm here - http://en.wikipedia.org/wiki/Bresenham_algorithm
     *
     * NB! This function doesn't perform clipping it just draws nothing in case of coordinates outside of the area
     * You must clip yourself
     */
template<class BufferType>
    static void drawLineSimple (BufferType &buffer, int x1, int y1, int x2, int y2, const typename BufferType::InternalElementType &color )
    {
        int h = buffer.getH();
        int w = buffer.getW();

        if (x1 < 0 || y1 < 0 || x1 >= w || y1 >= h)
            return;
        if (x2 < 0 || y2 < 0 || x2 >= w || y2 >= h)
            return;

        int    dx = (x2 >= x1) ? x2 - x1 : x1 - x2;
        int    dy = (y2 >= y1) ? y2 - y1 : y1 - y2;
        int    sx = (x2 >= x1) ? 1 : -1;
        int    sy = (y2 >= y1) ? 1 : -1;
        int x;
        int y;
        int i;


        if ( dy <= dx )
        {
            int    d  = ( dy << 1 ) - dx;
            int    d1 = dy << 1;
            int    d2 = ( dy - dx ) << 1;

            buffer.element(y1, x1) = color;

            for (x = x1 + sx, y = y1, i = 1; i <= dx; i++, x += sx )
            {
                if ( d > 0 )
                {
                    d += d2;
                    y += sy;
                }
                else
                {
                    d += d1;
                }

                buffer.element(y, x) = color;
            }
        }
        else
        {
            int    d  = ( dx << 1 ) - dy;
            int    d1 = dx << 1;
            int    d2 = ( dx - dy ) << 1;

            buffer.element(y1, x1) = color;

            for (x = x1, y = y1 + sy, i = 1; i <= dy; i++, y += sy )
            {
                if ( d > 0 )
                {
                    d += d2;
                    x += sx;
                }
                else
                {
                    d += d1;
                }

                buffer.element(y, x) = color;
            }
        }
    }
};

#endif // BRESENHAMRASTERIZER_H
