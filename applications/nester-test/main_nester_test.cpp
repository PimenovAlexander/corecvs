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

bool testDisjoint(std::vector<Polygon> &inputPolygons)
{
    std::vector<ConvexPolygon> cpl;
    cpl.reserve(inputPolygons.size());
    for (Polygon &p : inputPolygons)
    {
        ConvexPolygon cp = p.toConvexPolygon();
        cp.inset(0.1);
        cpl.push_back(cp);
    }

    for (size_t i = 0; i < cpl.size(); i++)
    {
        for (size_t j = i + 1; j < cpl.size(); j++)
        {
            ConvexPolygon &cp1 = cpl[i];
            ConvexPolygon &cp2 = cpl[j];

            ConvexPolygon cp3 = ConvexPolygon::intersect(cp1, cp2);
        }
    }

}



TEST(Nester, twoRectanges)
{
    Rectangled area (0, 0, 80, 40);

    vector<Polygon> inp = {
        Polygon::RegularPolygon(6, Vector2dd::Zero(), 20, 0),
        Polygon::RegularPolygon(7, Vector2dd::Zero(), 20, 0)
    };

    drawSvgPolygons(inp, area.height(), area.width(), "in.svg");

    LazySort(inp);

    for(auto &p : inp)
    {
        DoClockOrP(p);
    }

    BLPlacement(area, inp); //всякие защиты от пустых множеств отсутствуют


    drawSvgPolygons(inp, area.height(), area.width(), "out.svg");
}

TEST(Nester, manyRectanges)
{
    Rectangled area (0, 0, 1000, 1000);

    vector<Polygon> inp;
    for ( int i = 0; i < 200; i++)
    {
        inp.push_back(Polygon::RegularPolygon(4, Vector2dd::Zero(), 20, degToRad(i)));
    }

    drawSvgPolygons(inp, area.height(), area.width(), "in1.svg");

    LazySort(inp);

    for(auto &p : inp)
    {
        DoClockOrP(p);
    }

    BLPlacement(area, inp);


    drawSvgPolygons(inp, area.height(), area.width(), "out1.svg");
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




