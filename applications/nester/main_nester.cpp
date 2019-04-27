#include <fstream>
#include <list>
#include "core/utils/utils.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/buffers/rgb24/rgb24Buffer.h"

#include "core/fileformats/svgLoader.h"
#include <core/reflection/commandLineSetter.h>

#include <algorithm>
#include <core/buffers/bufferFactory.h>
#define EPSIL 0.00001

using namespace corecvs;
using namespace std;





void addSubPolygons (SvgShape *shape, vector<Polygon> &inputPolygons)
{
    SYNC_PRINT(("addSubPolygons(%s):called\n", SvgShape::getName(shape->type)));
    if (shape->type == SvgShape::POLYGON_SHAPE)
    {
        SvgPolygon *polygon = static_cast<SvgPolygon*>(shape);
        SYNC_PRINT(("adding polygon of %d sides\n", polygon->polygon.size()));
        inputPolygons.push_back(polygon->polygon);
    }
    if (shape->type == SvgShape::GROUP_SHAPE)
    {
        SvgGroup *group = static_cast<SvgGroup*>(shape);
        for (SvgShape * s : group->shapes)
        {
            addSubPolygons(s, inputPolygons);
        }
    }
}

/*
BASE_SHAPE,
RECTANGULAR_SHAPE,
POLYGON_SHAPE,
POLYLINE_SHAPE,
PATH_SHAPE,
CIRCLE_SHAPE,
ELLIPSE_SHAPE,
GROUP_SHAPE
*/

double Dist1(Vector2dd &A, Vector2dd &B)
{
    return (abs(A.x() - B.x()) + abs(A.y() - B.y()));

}

double Dist2(Vector2dd &A, Vector2dd &B)
{
    return sqrt((A.x() - B.x()) * (A.x() - B.x()) + (A.y() - B.y()) * (A.y() - B.y()));
}


Polygon PolygonMakeList(list<Vector2dd> &L)
{
    Vector2dd* P = new Vector2dd[L.size()];
    auto j = 0;
    for (auto i = L.begin(); i != L.end(); ++i)
    {
        P[j] = *i;
        ++j;
    }
    return Polygon(P, L.size());

}

Polygon PolygonMakeList2(list<Vector2dd> &L1, list<Vector2dd> &L2)
{
    Vector2dd* P = new Vector2dd[L1.size() + L2.size()];
    auto j = 0;
    for (auto i = L1.begin(); i != L1.end(); ++i)
    {
        P[j] = *i;
        ++j;
    }

    for (auto i = L2.begin(); i != L2.end(); ++i)
    {
        P[j] = *i;
        ++j;
    }

    return Polygon(P, L1.size() + L2.size());


}

void showP(Polygon &A) //5
{
    cout << endl;
    for(auto i = 0; i <A.size(); ++i)
    {
        cout << A.getPoint(i).x() <<" " << A.getPoint(i).y() << endl;
    }
}


int PyDi(int m, int n) //5_
{
    auto c = m %n;
    if (m >= 0)
        return c;
    else if (n <0)
    {
        return (-n +c)%n;
    }
    else
    {
        return (n + c)%n;
    }
}


Vector2dd GP(Polygon &A, int i)
{
    auto d = PyDi(i, A.size());
    return A[d];
}


double OrientAreaTwice (Vector2dd a, Vector2dd b, Vector2dd c)
{

    return (a.x() - b.x()) * (c.y() - a.y()) - (a.x() - c.x()) * (b.y() - a.y());

}

bool PointOnSegment(Vector2dd &a, Vector2dd &b, Vector2dd &c)
{
    if(abs(OrientAreaTwice(a, b, c)) <= EPSIL)
    {
        if(abs(a.x() - b.x()) + abs(a.x() - c.x()) <= abs(b.x() - c.x()) + EPSIL && abs(a.y() - b.y()) + abs(a.y() - c.y()) <= abs(b.y() - c.y()) + EPSIL )
            return true;
        else return false;
    }
    else return false;
}


bool SegIntersect(Vector2dd &a1, Vector2dd &a2, Vector2dd &b1, Vector2dd &b2)
{
    if((a1.x() == b1.x() && a1.y() == b1.y()) || (a2.x() == b1.x() && a2.y() == b1.y()) || (a2.x() == b2.x() && a2.y() == b2.y()) || (a1.x() == b2.x() && a1.y() == b2.y()))
        return true;
    if(PointOnSegment(a1, b1, b2))
        return true;

    if(PointOnSegment(a2, b1, b2))
        return true;

    if(PointOnSegment(b1, a1, a2))
        return true;


    if(PointOnSegment(b2, a1, a2))
        return true;


    if(OrientAreaTwice(b1, b2, a1) * OrientAreaTwice(b1, b2, a2) <= 0)
    {
        if(abs(OrientAreaTwice(b1, b2, a1)) + abs(OrientAreaTwice(b1, b2, a2)) < abs(OrientAreaTwice(b1, a1, a2)) +EPSIL || abs(OrientAreaTwice(b1, b2, a1)) + abs(OrientAreaTwice(b1, b2, a2)) < abs(OrientAreaTwice(b2,a1, a2)) +EPSIL
                || abs(OrientAreaTwice(b1, a1, a2)) + abs(OrientAreaTwice(b2,a1, a2)) < abs(OrientAreaTwice(b1, b2, a1)) +EPSIL || abs(OrientAreaTwice(b1, a1, a2)) + abs(OrientAreaTwice(b2,a1, a2)) < abs(OrientAreaTwice(b1, b2, a2)) +EPSIL)
            return false;
        else return true;
    }
    else return false;
}


bool PolsIntersect(Polygon &A, Polygon &B)
{
    for (int i = 0; i < A.size(); ++i)
        for (int j = 0; j < B.size(); ++j)
        {
            if(SegIntersect(A.getPoint(i), A.getNextPoint(i), B.getPoint(j), B.getNextPoint(j)))
                return true;
        }
    return false;
}





Polygon MovePolVec(Polygon A,  Vector2dd V)
{
    for (Vector2dd &v : A)
        v = v + V;
    return A;
}

void ActMovePolVec(Polygon &A,  Vector2dd V)
{
    for (Vector2dd &v : A)
        v = v + V;
}




double AngleOX(Vector2dd &V)
{
    Vector2dd O(0,0);
    Vector2dd OX(0,1);
    if(OrientAreaTwice(O, V, OX) <= 0)
        return (V.x() * OX.x() + V.y() * OX.y()) / sqrt (V.x() * V.x() + V.y() * V.y());
    else
    {
        return -2 - (V.x() * OX.x() + V.y() * OX.y()) / sqrt (V.x() * V.x() + V.y() * V.y());
    }
}

bool antiClock(Vector2dd &V1, Vector2dd &V2)
{
    Vector2dd OX(0,1);
    if (AngleOX(V1) <= AngleOX(V2))
        return true;
    else
        return false;
}


Vector2dd GetTopRightPoint(Polygon &A)
{
    list<Vector2dd> TopRightA;

    TopRightA.push_front( A.getPoint(0));
    for (auto v : A)
    {
        if(v.y() == TopRightA.front().y())
            TopRightA.push_back(v);
        else if(v.y() > TopRightA.front().y())
        {
            TopRightA.clear();
            TopRightA.push_front(v);
        }
    }

    auto MaxRight = TopRightA.front();
    for(auto it = TopRightA.begin(); it != TopRightA.end(); ++it)
    {
        if(MaxRight.x() < (*it).x())
            MaxRight = *it;
    }
    return MaxRight;
}

Polygon innernfpR(Rectangled A, Polygon B) //corner if actually bottom left one
{
    auto C = GetTopRightPoint(B);
    auto left = B.getPoint(0).x();
    auto right = B.getPoint(0).x();
    auto bot = B.getPoint(0).y();
    for (auto v : B)
    {
        if(v.x() > right)
            right = v.x();
        if(v.x() < left)
            left = v.x();
        if(v.y() < bot)
            bot = v.y();
    }

    Vector2dd A1 = A.corner;
    A1.y() += A.height() ;
    A1.x() += C.x() - left;
    auto A2 = A.corner;
    A2.x() += (A.width() - right + C.x());
    A2.y() = A1.y();
    auto A3 = A2;
    A3.y() = A2.y() - A.height() + C.y() - bot;
    auto A4 = A3;
    A4.x() = A1.x();
    list <Vector2dd> L = {A4, A1, A2, A3};
    return PolygonMakeList(L);

}

Vector2dd GetBotLeftPoint(Polygon &A)
{
    list<Vector2dd> BotLeftA;

    BotLeftA.push_front( A.getPoint(0));
    for (auto v : A)
    {
        if(v.y() == BotLeftA.front().y())
            BotLeftA.push_back(v);
        else if(v.y() < BotLeftA.front().y())
        {
            BotLeftA.clear();
            BotLeftA.push_front(v);
        }
    }

    auto MaxLeft = BotLeftA.front();
    for(auto it = BotLeftA.begin(); it != BotLeftA.end(); ++it)
    {
        if(MaxLeft.x() > (*it).x())
            MaxLeft = *it;
    }
    return MaxLeft;
}


bool ClockOrP(Polygon &A)
{
    if(OrientAreaTwice(A.getPoint(0), A.getPoint(1), A.getPoint(2)) >= 0)
        return true;
    else return false;

}

void DoClockOrP(Polygon &A)
{
    if(!ClockOrP(A))
    {
        list<Vector2dd> L;
        for(auto i = 0; i < A.size(); ++i)
            L.push_front(A.getPoint(i));
        A = PolygonMakeList(L);
    }
}

Polygon nfp(Polygon &A, Polygon &B)
{
    list<Vector2dd> Edges;
    for(auto i = 0; i < A.size(); ++i)
        Edges.push_back( - A.getNextPoint(i) + A.getPoint(i));
    for(auto i = 0; i < B.size(); ++i)
        Edges.push_back(B.getNextPoint(i) - B.getPoint(i));

    Edges.sort(antiClock);
    auto it1 = Edges.begin();
    ++it1;
    for(auto it2 = Edges.begin(); it1 != Edges.end(); ++it2, ++it1)
    {
        *it1 += *it2;
    }
    auto P = PolygonMakeList(Edges);
    DoClockOrP(P);
    auto BotA = GetBotLeftPoint(A);
    auto BotP = GetBotLeftPoint(P);
    return MovePolVec(P, BotA - BotP);
}

double AreaOfCP(Polygon &A)
{
    auto C = A.getPoint(0);
    auto Result = 0;
    for (auto i =1 ; i <A.size() - 1 ; ++i)
    {
        Result += OrientAreaTwice(C, A.getPoint(i), A.getNextPoint(i));
    }
    Result /= 2;
    return Result;
}

bool PointLiesPol(Vector2dd V, Polygon &A)
{
    for (auto i = 0; i < A.size(); ++i)
    {
        if (PointOnSegment(V, A.getPoint(i), A.getNextPoint(i)))
            return true;
    }

    Vector2dd M(2000, 2005);
    int b = 0;
    for (auto i = 0; i < A.size(); ++i)
    {
        if (SegIntersect(V, M, A.getPoint(i), A.getNextPoint(i)))
        {
            if (abs(OrientAreaTwice(V, M, A.getPoint(i))) <=EPSIL)
            {
                if ( OrientAreaTwice(M, A.getNextPoint(i), V) > 0)
                    if( OrientAreaTwice(M, V,  GP(A, i-1)) >= 0)
                        b++;
                    else
                    {

                    }


                else
                {
                    if ( OrientAreaTwice(M, V, GP(A, i-1)) >= 0)
                        --b;

                }

            }
            else if(OrientAreaTwice(A.getPoint(i), A.getNextPoint(i), V) > 0)
                ++b;
            else
            {
                --b;
            }

        }

    }
    if (b >= 1)
        return true;
    else return false;

}

bool PointLiesIntPol(Vector2dd V, Polygon &A)
{
    for (auto i = 0; i < A.size(); ++i)
    {
        if (PointOnSegment(V, A.getPoint(i), A.getNextPoint(i)))
            return false;
    }

    Vector2dd M(2000, 2005);
    int b = 0;
    for (auto i = 0; i < A.size(); ++i)
    {
        if (SegIntersect(V, M, A.getPoint(i), A.getNextPoint(i)))
        {
            if (abs(OrientAreaTwice(V, M, A.getPoint(i))) <=EPSIL)
            {
                if ( OrientAreaTwice(M, A.getNextPoint(i), V) > 0)
                    if( OrientAreaTwice(M, V,  GP(A, i-1)) >= 0)
                        b++;
                    else
                    {

                    }


                else
                {
                    if ( OrientAreaTwice(M, V, GP(A, i-1)) >= 0)
                        --b;

                }

            }
            else if(OrientAreaTwice(A.getPoint(i), A.getNextPoint(i), V) > 0)
                ++b;
            else
            {
                --b;
            }
        }

    }
    if (b >= 1)
        return true;
    else return false;

}


bool BiggaArea(Polygon &A, Polygon &B)
{
    return (AreaOfCP(A) >= AreaOfCP(B));
}
void LazySort(vector <Polygon> &A)
{
    list<Polygon> B;
    for(int i = 0; i < A.size(); ++i)
    {
        B.push_back(A[i]);
    }
    B.sort(BiggaArea);
    for(int i = 0; i < A.size(); ++i)
    {
        A[i] = B.front();
        B.pop_front();
    }

}


Vector2dd BestOne(list <Vector2dd> &L)
{
    list<Vector2dd> BotLeft;

    BotLeft.push_front(L.front());
    for (auto v : L)
    {
        if(v.y() == BotLeft.front().y())
            BotLeft.push_back(v);
        else if(v.y() < BotLeft.front().y())
        {
            BotLeft.clear();
            BotLeft.push_front(v);
        }
    }

    auto MaxLeft = BotLeft.front();
    for(auto it = BotLeft.begin(); it != BotLeft.end(); ++it)
    {
        if(MaxLeft.x() > (*it).x())
            MaxLeft = *it;
    }
    return MaxLeft;

}


void BLPlacement(Rectangled A, vector <Polygon> &input)
{

    auto firstnfp = innernfpR(A, input[0]);//actually must check if first polygon suits

    list<int> Placed;
    ActMovePolVec(input[0], firstnfp.getPoint(0) - GetTopRightPoint(input[0]));
    Placed.push_front(0);

    for(int i = 1; i < input.size(); ++i)
    {
        firstnfp = innernfpR(A, input[i]); //same
        list <Polygon> nfps;
        Polygon d;
        for (auto it = Placed.begin(); it != Placed.end(); ++it)
        {
            nfps.push_back(nfp(input[*it], input[i]));
        }
        list<Vector2dd> BestCandidates;
        for (auto it1 = nfps.begin(); it1 != nfps.end(); ++it1)
        {
            list<Vector2dd> Candidates;
            for(Vector2dd v : *it1)
            {
                if(v.x() >= firstnfp.getPoint(0).x() && v.x() <= firstnfp.getPoint(2).x()  && v.y() <= firstnfp.getPoint(2).y() && v.y() >= firstnfp.getPoint(0).y())
                {
                    bool b = 0;
                    auto i = it1;
                    if(i!=nfps.end())
                        ++i;
                    auto j = it1;
                    if(j!=nfps.begin())
                        --j;
                    while(i!=nfps.end() && b != 1)
                    {
                        if (PointLiesIntPol(v, *i))
                            b = 1;
                        ++i;
                    }
                    while(j!=nfps.begin() && b != 1)
                    {
                        if (PointLiesIntPol(v, *j))
                            b = 1;
                        --j;
                    }

                    if (PointLiesIntPol(v, *j))
                        b = 1;

                    if(b != 1)
                    {
                        Candidates.push_back(v);
                    }
                }

            }
            if (!Candidates.empty())
            {
                BestCandidates.push_back(BestOne(Candidates));
            }


        }
        if(!BestCandidates.empty())
        {
            ActMovePolVec(input[i], BestOne(BestCandidates) - GetTopRightPoint(input[i]));
            Placed.push_back(i);
        }
        else
        {
            cout << i << "cannot be placed" << endl;

        }
    }
}




int main(int argc, char **argv)
{

    Rectangled Bin (0, 0, 80, 40);
/*
    Vector2dd A1(0,0);
    Vector2dd A2(0,2);
    Vector2dd A3(2,2);
    Vector2dd A4(2,0);

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
    auto M = nfp(Badone,Badone);
     showP(M);
     cout << "AX";
     */

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

    SYNC_PRINT(("Loaded %d polygons\n", inputPolygons.size()));


    // BLPlacement(Bin, inputPolygons);

    /****
     *  Put your code here
     ****/
/*
    inputPolygons.push_back(A);
    inputPolygons.push_back(B);
    inputPolygons.push_back(C);
    inputPolygons.push_back(D);
    inputPolygons.push_back(Badone);
*/
    LazySort(inputPolygons);

    for(auto &p : inputPolygons)
    {
        DoClockOrP(p);
    }

    BLPlacement(Bin, inputPolygons); //всякие защиты от пустых множеств отсутствуют


    for (Polygon& p: inputPolygons )
    {
        showP(p);
    }

    Rectangled area = Rectangled::Empty();
    for (Polygon& p: inputPolygons )
    {
        for (Vector2dd &v : p) {
            cout << "Point:" << v << endl;
            area.extendToFit(v);
        }
    }

    cout << "Surrounding Area " << area << endl;

    int h = s.getInt("h", 1000);
    int w = s.getInt("w", 1000);

     /* Some debug output */
    RGB24Buffer *buffer = new RGB24Buffer(h,w);
    Vector2dd scale(buffer->w, buffer->h);

    for (Polygon& p: inputPolygons )
    {
        for (size_t i = 0; i < p.size(); i++)
        {
            Vector2dd  p1 = ((p.getPoint    (i) - area.corner) / area.size) * scale;
            Vector2dd  p2 = ((p.getNextPoint(i) - area.corner) / area.size) * scale;

            cout << "Point:" << p.getPoint(i) << "=>" << (p.getPoint(i) - area.corner) << "=>" << p1 << endl;

            buffer->drawLine(
                        p1, p2,
                        RGBColor::parula((i + 1.0) / p.size()));
        }
    }

    BufferFactory::getInstance()->saveRGB24Bitmap(buffer, "debug.bmp");

    return 0;

}


