#include <fstream>
#include <list>
#include <algorithm>

#include "core/utils/utils.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/bufferFactory.h"
#include "core/fileformats/svgLoader.h"
#include "core/reflection/commandLineSetter.h"

#define EPSIL 0.00001

#include "nester.h"

using namespace corecvs;
using namespace std;



int main(int argc, char **argv)
{
    Polygon QQQ = Polygon::FromRectagle(Rectangled());



    Rectangled Bin (0, 0, 20, 20);

    Vector2dd A1(0,0);
    Vector2dd A2(1,1);
    Vector2dd A3(2,0);
    Vector2dd A4(1,-1);

    Polygon A = {A1, A2, A3, A4};

    Vector2dd A5(1,4);
    Vector2dd A6(0, 5);
    Vector2dd A7(7, 16);
    Polygon B = {A5, A6, A7};
    Vector2dd A8(1, 1);
    Vector2dd A9(2,2);
    Vector2dd A10(3,1);
    Polygon C = {A8, A9, A10};
    Vector2dd A11(0,0);
    Vector2dd A12(0,1);
    Vector2dd A13(1,0);
    Polygon D {A11, A12, A13};
    Vector2dd A14(0,0);
    Vector2dd A15(0,1);
    Vector2dd A16(1,0);
    Polygon Badone = {A14, A15, A16};

    Vector2dd A17(0,0);
    Vector2dd A18(0,4);
    Vector2dd A19(4,4);
    Vector2dd A20(4,0);
    Polygon TRIANGLE{A17, A18, A19, A20};



    CommandLineSetter s(argc, argv);
    vector<std::string> params = s.nonPrefix();

    std::string svgName = (params.size() < 2) ? "/home/evgeny/poly.svg" : params[1];
    cout << svgName;
    SvgFile svg;
    SvgLoader loader;

    std::ifstream svgFile;

    svgFile.open(svgName, std::ifstream::in);
    if (svgFile.fail())
    {
        SYNC_PRINT(("Can't open svg file <%s> for reading\n", svgName.c_str()));
        return 1;
    } else {
        SYNC_PRINT(("Opened svg file <%s> for reading\n", svgName.c_str()));
    }

    loader.loadSvg(svgFile, svg);

    vector<Polygon> inputPolygons;

    for (auto s: svg.shapes )
    {
        addSubPolygons(s, inputPolygons);
    }

    SYNC_PRINT(("Loaded %d polygons\n", (int)inputPolygons.size()));


    // BLPlacement(Bin, inputPolygons);

    /****
     *  Put your code here
     ****/

    inputPolygons.push_back(B);
    inputPolygons.push_back(B);
    inputPolygons.push_back(B);
    inputPolygons.push_back(A);
    inputPolygons.push_back(A);
    inputPolygons.push_back(A);
    inputPolygons.push_back(A);
    inputPolygons.push_back(A);

    inputPolygons.push_back(A);
    inputPolygons.push_back(A);
    inputPolygons.push_back(A);
    inputPolygons.push_back(A);
    inputPolygons.push_back(A);
    inputPolygons.push_back(A);
    inputPolygons.push_back(A);
    inputPolygons.push_back(A);
    inputPolygons.push_back(A);
    inputPolygons.push_back(A);




    LazySort(inputPolygons);

    for(auto &p : inputPolygons)
    {
        DoClockOrP(p);
        LowerMassCenter(p);

    }

    BLPlacement(Bin, inputPolygons); //всякие защиты от пустых множеств отсутствуют


    for (Polygon& p: inputPolygons )
    {
        showP(p);
    }

    int h = s.getInt("h", 1000);
    int w = s.getInt("w", 1000);

    drawPolygons(inputPolygons, h, w, "debug.bmp");
    drawSvgPolygons(inputPolygons, Bin.height(), Bin.width(), "debug.svg");
    return 0;

}


