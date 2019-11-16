#include <iostream>


#include "core/reflection/commandLineSetter.h"
#include "core/buffers/bufferFactory.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/rgb24/abstractPainter.h"

#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif

#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

using namespace std;
using namespace corecvs;

//https://people.csail.mit.edu/yichangshih/wide_angle_portrait/webpage/additional-results/97_TPE_2018_0322_IMG_20180322_120256_P000.jpg

static const int GRID_STEP = 100;


class Grid : public AbstractBuffer<Vector2dd>
{
public:
    Grid(int h, int w) : AbstractBuffer<Vector2dd>(h, w)
    {}

    Polygon getPolygon(int i, int j) const
    {
        return Polygon({
            element(i,j),
            element(i,j+1),
            element(i+1,j+1),
            element(i+1,j)});
    }

    void drawGrid(RGB24Buffer *buffer)
    {
        for (int i = 0; i < h - 1; i++)
        {
            for (int j = 0; j < w - 1; j++)
            {
                buffer->drawLine(element(i,j), element(i,j + 1), RGBColor::Blue() );
                buffer->drawLine(element(i,j), element(i + 1,j), RGBColor::Red()  );
            }
            buffer->drawLine(element(i, w - 1), element(i + 1, w - 1), RGBColor::Red() );
        }
        for (int j = 0; j < w - 1; j++)
        {
            buffer->drawLine(element(h - 1,j), element(h - 1,j + 1), RGBColor::Blue() );
        }
    }

    static Grid TrivialGrid(int h, int w, int step = GRID_STEP)
    {
        Grid toReturn(h, w);
        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                toReturn.element(i, j) = Vector2dd(j * step , i * step );
            }
        }
        return toReturn;
    }

    static Grid TestGrid(int h, int w, int step = GRID_STEP)
    {
        Grid toReturn(h,w);
        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                toReturn.element(i, j) = Vector2dd(j * step , i * step ) + 0.9 * step  * Vector2dd( sin(i / 24.0 * M_PI), cos(j / 24.0 * M_PI));
            }
        }
        return toReturn;
    }

};


int main(int argc, char *argv[])
{
#ifdef WITH_LIBJPEG
    LibjpegFileReader::registerMyself();
    SYNC_PRINT(("Libjpeg support on\n"));
#endif
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    LibpngFileSaver::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif

    CommandLineSetter s(argc, argv);
    cout << "Face undistortion launched" << endl;

    RGB24Buffer *in = BufferFactory::getInstance()->loadRGB24Bitmap("in.png");
    if (in != NULL)
    {
        SYNC_PRINT(("Loaded %d x %d\n", in->h, in->w));
    }

    int gridH = in->h / GRID_STEP + 1;
    int gridW = in->w / GRID_STEP + 1;

    SYNC_PRINT(("Grid size %d x %d\n", gridH, gridW));

    Grid startGrid = Grid::TrivialGrid(gridH , gridW);
    startGrid.drawGrid(in);

    RGB24Buffer *gridDebug = new RGB24Buffer(in->getSize());
    Grid grid = Grid::TestGrid(gridH , gridW);
    grid.drawGrid(gridDebug);



    RGB24Buffer *out = new RGB24Buffer(in->getSize());
    RGB24Buffer *grid1Debug = new RGB24Buffer(in->getSize());

    int count = 0;
    SYNC_PRINT(("Running:\n"));
    parallelable_for( 0, out->h, [&](const BlockedRange<int> &r) {
        for (int i = r.begin(); i < r.end(); i++)
        {
            for (int j = 0; j < out->w; j++)
            {
                Vector2dd point(j,i);
                for (int k = 0; k < grid.h - 1; k++)
                {
                    for (int l = 0; l < grid.w - 1; l++)
                    {
                        Polygon p = grid.getPolygon(k,l);
                        if (!p.isInside(point))
                        {
                            grid1Debug->element(i,j) = RGBColor::getParulaColor(123*k + 6343*l);
                            continue;
                        }                        
                        Vector2dd a =  grid.element(k    , l    );
                        Vector2dd e1 = grid.element(k    , l + 1) - a;
                        Vector2dd e2 = grid.element(k + 1, l    ) - a;
                        Matrix22 m(e1.x(), e2.x(),
                                   e1.y(), e2.y());

                        Vector2dd inv = m.inverted() * (point - a);
                        inv *= GRID_STEP;
                        inv += Vector2dd(l * GRID_STEP , k * GRID_STEP );

                        if (in->isValidCoordBl(inv)) {
                            out->element(i, j) = in->elementBl(inv);
                        }

                    }
                }

            }
            count++;
            SYNC_PRINT(("\r%d", count));

    }});
    SYNC_PRINT(("\n"));



    BufferFactory::getInstance()->saveRGB24Bitmap(in         , "input.png");
    BufferFactory::getInstance()->saveRGB24Bitmap(gridDebug  , "grid.png");
    BufferFactory::getInstance()->saveRGB24Bitmap(grid1Debug , "grid1.png");
    BufferFactory::getInstance()->saveRGB24Bitmap(out        , "out.png");


 return 0;
}
