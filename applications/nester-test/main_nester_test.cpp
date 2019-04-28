#include <fstream>
#include <list>

#include "gtest/gtest.h"

#include "core/utils/utils.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/buffers/rgb24/rgb24Buffer.h"

#include "core/fileformats/svgLoader.h"
#include <core/reflection/commandLineSetter.h>

#include <algorithm>
#include <core/buffers/bufferFactory.h>
#define EPSIL 0.00001

#include "../nester/nester.h"

using namespace corecvs;
using namespace std;

TEST(Nester, twoRectanges)
{
    Rectangled Bin (0, 0, 80, 40);

    vector<Polygon> inp = {
        Polygon::RegularPolygon(6, Vector2dd::Zero(), 40, 0),
        Polygon::RegularPolygon(7, Vector2dd::Zero(), 40, 0)
    };

    drawSvgPolygons(inp, "in.svg");

    LazySort(inp);

    for(auto &p : inp)
    {
        DoClockOrP(p);
    }

    BLPlacement(Bin, inp); //всякие защиты от пустых множеств отсутствуют


    drawSvgPolygons(inp, "out.svg");
}

TEST(Nester, nfp)
{
    Polygon A = Polygon::RegularPolygon(3, Vector2dd(50, 50), 10, 0);
    Polygon B = Polygon::FromRectagle(Rectangled());

    Polygon C = nfp(A, B);

    cout << C << endl;
    vector<Polygon> p = { C };
    drawPolygons(p, 100, 100, "nfp1.bmp");
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}




