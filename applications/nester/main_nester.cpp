#include <fstream>
#include <list>
#include <algorithm>

#include "core/utils/utils.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/bufferFactory.h"
#include "core/fileformats/svgLoader.h"
#include "core/reflection/commandLineSetter.h"
#include "nester.h"

using namespace corecvs;
using namespace std;

void bottomLeftPlacement(list <corecvs :: Polygon> &inp, corecvs :: Rectangled &Bin);
void addPointsList(list <Vector2dd> &L);

void drawPolygons(list <Polygon> inputPolygons, int h, int w, string bmpname);
void drawSvgPolygons(list <Polygon> inputPolygons, int h, int w, string svgName);

void lowerMassCenter(Polygon& A);

Vector2dd massCenter(const Polygon &A);


int main(int argc, char **argv)
{

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



    //Vector2dd B = {4, 12};
    //Polygon A = {{12, 12}, {24, 12}, {25,12}};

    Polygon SQUAR = {{1, 1}, {1, 3}, {3,3}, {3, 1}};
    Vector2dd A1 = {2, 0.5}; //outside
    Vector2dd A2 = {3, 4};   //outside
    Vector2dd A3 = {3, 2};   //on the edge
    Vector2dd A4 = {2, 3};   //on the edge
    Vector2dd A5 = {3, 3};   //vertex
    Vector2dd A6 = {-1, 5};  //outside
    Vector2dd A7 = {-3, -4}; //outside

    cout <<"are some points in squar" << endl;
    cout << SQUAR.isInsideConvex(A1) << " " << SQUAR.isInsideConvex(A2) << " " << SQUAR.isInsideConvex(A3) << " " << SQUAR.isInsideConvex(A4) << " ";
    cout << SQUAR.isInsideConvex(A5) << " " << SQUAR.isInsideConvex(A6) << " " << SQUAR.isInsideConvex(A7);
    cout << endl << "so edges count as inside too" <<endl <<endl;

    Polygon IfLongNamedFunctionWorthIt = {{-4, -0.5}, {-2.5, 3}, {2, 2.5}, {4, 0}, {1, -0.5}, {-2, -1}}; //last three vertexes on one line
    // if initiate degenerate polygon:  Polygon IfLongNamedFunctionWorthIt = {{-4, -0.5}, {-2.5, 3}, {-2.5, 3}, {2, 2.5}, {4, 0}, {1, -0.5}, {-2, -1}};
    // second and third vertexes coincide
    // all seems to work as well, though not hard-tested it

    Vector2dd B1 = {-4, 1}; //outside
    Vector2dd B2 = {-2, 4}; //outside
    Vector2dd B3 = {0, -1}; //outside
    Vector2dd B4 = {91, 0}; //outside
    //vertexes
    Vector2dd B5 = {-4, -0.5}; //vertex
    Vector2dd B6 = {-2.5, 3}; //vertex
    Vector2dd B7 = {2, 2.5}; //vertex
    Vector2dd B8 = {4, 0}; //vertex
    Vector2dd B9 = {1, -0.5}; //vertex
    Vector2dd B10 = {-2, -1}; //vertex
    //checking points lying on edge centers, going from last edge to first
    Vector2dd B11 = {-3, -0.75}; // on the edge
    Vector2dd B12 = {2.5, -0.25}; // on the edge
    Vector2dd B13 = {3, 1.25}; // on the edge
    Vector2dd B14 = {-0.25, 2.75}; // on the edge
    //Really Interior now
    Vector2dd B15 = {-1, 0};
    Vector2dd B16 = {-2.5, 2.999}; //close to second vertex
    Vector2dd B17 = {-0.5, 2}; //close to second edge
    Vector2dd B18 = {3, 0}; //close to second edge


    cout << "are some points in vertex-rich polygon" <<endl;
    cout << "outside ones:" << endl;
    cout << isInteriorROConvexPolBinSearch(B1, IfLongNamedFunctionWorthIt) << " " << isInteriorROConvexPolBinSearch(B2, IfLongNamedFunctionWorthIt) << " ";
    cout << isInteriorROConvexPolBinSearch(B3, IfLongNamedFunctionWorthIt) << " " << isInteriorROConvexPolBinSearch(B4, IfLongNamedFunctionWorthIt) << " ";
    cout << endl << "vertexes" << endl;
    cout << isInteriorROConvexPolBinSearch(B5, IfLongNamedFunctionWorthIt) << " " << isInteriorROConvexPolBinSearch(B6, IfLongNamedFunctionWorthIt) << " ";
    cout << isInteriorROConvexPolBinSearch(B7, IfLongNamedFunctionWorthIt) << " " << isInteriorROConvexPolBinSearch(B8, IfLongNamedFunctionWorthIt) << " ";
    cout << isInteriorROConvexPolBinSearch(B9, IfLongNamedFunctionWorthIt) << " " << isInteriorROConvexPolBinSearch(B10, IfLongNamedFunctionWorthIt) << " ";
    cout << endl << "on the edges" << endl;
    cout << isInteriorROConvexPolBinSearch(B11, IfLongNamedFunctionWorthIt) << " " << isInteriorROConvexPolBinSearch(B12, IfLongNamedFunctionWorthIt) << " ";
    cout << isInteriorROConvexPolBinSearch(B13, IfLongNamedFunctionWorthIt) << " " << isInteriorROConvexPolBinSearch(B14, IfLongNamedFunctionWorthIt) << " ";
    cout << endl << "Interior" << endl;
    cout << isInteriorROConvexPolBinSearch(B15, IfLongNamedFunctionWorthIt) << " " << isInteriorROConvexPolBinSearch(B16, IfLongNamedFunctionWorthIt) << " ";
    cout << isInteriorROConvexPolBinSearch(B17, IfLongNamedFunctionWorthIt) << " " << isInteriorROConvexPolBinSearch(B18, IfLongNamedFunctionWorthIt) << " ";
    cout << endl << endl << endl << endl;
    cout << "are some points in vertex-rich polygon" <<endl;
    cout << "outside ones:" << endl;
    cout << isInteriorConvexPol(B1, IfLongNamedFunctionWorthIt) << " " << isInteriorConvexPol(B2, IfLongNamedFunctionWorthIt) << " ";
    cout << isInteriorConvexPol(B3, IfLongNamedFunctionWorthIt) << " " << isInteriorConvexPol(B4, IfLongNamedFunctionWorthIt) << " ";
    cout << endl << "vertexes" << endl;
    cout << isInteriorConvexPol(B5, IfLongNamedFunctionWorthIt) << " " << isInteriorConvexPol(B6, IfLongNamedFunctionWorthIt) << " ";
    cout << isInteriorConvexPol(B7, IfLongNamedFunctionWorthIt) << " " << isInteriorConvexPol(B8, IfLongNamedFunctionWorthIt) << " ";
    cout << isInteriorConvexPol(B9, IfLongNamedFunctionWorthIt) << " " << isInteriorConvexPol(B10, IfLongNamedFunctionWorthIt) << " ";
    cout << endl << "on the edges" << endl;
    cout << isInteriorConvexPol(B11, IfLongNamedFunctionWorthIt) << " " << isInteriorConvexPol(B12, IfLongNamedFunctionWorthIt) << " ";
    cout << isInteriorConvexPol(B13, IfLongNamedFunctionWorthIt) << " " << isInteriorConvexPol(B14, IfLongNamedFunctionWorthIt) << " ";
    cout << endl << "inside" << endl;
    cout << isInteriorConvexPol(B15, IfLongNamedFunctionWorthIt) << " " << isInteriorConvexPol(B16, IfLongNamedFunctionWorthIt) << " ";
    cout << isInteriorConvexPol(B17, IfLongNamedFunctionWorthIt) << " " << isInteriorConvexPol(B18, IfLongNamedFunctionWorthIt) << " ";

    cout << endl;
    Vector2dd a[99][99];
    for (size_t i = 0; i < 99; ++i)
        for (size_t j = 0; j < 99; ++j){
            a[i][j] = {(double) (i + 1) / 100, (double) (j + 1) / 100};
        }


    Polygon DegeneratePol = {{0,0}, {0, 0.5}, {0,1}, {0.5, 1}, {1,1}, {1, 0.5}, {1, 0}, {0.5, 0} }; //kinda realistic one for nesting

    int sum1 = 0;
    int sum2 = 0;
    cout << endl << "STRICTLY INSIDE";
    //

    clock_t t1 = clock();
    for(int l = 0; l < 1000; ++l)
        for (size_t i = 0; i < 99; ++i)
            for (size_t j = 0; j < 99; ++j)
            {
                sum1 += isInteriorConvexPol(a[i][j], DegeneratePol); //isInteriorROConvexPolBinSearch(a[i][j], DegeneratePol); for some reason runtimes are different depending on order

            }
    t1 = clock() - t1;
    cout << "runtime without BS: " << t1 << endl;

    t1 = clock();
    for(int l = 0; l < 1000; ++l)
        for (size_t i = 0; i < 99; ++i)
            for (size_t j = 0; j < 99; ++j)
            {
                sum2 += isInteriorROConvexPolBinSearch(a[i][j], DegeneratePol); // isInteriorConvexPol(a[i][j], DegeneratePol);
            }
    t1 = clock() - t1;
    cout << "runtime with BS: " <<  t1 << endl;
    cout << endl << sum1 << " " <<sum2 << endl;
    sum1 = 0;
    sum2 = 0;
    cout << endl << "VERTEXES" << endl;
    t1 = clock();
    for(int l = 0; l < 100000; ++l)
        for (size_t i = 0; i < 8; ++i)
        {
            sum1 += isInteriorConvexPol(DegeneratePol.getPoint(i), DegeneratePol);
        }
    t1 = clock() - t1;
    cout << "runtime without BS: " <<  t1 << endl;

    t1 = clock();
    for(int l = 0; l < 100000; ++l)
        for (size_t i = 0; i < 8; ++i)
        {
            sum2 += isInteriorROConvexPolBinSearch(DegeneratePol.getPoint(i), DegeneratePol);
        }
    t1 = clock() - t1;
    cout << "runtime with BS: " <<  t1 << endl;
    vector <Vector2dd> Edges = {{0,25}, {0, 0.75}, {0.25,1}, {0.75, 1}, {1,0.75}, {1, 0.25}, {0.75, 0}, {0.25, 0} };
    cout << "runtime with BS: " <<  t1 << endl;
    cout << endl << sum1 << " " <<sum2 << endl;
    sum1 = 0;
    sum2 = 0;
    cout << endl << "EDGES" << endl;
    t1 = clock();
    for(int l = 0; l < 100000; ++l)
        for (size_t i = 0; i < 8; ++i)
        {
            isInteriorConvexPol(Edges[i], DegeneratePol);
        }
    t1 = clock() - t1;
    cout << "runtime without BS: " <<  t1 << endl;

    t1 = clock();
    for(int l = 0; l < 100000; ++l)
        for (size_t i = 0; i < 8; ++i)
        {
            isInteriorROConvexPolBinSearch(Edges[i], DegeneratePol);
        }
    t1 = clock() - t1;
    cout << "runtime with BS: " <<  t1 << endl;

    cout << endl << sum1 << " " <<sum2 << endl;
    sum1 = 0;
    sum2 = 0;
    cout << endl << "OUTSIDE" << endl;
    Vector2dd outsideArray1[99][99];
    Vector2dd outsideArray2[99][99];
    Vector2dd outsideArray3[99][99];
    Vector2dd shift1 = {1, 1};
    Vector2dd shift2 = {1, 0};
    Vector2dd shift3 = {0, 1};

    for (size_t i = 0; i < 99; ++i)
        for (size_t j = 0; j < 99; ++j)
        {
            outsideArray1[i][j] = a[i][j] + shift1;
            outsideArray2[i][j] = a[i][j] + shift2;
            outsideArray3[i][j] = a[i][j] + shift3;
        }
    t1 = clock();

    for(int l = 0; l < 1000; ++l)
        for (size_t i = 0; i < 99; ++i)
            for (size_t j = 0; j < 99; ++j)
            {
                sum1 += isInteriorROConvexPolBinSearch(outsideArray1[i][j], DegeneratePol);
                sum1 += isInteriorROConvexPolBinSearch(outsideArray2[i][j], DegeneratePol);
                sum1 += isInteriorROConvexPolBinSearch(outsideArray3[i][j], DegeneratePol);
            }
    t1 = clock() - t1;
    cout << "runtime with BS: " <<  t1 << endl;

    t1 = clock();
    for(int l = 0; l < 1000; ++l)
        for (size_t i = 0; i < 99; ++i)
            for (size_t j = 0; j < 99; ++j)
            {
                sum2 += isInteriorConvexPol(outsideArray1[i][j], DegeneratePol);
                sum2 += isInteriorConvexPol(outsideArray2[i][j], DegeneratePol);
                sum2 += isInteriorConvexPol(outsideArray3[i][j], DegeneratePol);
            }
    t1 = clock() - t1;
    cout << "runtime without BS: " <<  t1 << endl;
    cout << endl << sum1 << " " <<sum2 << endl;
    sum1 = 0;
    sum2 = 0;
}


