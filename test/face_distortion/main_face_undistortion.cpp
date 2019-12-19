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

static const int GRID_STEP = 30;
static const double f = 750;

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
                //buffer->drawLine(element(i, j+1), element(i+1, j), RGBColor::Yellow());
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
                //toReturn.element(i, j) = Vector2dd(j * step , i * step ) + 1.2 * step  * Vector2dd( sin(i / 24.0 * M_PI), cos(j / 24.0 * M_PI));

                // STEREOGRAPHIC PROJECTION
                // (y, x) = (j * step, i * step)
                double x = j*step;
                double y = i*step;

                // reverse stereographic projection
                // with principal point (w*step/2, h*step/2) - center of image
                Vector2dd shift = Vector2dd(x, y) - Vector2dd(w*step/2.0, h*step/2.0);
                double r = shift.l2Metric();
                shift /= r;
                double tau = 2 * atan2(r / 2, f);
                Vector3dd onSphere = Vector3dd(shift * tau, 1.0).normalised();

                // we got point (x,y,z) on sphere and
                // we want to get stereographic projection
                double theta = atan2(onSphere.xy().l2Metric(), onSphere.z());
                Vector2dd dir = onSphere.xy().normalised();
                dir *= 2 * f * tan(theta/2);
                dir += Vector2dd(w*step/2.0, h*step/2.0);

                toReturn.element(i,j) =  Vector2dd(dir.x(), dir.y());
            }
        }
        return toReturn;
    }

    void SlowGridWarp(RGB24Buffer* in, RGB24Buffer* out)
    {
        int count = 0;
        SYNC_PRINT(("Running:\n"));
        parallelable_for( 0, out->h, [&](const BlockedRange<int> &r) {
            for (int i = r.begin(); i < r.end(); i++)
            {
                for (int j = 0; j < out->w; j++)
                {
                    Vector2dd point(j,i);

                    for (int k = 0; k < h - 1; k++)
                    {
                        for (int l = 0; l < w - 1; l++)
                        {
                            Polygon p = getPolygon(k,l);
                            if (!p.isInside(point))
                            {
                                //grid1Debug->element(i,j) = RGBColor::getParulaColor(13*k + 1235*l);
                                continue;
                            }

                            Vector2dd a =  element(k    , l    );
                            Vector2dd e2 = element(k + 1, l    ) - a;
                            Vector2dd e1 = element(k     , l + 1) - a;

                            Matrix22 m(e1.x(), e2.x(),
                                       e1.y(), e2.y());

                             Vector2dd inv = m.inverted() * (point - a);

                             inv *= GRID_STEP;
                             inv += Vector2dd(l * GRID_STEP , k * GRID_STEP );

                            // consider the sign of |e3 x A| to understand where is the point
                            Vector2dd e3 = element(k+1, l) - element(k, l+1);
                            Vector2dd A = point - element(k, l+1);

                            // if point lies in lower triangle
                            if (e3.x()*A.y() - e3.y()*A.x() < 0)
                            {
                                // rearrange our basis to match this triangle
                                a  = element(k+1, l+1);
                                e2 = element(k, l+1) - a;
                                e1 = element(k+1, l) - a;

                                m = Matrix22(e1.x(), e2.x(),
                                             e1.y(), e2.y());

                                inv = m.inverted() * (point - a);
                                // we actually have to reverse the picture
                                inv *= -GRID_STEP;
                                inv += Vector2dd((l+1) * GRID_STEP , (k+1) * GRID_STEP );


                            }
                            if (in->isValidCoordBl(inv))
                            {
                                out->element(i, j) = in->elementBl(inv);
                            }
                        }
                    }

                }
                count++;
                SYNC_PRINT(("\r%d", count));

        }});
        SYNC_PRINT(("\n"));
    }

    void FasterGridWarp();
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

    cout << gridH << endl;
    cout << gridW << endl;

    SYNC_PRINT(("Grid size %d x %d\n", gridH, gridW));

    Grid startGrid = Grid::TrivialGrid(gridH , gridW);
    //startGrid.drawGrid(in);

    RGB24Buffer *gridDebug = new RGB24Buffer(in->getSize());
    Grid grid = Grid::TestGrid(gridH , gridW);
    grid.drawGrid(gridDebug);

    RGB24Buffer *out = new RGB24Buffer(in->getSize());
    RGB24Buffer *grid1Debug = new RGB24Buffer(in->getSize());

    grid.SlowGridWarp(in, out);

    // Пройти по всей сетке и пометить один раз все пиксели
    // на принадлежность той или иной ячейке (в худшем случае
    // видимо каждый пиксель может принадлежать нескольким ячейкам если он на границе).
    // И потом тогда финишный проход вообще не потребует поиска.



    BufferFactory::getInstance()->saveRGB24Bitmap(in         , "input.png");
    BufferFactory::getInstance()->saveRGB24Bitmap(gridDebug  , "grid.png");
    //BufferFactory::getInstance()->saveRGB24Bitmap(grid1Debug , "grid1.png");
    BufferFactory::getInstance()->saveRGB24Bitmap(out        , "out.png");


 return 0;
}
