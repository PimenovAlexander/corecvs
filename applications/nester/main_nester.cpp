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
#include "core/fileformats/dxf_support/dxfLoader.h"
#include <algorithm>

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
    for (int i = 0; i < amount; ++i) {
        conteiner.push_back(obj);
    }
}

template < typename T > void shuffle( std::list<T>& lst ) // shuffle contents of a list
{
    // create a vector of (wrapped) references to elements in the list
    // http://en.cppreference.com/w/cpp/utility/functional/reference_wrapper
    std::vector< std::reference_wrapper< const T > > vec( lst.begin(), lst.end() ) ;

    // shuffle (the references in) the vector
    std::shuffle( vec.begin(), vec.end(), std::mt19937{ std::random_device{}() } ) ;

    // copy the shuffled sequence into a new list
    std::list<T> shuffled_list {  vec.begin(), vec.end() } ;

    // swap the old list with the shuffled list
    lst.swap(shuffled_list) ;
}




void tester(const string &name, const string &whereName = "", char symbol = 'a') {
    list<Polygon> polygonList {};
    if (name[name.size() - 1] == 'g') {
        polygonList = loadPolygonListSVG(name);
    } else {
        polygonList = loadPolygonListDXF(name);
    }
    Rectangled area(0, 0, 200, 200);
    vector<list<Polygon>> tests(12, polygonList);

    vinilPlacementNester(tests[0], area, 0, 1, 1, 0.2, 4);
    cout << getMaxValueY(tests[0]) / area.height() << endl << endl;

    vinilPlacementNester(tests[1], area, 0, 1, 1, 0.2, 8);
    cout << getMaxValueY(tests[1]) / area.height() << endl << endl;

    vinilPlacementNester(tests[2], area, 0, 1, 1, 0.2, 16);
    cout << getMaxValueY(tests[2]) / area.height() << endl << endl;

    vinilPlacementNester(tests[3], area, 0, 1, 2, 0.2, 4);
    cout << getMaxValueY(tests[3]) / area.height() << endl << endl;

    vinilPlacementNester(tests[4], area, 0, 1, 2, 0.2, 8);
    cout << getMaxValueY(tests[4]) / area.height() << endl << endl;

    vinilPlacementNester(tests[5], area, 0, 1, 2, 0.2, 16);
    cout << getMaxValueY(tests[5]) / area.height() << endl << endl;

    bruteBL(tests[6], area, 4);
    cout << getMaxValueY(tests[6]) / area.height() << endl << endl;

    bruteBL(tests[7], area, 8);
    cout << getMaxValueY(tests[7]) / area.height() << endl << endl;

    bruteBL(tests[8], area, 16);
    cout << getMaxValueY(tests[8]) / area.height() << endl << endl;

    bottomLeftPlacement(tests[9], area);
    cout << getMaxValueY(tests[9]) / area.height() << endl << endl;

    for (auto &p : tests[10])
        lowerMassCenter(p);
    bottomLeftPlacement(tests[10], area);
    cout << getMaxValueY(tests[10]) / area.height() << endl << endl;

    double min = 10000;
    for (int i = 0; i < 10; ++i) {
        for (auto &p : tests[11])
            lowerMassCenter(p);
        shuffle(tests[11]);
        bruteHeightBL(tests[11], area, 16 , 0.2);
        if (min > getMaxValueY(tests[11]) / area.height())
        min = getMaxValueY(tests[11]) / area.height();
    }
    cout << min << endl << endl;
}

int main(int argc, char **argv) {
    auto polygonList = loadPolygonListSVG("svgnest1.svg");
    cout << polygonList.size();
    cout << getMaxValueY(polygonList) / 300 << endl;
    string name = "/home/evgeny/outTest1.svg";
    //tester(name);


}
