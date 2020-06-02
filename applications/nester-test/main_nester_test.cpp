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
//bool testDisjoint(std::vector<Polygon> &inputPolygons)
//{
//    std::vector<ConvexPolygon> cpl;
//    cpl.reserve(inputPolygons.size());
//    for (Polygon &p : inputPolygons)
//    {
//        ConvexPolygon cp = p.toConvexPolygon();
//        cp.inset(0.1);
//        cpl.push_back(cp);
//    }

//    for (size_t i = 0; i < cpl.size(); i++)
//    {
//        for (size_t j = i + 1; j < cpl.size(); j++)
//        {
//            ConvexPolygon &cp1 = cpl[i];
//            ConvexPolygon &cp2 = cpl[j];

//            ConvexPolygon cp3 = ConvexPolygon::intersect(cp1, cp2);
//        }
//    }

//}
TEST(Nester, twoRectanges)
{
    Rectangled area (0, 0, 200, 100);

    list <Polygon> inpList = {
        Polygon::RegularPolygon(6, Vector2dd::Zero(), 20, 0),
        Polygon::RegularPolygon(7, Vector2dd::Zero(), 20, 0)
    };
    drawSvgPolygons(inpList, area.height(), area.width(), "in.svg");

    bottomLeftPlacementProtected(inpList, area, 3);
    drawSvgPolygons(inpList, area.height(), area.width(), "out.svg");
}

TEST(Nester, manyRectanges)
{
    Rectangled area (0, 0, 500, 500); //1000, 1000 46
    list<Polygon> inpList;
    Vector2dd A(0,0);
    Vector2dd B(2, 10);
    Vector2dd C(20, 0);
    Polygon someTriangle = {A,B,C};
    for (size_t i = 0; i < 30; i++)
    {
        auto Pol = Polygon::RegularPolygon(4, Vector2dd::Zero(), 20, degToRad(i));
        inpList.push_back(Pol);
    }
    for (size_t i = 0; i < 30; i++)
    {
        auto Pol = Polygon::RegularPolygon(7, Vector2dd::Zero(), 20, degToRad(i));
        inpList.push_back(Pol);
    }
    for (size_t i = 0; i < 30; i++)
    {
        auto Pol = Polygon::RegularPolygon(8, Vector2dd::Zero(), 20, degToRad(i));
        inpList.push_back(Pol);
    }
    for (size_t i = 0; i < 20; ++i)
    {
        inpList.push_back(someTriangle);
    }
    drawSvgPolygons(inpList, area.height(), area.width(), "in1.svg");

    bottomLeftPlacementProtected(inpList, area, 4);
    drawSvgPolygons(inpList, area.height(), area.width(), "out1.svg");
}

TEST(Nester, nfp) //both figures must be right-oriented, or use doClockOrientation(Polygon)
{
    Polygon nfpTestB = {{4,2}, {2,2}, {1,3}, {1,5}, {5, 3.5}};//ClockOrintated
    Polygon nfpTestA = {{4, 0}, {3, -0.5}, {2, -1}, {2, 1}}; //ClockOrintated
    Polygon nfpTestResult1 = convexNFP(nfpTestA, nfpTestB);

    int indWhere = getTopRightIndex(nfpTestResult1);
    int indFrom = getTopRightIndex(nfpTestA);
    nfpTestA.translate(nfpTestResult1.getPoint(indWhere) - nfpTestA.getPoint(indFrom));

    auto d = nfpTestResult1;
    d.translate({-5, -5});

    list <Polygon> p = {nfpTestResult1, nfpTestA, nfpTestB, d};
    drawPolygons(p, 100, 100, "nfp1.bmp"); //does not draw last edge of nfp,  so d show that its all ok
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



