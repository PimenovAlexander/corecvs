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




/** =========================================== **/
void drawPolygons(vector<Polygon> inputPolygons, int h, int w, string bmpname)
{
    Rectangled area = Rectangled::Empty();
    for (Polygon& p: inputPolygons )
    {
        for (Vector2dd &v : p) {
            cout << "Point:" << v << endl;
            area.extendToFit(v);
        }
    }

    cout << "Surrounding Area " << area << endl;

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

    BufferFactory::getInstance()->saveRGB24Bitmap(buffer, bmpname);
}

void drawSvgPolygons(vector<Polygon> inputPolygons, int h, int w, string svgName)
{
    std::ofstream file;
    file.open(svgName, ios::out);
    file.imbue(std::locale("C"));

    file << "<svg height=\""<< h <<"\" width=\""<< w <<"\">" << endl;
    for (size_t i = 0; i < inputPolygons.size(); i++)
    {
          Polygon &p = inputPolygons[i];

          RGBColor color = RGBColor::parula((double)i / inputPolygons.size());
          char colorStr[7];
          snprintf2buf(colorStr, "%02X%02X%02X", color.r(), color.g(), color.b());

          std::string style="style=\"stroke:#";
          style += colorStr;
          style += ";fill:none;stroke-width:1\"";


         file << "<polygon points=\"";
         for (Vector2dd &point : p)
         {
             file << point.x() << "," << point.y() << " ";
         }
         file << "\" " << style << "/>" << endl;
    }
    file << "</svg>" << endl;

    file.close();
}



void addSubPolygons (SvgShape *shape, vector<Polygon> &inputPolygons)
{
    cout << shape->type;

    SYNC_PRINT(("addSubPolygons(%s):called\n", SvgShape::getName(shape->type)));
    if (shape->type == SvgShape::POLYGON_SHAPE)
    {
        SvgPolygon *polygon = static_cast<SvgPolygon*>(shape);
        SYNC_PRINT(("adding polygon of %d sides\n", (int)polygon->polygon.size()));
        inputPolygons.push_back(polygon->polygon);
    }
    /*

    if (shape->type == SvgShape::PATH_SHAPE)
    {
        SYNC_PRINT(("Try to add Path\n"));
        SvgPath *path = static_cast<SvgPath*>(shape);
        Polygon p;
        if (path->toPolygon(p) == false)
            SYNC_PRINT(("Not a polygon"));
        else
            SYNC_PRINT(("adding polygon of %d sides\n", (int)p.size()));
        inputPolygons.push_back(p);
    }
    */

    if (shape->type == SvgShape::GROUP_SHAPE)
    {
        SvgGroup *group = static_cast<SvgGroup*>(shape);
        for (SvgShape * s : group->shapes)
        {
            addSubPolygons(s, inputPolygons);
        }
    }
}

Polygon PolygonMakeList(list<Vector2dd> &L)
{
    Polygon p;
    p.reserve(L.size());
    for (auto i = L.begin(); i != L.end(); ++i)
    {
        p.push_back(*i);
    }
    return p;
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
    for(size_t i = 0; i < A.size(); ++i)
    {
        cout << A.getPoint(i).x() <<" " << A.getPoint(i).y() << endl;
    }
}


int PyDi(int m, int n) //5_
{
    auto c = m % n;
    if (m >= 0)
        return c;
    else if (n <0)
    {
        return (-n + c) % n;
    }
    else
    {
        return (n + c) % n;
    }
}

double Dist1(Vector2dd &A, Vector2dd &B)
{
    return abs(A.x() - B.x()) + abs(A.y() - B.y());

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


pair <int, int> CheckPolsIntersect(vector<Polygon> VecPol)
{
auto d = make_pair(0,0);
for (int i = 0; i < VecPol.size(); ++i)
{
    auto j = i - 1;
    while(j >= 0)
    {

        if(PolsIntersect(VecPol[i], VecPol[j]) == 1)
            d = make_pair(i,j);
        --j;

    }
    j = i + 1;
    while(j < VecPol.size())
    {

        if(PolsIntersect(VecPol[i], VecPol[j]) == 1)
            d = make_pair(i,j);
      ++j;
    }


}
}


double AngleOX(Vector2dd &V)
{
    Vector2dd O(0,0);
    Vector2dd OX(0,1);
    if(OrientAreaTwice(O, V, OX) <= 0)
        return (V.x() * OX.x() + V.y() * OX.y()) / V.l2Metric();
    else
    {
        return -2 - (V.x() * OX.x() + V.y() * OX.y()) / V.l2Metric();
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


Vector2dd getTopRightPoint(Polygon &A)
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

int getTopRightIndex(Polygon &A)
{
    list<int> BotLeftA;

    BotLeftA.push_front(0);
    for (int i = 1; i < A.size(); ++i)
    {
        if(A.getPoint(i).y() == A.getPoint(BotLeftA.front()).y())
            BotLeftA.push_back(i);
        else if(A.getPoint(i).y() > A.getPoint(BotLeftA.front()).y())
        {
            BotLeftA.clear();
            BotLeftA.push_front(i);
        }
    }

    auto MaxRight = BotLeftA.front();
    for(auto it = BotLeftA.begin(); it != BotLeftA.end(); ++it)
    {
        if(A.getPoint(MaxRight).x() > A.getPoint(*it).x())
            MaxRight = *it;
    }
    return MaxRight;
}

Polygon innernfpR(Rectangled A, Polygon B) //corner if actually bottom left one
{
    auto C = getTopRightPoint(B);
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

Vector2dd getBotLeftPoint(Polygon &A)
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



int getBotLefIndex(Polygon &A)
{
    list<int> BotLeftA;

    BotLeftA.push_front(0);
    for (int i = 1; i < A.size(); ++i)
    {
        if(A.getPoint(i).y() == A.getPoint(BotLeftA.front()).y())
            BotLeftA.push_back(i);
        else if(A.getPoint(i).y() < A.getPoint(BotLeftA.front()).y())
        {
            BotLeftA.clear();
            BotLeftA.push_front(i);
        }
    }

    auto MaxLeft = BotLeftA.front();
    for(auto it = BotLeftA.begin(); it != BotLeftA.end(); ++it)
    {
        if(A.getPoint(MaxLeft).x() > A.getPoint(*it).x())
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
    for(size_t i = 0; i < A.size(); ++i)
        Edges.push_back( - A.getNextPoint(i) + A.getPoint(i));
    for(size_t i = 0; i < B.size(); ++i)
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
    auto BotA = getBotLeftPoint(A);
    auto BotP = getBotLeftPoint(P);
    return P.translated(BotA - BotP);
}

Polygon nfp1(Polygon &A, Polygon &B)
{
    auto c1 = getBotLefIndex(B);
    auto c2 = getTopRightIndex(A);
    list<Vector2dd> L;

    auto c3 = getTopRightPoint(A)- getBotLeftPoint(B);
    for (auto &v : B)
    v = v + c3;

    L.push_front(getTopRightPoint(B));

    for (int i = 0; i < A.size() + B.size(); ++i)
    {
        if(OrientAreaTwice(GP(B, c1 + 1),GP(B, c1), GP(A, c2)) > 0)
    {
       auto c = L.back() + GP(B, c1) - GP(B, c1 + 1);
        L.push_back(c);
                ++c1;
        for (auto &v : B)
        v = v + c;
    }
    else
    {
          auto c = L.back() + GP(A, c2 + 1) - GP(B, c2);
          ++c2;
          L.push_back(c);
          for (auto &v : B)
          v = v + c;
    }
    }
    if (Dist1(L.front(), L.back()) <= 0.0001)
        L.pop_back();
    auto P = PolygonMakeList(L);
    auto c = getBotLeftPoint(A) - getBotLeftPoint(P);
    for (auto &v : P)
    v = v + c;
    return P;
}

double AreaOfCP(Polygon &A)
{
    auto C = A.getPoint(0);
    auto Result = 0;
    for (size_t i = 1 ; i < A.size() - 1 ; ++i)
    {
        Result += OrientAreaTwice(C, A.getPoint(i), A.getNextPoint(i));
    }
    Result /= 2;
    return Result;
}



bool PointLiesIntConPol(Vector2dd &V, Polygon &A) //Judas
{
    for (size_t i = 0; i < A.size(); ++i)
    {
        if (PointOnSegment(V, A.getPoint(i), A.getNextPoint(i)))
            return false;
    }

   int k = A.size() / 2;
   int right = A.size() - 1;
   int left = 0;
   bool b = 0;
  while(true)
  {
      if (OrientAreaTwice(A.getPoint(k), A.getPoint(k+1), V) >= 0)
          if ( OrientAreaTwice(A.getPoint(0), A.getPoint(k), V) >= 0)
              if (OrientAreaTwice(A.getPoint(k+1), A.getPoint(0), V) >= 0)
        return true;
               else
               {
                    left = k;
                    k = (k + right) / 2;

                }
          else
          {
              right = k;
              k = (k + left) / 2;

          }
      else return false;
      if (right - 1 <= left)
          if (b == 1)
              return false;
      else
          {
              b = 1;
              if (left == k)
                  k = right;
          }
        }
}



bool PointLiesPol(Vector2dd V, Polygon &A)
{
    for (size_t i = 0; i < A.size(); ++i)
    {
        if (PointOnSegment(V, A.getPoint(i), A.getNextPoint(i)))
            return true;
    }

    Vector2dd M(2000, 2005);
    int b = 0;
    for (size_t i = 0; i < A.size(); ++i)
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
    for (size_t i = 0; i < A.size(); ++i)
    {
        if (PointOnSegment(V, A.getPoint(i), A.getNextPoint(i)))
            return false;
    }

    Vector2dd M(2000, 2005);
    int b = 0;
    for (size_t i = 0; i < A.size(); ++i)
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
    if (b >= 1) {
        return true;
    } else {
        return false;
    }

}


Vector2dd SegIntescetionSpecial(Vector2dd &a1, Vector2dd &a2, Vector2dd &b1, Vector2dd &b2) //seg a1, a2 || OX or || OY   seg b1, b2 neither
{
    Vector2dd Result;
    if(a1.y() == a2.y())
    {
        Result.y() =  a1.y();
        if(b1.x() != b2.x())
        {
            double k2 = (b1.y() - b2.y()) / (b1.x() - b2.x());
            double c2 = b1.y() - b1.x() * k2;
            Result.x() =   (a1.y() - c2) / k2;
        }
        else
        {
            Result.x() = b1.x();
        }
    }
    else
    {
        Result.x() =  a1.x();
        if(b1.x() != b2.x())
        {

            double k2 = (b1.y() - b2.y()) / (b1.x() - b2.x());
            double c2 = b1.y() - b1.x() * k2;
            Result.y() = k2* a1.x() + c2;
        }
        else
        {
            Result.y() = b1.y();

        }
    }
    return Result;

}



Vector2dd TargetOfSort;

bool CloserToTarget(Vector2dd V1,Vector2dd V2)
{
    return (Dist1(V1, TargetOfSort) <= Dist1(V2, TargetOfSort));

}

Polygon AddInterPoints2(Polygon A, Polygon RectangledAtually)
{

    list <Vector2dd> L;
    for (auto i = 0; i <A.size(); ++i)
    {
        L.push_back(A.getPoint(i));

        list<Vector2dd> TempL;
        for (auto j = 0; j < 4; ++j)
        {
            if (!PointOnSegment(A.getPoint(i), RectangledAtually.getPoint(j),RectangledAtually.getNextPoint(j)))
            {
                if(SegIntersect( RectangledAtually.getPoint(j), RectangledAtually.getNextPoint(j), A.getPoint(i), A.getNextPoint(i)))
                {
                    TempL.push_back(SegIntescetionSpecial(RectangledAtually.getPoint(j), RectangledAtually.getNextPoint(j), A.getPoint(i), A.getNextPoint(i)));
                }

            }
        }

        TargetOfSort = A.getPoint(i);
        if(!TempL.empty())
        {

            TempL.sort(CloserToTarget);

            while(!TempL.empty())
            {
                L.push_back(TempL.front());
                TempL.pop_front();
            }
        }
    }


    return PolygonMakeList(L);

}





bool BiggaArea(Polygon &A, Polygon &B)
{
    return (AreaOfCP(A) <= AreaOfCP(B));
}

void LazySort(vector <Polygon> &A)
{
    list<Polygon> B;
    for(size_t i = 0; i < A.size(); i++)
    {
        B.push_back(A[i]);
    }
    B.sort(BiggaArea);
    for(size_t i = 0; i < A.size(); i++)
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
        if(v.y() == BotLeft.front().y()) {
            BotLeft.push_back(v);
        } else if(v.y() < BotLeft.front().y())
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

void ActMovePolVec(Polygon &A,  Vector2dd V)
{
    for (Vector2dd &v : A)
        v = v + V;
}
/*

void PolBotIndexZero(Polygon &A)
{
    int i = 0;
    bool b = 1;
    while(b)
    {
        if (A.getPoint(i).y() < A.getPoint(i+1).y())
            while(b)
            {
              if (GP(A,i).y() > GP(A,i -1).y())
                       --i;
              
            }
        else
        {
        ++i;    
        }
    }
}

*/

void BLPlacement(Rectangled A, vector <Polygon> &input)
{

    auto firstnfp = innernfpR(A, input[0]);//actually must check if first polygon suits

    list<int> Placed;
    if(firstnfp.getPoint(0).y() < firstnfp.getPoint(1).y() && firstnfp.getPoint(0).x() < firstnfp.getPoint(2).x())
    {
        ActMovePolVec(input[0], firstnfp.getPoint(0) - getTopRightPoint(input[0]));
        Placed.push_front(0);
    }

    for(int i = 1; i < input.size(); ++i)
    {
        firstnfp = innernfpR(A, input[i]); //same
        list <Polygon> nfps;
        Polygon d;
        //if(firstnfp.getPoint(0).y() < firstnfp.getPoint(1).y() && firstnfp.getPoint(0).x() < firstnfp.getPoint(2).x())
        //{
            for (auto it = Placed.begin(); it != Placed.end(); ++it)
            {
               nfps.push_back(AddInterPoints2(nfp(input[*it], input[i]),firstnfp));

            }
            list<Vector2dd> BestCandidates;

            for (auto it1 = nfps.begin() ; it1 != nfps.end(); ++it1 )
            {
                list<Vector2dd> Candidates;



                for(Vector2dd v : *it1)
                {
                    if(v.x() >= firstnfp.getPoint(0).x() && v.x() <= firstnfp.getPoint(2).x()  && v.y() <= firstnfp.getPoint(2).y() && v.y() >= firstnfp.getPoint(0).y())
                    {

                        bool b = 0;
                        for(auto it2 = nfps.begin(); it2 != nfps.end() && b == 0; ++it2)
                        {
                            if(it2->isInsideConvex(v))
                            b = 1;
                        }


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
                ActMovePolVec(input[i], BestOne(BestCandidates) - getTopRightPoint(input[i]));
                Placed.push_back(i);
            }
            else
            {
                i = 10000;
                cout << i << "cannot be placed" << endl;
//               vector <Polygon> Killah;
//               auto m = nfps.end();
//               --m;
//               for (auto it5 = nfps.begin(); it5 != m; ++it5)
//               {
//                   Killah.push_back(*it5);
//               }
//               input = Killah;
            }


    }
}



Polygon RecToPol(Rectangled A)
{
    Vector2dd C1 = A.llCorner();
    Vector2dd C2 = A.ulCorner();
    Vector2dd C3 = A.urCorner();
    Vector2dd C4 = A.lrCorner();
    Polygon Result = {C1, C2, C3, C4};
    return Result;

}
void Rotate(Polygon &A, double Phi)
{
    for (auto &v : A)
    {
        auto d = v.x() * cos(Phi) - v.y() * sin(Phi);
        v.y() = v.x() * sin(Phi) + v.y() * cos(Phi);
        v.x() = d;
    }

}


void Rotate90(Polygon &A)
{
    for (auto &v : A)
    {
        auto d = v.x();
        v.x() = -v.y();
        v.y() = d;
    }
}






Vector2dd MassCenter(Polygon & A)
{
    Vector2dd Result (0,0);
    for (auto &v : A)
    {
        Result += v;
    }
    return Result/A.size();
}



void LowerMassCenter(Polygon& A)
{

    ActMovePolVec(A,-getBotLeftPoint(A));
    double Phi = 6.28 / 200;
    list <pair<int, double>> L;
    L.push_back(make_pair(0, MassCenter(A).y()));
    ActMovePolVec(A,-MassCenter(A));

    auto B = A;


    for(int i = 1; i < 201; ++i)
    {
        Rotate(A, Phi);
        ActMovePolVec(A,-getBotLeftPoint(A));
        L.push_back(make_pair(i, MassCenter(A).y()));
        ActMovePolVec(A,-MassCenter(A));
    }

    A = B;

    for(int i = 1; i < 3; ++i)
    {
        Rotate90(A);
        ActMovePolVec(A,-getBotLeftPoint(A));
        L.push_back(make_pair(-i, MassCenter(A).y()));
        ActMovePolVec(A,-MassCenter(A));
    }

    L.sort([]( const pair<int,double> &a, const pair<int,double> &b ) { return a.second <= b.second;});
    if(L.front().first > 0)
    {
        A = B;
        Rotate(A, L.front().first * Phi);
    }
    else if(L.front().first == 0)
    {
        A = B;

    }
    else
    {
        A = B;
        for(int i = L.front().first; i <0; ++i)
        Rotate90(A);
    }

}





/*
auto i = it1;
if(i!=nfps.end())
   ++i;
auto j = it1;
if(j!=nfps.begin())
   --j;
while(i!=nfps.end() && b != 1)
{
   if (PointLiesIntConPol(v, *i))
       b = 1;
   ++i;
}
while(j!=nfps.begin() && b != 1)
{
   if (PointLiesIntConPol(v, *j))
       b = 1;
   --j;
}

if (PointLiesIntConPol(v, *j))
   b = 1;
*/




// }
   //  else
//     {
 //        cout << i << "cannot be placed" << endl;

 //    }



/*
auto l1 = getBotLeftIndex(*it1);
auto l2 = l1 + 1;
auto k = ((*it1).size() / 2 + 1);
bool b1 = 0;

for(int counter = 0; counter < k && !b1; ++counter);
{
v = GP((*it1), l1--);
if(v.x() >= firstnfp.getPoint(0).x() && v.x() <= firstnfp.getPoint(2).x()  && v.y() <= firstnfp.getPoint(2).y() && v.y() >= firstnfp.getPoint(0).y())
{
    bool b = 0;
    auto i = it1;
    if(i!=nfps.end())
        ++i;
    auto j = it1;
    if(j!=nfps.begin())
        --j;
    while(i!=n else
{
cout << i << "cannot be placed" << endl;

}fps.end() && b != 1)
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
        b1 = 1;
    }
}
}
b1 = 0;

for(int counter = 0; counter < k && !b1; ++counter);
{
v = GP((*it1), l1--);
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
        b1 = 1;
    }
}
}



}





*/
