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

corecvs::Polygon innerPolygon(corecvs::Polygon& p, double epsil);
void showPolygon (const corecvs::Polygon &A);

template<typename T1, typename T2>
void putInConteiner(T1& conteiner, T2& obj, int amount) {
    for (int i =0; i < amount; ++i) {
        conteiner.push_back(obj);
    }
}

int main(int argc, char **argv) {
    Rectangled area (0, 0, 150, 50);
    Rectangled area2 (0, 0, 100, 15);
    list<Polygon> inputList;

    Polygon urod1 = {{0, 0}, {0, 5}, {3, 2}, {2, 0}};
    Polygon urod2 = {{0, 0}, {2, 2}, {4, 1}, {5, 0}};
    Polygon urod3 = {{0, 0}, {1, 4}, {5, 5}, {6 , 3}, {6 ,1}, {3, 0}};
    Polygon urod4 = {{0, 0}, {1, 2}, {6, 7}, {4, 2}, {3, 1}};
    Polygon urodishe1 = getHomotheticPolygon(urod1, 2);
    Polygon urodishe2 = getHomotheticPolygon(urod2, 2);
    Polygon urodishe3 = getHomotheticPolygon(urod3, 2);
    Polygon urodishe4 = getHomotheticPolygon(urod4, 2);

    Polygon triangleR = Polygon::RegularPolygon(3, Vector2dd::Zero(), 10, 0);
    Polygon quad = Polygon::RegularPolygon(4, Vector2dd::Zero(), 10, 0);
    Polygon pentaR = Polygon::RegularPolygon(5, Vector2dd::Zero(), 10, 0);
    Polygon geksaR = Polygon::RegularPolygon(5, Vector2dd::Zero(), 10, 0);

    Polygon longRectangler = {{0, 0}, {0, 5}, {20, 5}, {20, 0}};
    Polygon simpleRectangler = {{0, 0}, {0, 7}, {15, 7}, {15, 0}};

    Polygon triangle1 = {{0, 0}, {0 , 6}, {10, 0}};
    Polygon triangle2 = {{0, 0}, {2 , 10}, {4, 0}};
    Polygon triangle3 = {{0, 0}, {2 , 8}, {8, 0}};
    Polygon smallTriangle1 = getHomotheticPolygon(triangle1, -2);
    Polygon smallTriangle2 = getHomotheticPolygon(triangle2, -2);
    Polygon smallTriangle3 = getHomotheticPolygon(triangle2, -2);

    Polygon A{{10, 0}, {0, -10}, {-10, 0}, {0, 10}};
    Polygon B{{0, 0}, {10, 20}, {20, 0}};
    Polygon C {{0, 0}, {3.5, 2.5}, {4, 0}};

    clock_t begin = clock();
    putInConteiner(inputList, triangle1, 20);
    putInConteiner(inputList, triangle2, 20);
    putInConteiner(inputList, quad, 10);
    putInConteiner(inputList, urod1, 10);
    putInConteiner(inputList, smallTriangle1, 20);


    drawSvgPolygons(inputList, area.height(), area.width(), "inTest1.svg");
    vinilPlacementNester(inputList, area, 0, 1, 1, 0.2, 32);
    double max = 0;
        double listarea = 0;
    for (auto &v : inputList) {

        if (max < v[getTopRightIndex(v)].y())
            max = v[getTopRightIndex(v)].y();
        listarea += v.area();
    }
    cout << listarea /(area.height() * area.width()) <<endl;
    inputList.push_back(polFromRec(area));
    drawSvgPolygons(inputList, area.height(), area.width(), "outTest1.svg");
    clock_t end = clock();
    cout << max / area.height() << endl;
    cout << double(end - begin) / 1000000 << endl << endl;
}
