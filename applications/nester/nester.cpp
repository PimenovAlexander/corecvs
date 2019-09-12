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

void drawPolygons(list <Polygon> inputPolygons, int h, int w, string bmpname)
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


void drawSvgPolygons(list <Polygon> inputPolygons, int h, int w, string svgName)
{
    std::ofstream file;
    file.open(svgName, ios::out);
    file.imbue(std::locale("C"));

    file << "<svg height=\""<< h <<"\" width=\""<< w <<"\">" << endl;
    size_t i = 0;
    for (auto it = inputPolygons.begin(); it != inputPolygons.end(); ++i, ++it)
    {
        Polygon &p = *it;

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


double distL1(const Vector2dd &v1, const Vector2dd &v2)
{
    return (abs (v1.x() - v2.x()) + abs(v1.y() - v2.y()));
}


bool isInteriorROConvexPolBinSearch(const corecvs ::Vector2dd &point, const corecvs :: Polygon &A) //RO means right-oriented, bs used cause no-fit-polygons have quite a few vertexes
{
    int len = A.size();
    Vector2dd O = A.getPoint(0);
    if (distL1(point, O) < EPSIL)
        return false;

    int left = 1;
    int mid = A.size() / 2;
    int right = A.size() - 1;

    Vector2dd diff = point - O;
    Vector2dd normal;

    while(right - left > 1)
    {
        normal = (A.getPoint(mid) - O).rightNormal();
        if ((diff & normal) >= 0)
        {
            left = mid;
            mid = (right + mid) / 2;
        }
        else
        {
            right = mid;
            mid = (mid + left) / 2;
        }

    }
    Vector2dd leftP = A.getPoint(left);
    Vector2dd rightP = A.getPoint(right);
    normal = ( rightP - leftP).rightNormal();
    if((normal & (point - leftP)) <= 0) {
        return false;
    }
    else {
        if(((A.getPoint(1) - O).rightNormal() & diff) > EPSIL)
        {
            if(((A.getPoint(len-1) - O).leftNormal() & diff) > EPSIL){
                return true;
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }
    }

}


bool isInteriorConvexPol(const corecvs ::Vector2dd &point, const corecvs :: Polygon &A) //discarding an idea of bs
{
    double oldsign = (A.getPoint(1) - A.getPoint(0)).rightNormal() & (point - A.getPoint(0)); //need to initialized
    int len = A.size();
    for (int i = 1; i < len; i++)
    {
        const Vector2dd &curr = A.getPoint(i);
        const Vector2dd &next = A.getNextPoint(i);
        const Vector2dd normal = (next - curr).rightNormal();
        Vector2dd diff = point - curr;
        double sign = diff & normal;
        if (oldsign >= 0  && sign <= 0) //may be EPSIL better
            return false;
        if (oldsign <= 0  && sign >= 0)
            return false;
        oldsign = sign;
    }
    return true;
}

bool hasBiggerLOArg(const Vector2dd &v1, const Vector2dd &v2) // antiClockwise to vector -(OX)
{
    double minusRadian1 = v1.x()/ v1.l2Metric();
    double minusRadian2 = v2.x()/ v2.l2Metric();

    if(v1.y() <= 0) //like the most complex moment in all thing
        minusRadian1 = 2 - minusRadian1;
    if(v2.y() <= 0) //same
        minusRadian2 = 2 - minusRadian2;

    return (minusRadian1 > minusRadian2);

}


int pythonDir(int m, int n)
{
    int r = m % n;
    if (m >= 0)
        return r;
    else if (n < 0){
        return (-n + r) % n;
    }
    else{
        return (n + r) % n;
    }
}

Vector2dd getPointByGenInd(const Polygon &A, int i)
{
    int r = pythonDir(i, (int)A.size());
    return A[r];
}

int getTopRightIndex(const Polygon &A)
{
    int i = 0;
    int result = 0;
    while(i < A.size())
    {
        if(A.getPoint(i).y() > A.getPoint(result).y())
        {
            result = i;
        }
        else if (A.getPoint(i).y() == A.getPoint(result).y())
        {
            if (A.getPoint(i).x() > A.getPoint(result).x())
                result = i;

        }
        ++i;
    }
    return result;

}

int getBotLeftIndex(const Polygon &A)
{
    int i = 0;
    int result = 0;
    while(i < A.size())
    {
        if(A.getPoint(i).y() < A.getPoint(result).y())
        {
            result = i;
        }
        else if (A.getPoint(i).y() == A.getPoint(result).y())
        {
            if (A.getPoint(i).x() < A.getPoint(result).x())
                result = i;

        }
        ++i;
    }
    return result;

}


Polygon convexNFP(const Polygon &A, const Polygon &B)
//A walks around B, begining in his bottom-left point going counterclockwise, top-right point of A leaves the trace
//assuming both Polygons are RO
{

    Polygon conNFP;
    int i = getTopRightIndex(A);
    int j = getBotLeftIndex(B);
    conNFP.push_back(getPointByGenInd(B, j));

    int length1 = (int)A.size() + i;
    int length2 = (int)B.size() + j;
    int place = 0;

    while(i < length1 && j < length2 )
    {
        Vector2dd candidateFromA = -getPointByGenInd(A, i + 1) + getPointByGenInd(A, i);
        Vector2dd candidateFromB = getPointByGenInd(B, j + 1) - getPointByGenInd(B, j);

        if (hasBiggerLOArg(candidateFromA, candidateFromB))
        {
            conNFP.push_back(conNFP[place] + candidateFromB);
            ++place;
            ++j;
        }
        else
        {
            conNFP.push_back(conNFP[place] + candidateFromA);
            ++place;
            ++i;
        }

    }

    while(i < length1)
    {
        Vector2dd candidateFromA = -getPointByGenInd(A, i + 1) + getPointByGenInd(A,i);
        conNFP.push_back(conNFP[place] + candidateFromA);
        ++place;
        ++i;
    }

    while(j < length2)
    {
        Vector2dd candidateFromB = getPointByGenInd(B, j + 1) - getPointByGenInd(B, j);
        conNFP.push_back(conNFP[place] + candidateFromB);
        ++place;
        ++j;
    }
    conNFP.pop_back(); // not to dublicate first vertex
    return conNFP;

}




Rectangled innerFitPolygon(const Polygon &A, const Rectangled &R) // all RO
{
    double leftOfA = A.getPoint(0).x();
    double rightOfA = A.getPoint(0).x();
    double topOfA = A.getPoint(0).y();
    double botOfA = A.getPoint(0).y();
    double markedPointAbciss = A.getPoint(0).x(); //marked is top-right of A
    for(size_t i = 1; i < A.size(); ++i)
    {
        double xCandidate = A.getPoint(i).x();
        double yCandidate = A.getPoint(i).y();
        if(yCandidate > topOfA) {
            topOfA = yCandidate;
            markedPointAbciss = xCandidate;
        }
        else if (yCandidate == topOfA){
            if(xCandidate > markedPointAbciss){
                markedPointAbciss = xCandidate;
            }
        }
        else if (yCandidate < botOfA) {
            botOfA = yCandidate;
        }


        if(xCandidate > rightOfA) {
            rightOfA = xCandidate;
        }
        else if (xCandidate < leftOfA) {
            leftOfA = xCandidate;
        }
    }
    Vector2dd cornerOfIFP = {markedPointAbciss - leftOfA, topOfA - botOfA};
    cornerOfIFP += R.corner;


    Vector2dd widthHeightOfIFP = {R.width()- rightOfA + leftOfA, R.height() - topOfA + botOfA};
    return Rectangled(cornerOfIFP.x(), cornerOfIFP.y(), widthHeightOfIFP.x(), widthHeightOfIFP.y()); //x,y,w,h
}

bool pointDoRefRec(Vector2dd const &point, Rectangled const &R) //RO Rect
{
    double up = R.corner.y() + R.height();
    double low = R.corner.y();
    double left = R.corner.x();
    double right = R.corner.x() + R.width();
    return (point.x() >= left && point.x() <= right && point.y() >= low && point.y() <= up);

}


Polygon polFromRect(Rectangled &R) //rectangled is RO and corner is ll one
{
    return {R.ulCorner(), R.llCorner(), R.lrCorner(), R.urCorner()};
}


void bottomLeftPlacement(list <corecvs :: Polygon> &inp, corecvs :: Rectangled &Bin)
{
    int inpNumber = 0;
    //suppose initialisation is ok
    auto it = inp.begin();
    list <Polygon> placedPolygons;
    Rectangled innerFP = innerFitPolygon(*it, Bin);

    if (innerFP.height() >= 0 && innerFP.width() >= 0) {
        Vector2dd topRightInitiate = it->getPoint(getTopRightIndex(*it));
        it->translate(innerFP.corner - topRightInitiate);
        placedPolygons.push_back(*it);
    }
    else {
        cout << endl << "0 polygon can't be placed" << endl;
    }

    ++it;
    ++inpNumber;


    while(it != inp.end())
    {
        innerFP = innerFitPolygon(*it, Bin);
        list <Polygon> currNFPs;
        for (Polygon c : placedPolygons){
            currNFPs.push_back(convexNFP(*it, c));
        }
        Polygon Candidates; //cause there is getBotLeft function

        for (auto it2 = currNFPs.begin(); it2 != currNFPs.end(); ++it2)
        {

            size_t i  = 0;
            while(i < it2->size())
            {
                auto Candidate = it2->getPoint(i);
                bool b = 0;
                if (pointDoRefRec(Candidate, innerFP))
                {
                    auto it3 = currNFPs.begin();
                    while(it3 != currNFPs.end() && b == 0)
                    {
                        if(isInteriorConvexPol(Candidate, *it3)){
                            b = 1;
                        }
                        ++it3;
                    }
                    if (b == 0){
                        Candidates.push_back(Candidate);
                    }
                }
                ++i;
            }
        }
        Polygon ifpClone = polFromRect(innerFP);

        for (Vector2dd &p : ifpClone)
        {
            bool b = 0;
            auto it3 = currNFPs.begin();
            while(it3 != currNFPs.end() && b == 0)
            {
                if(isInteriorConvexPol(p, *it3)){
                    b = 1;
                }
                ++it3;
            }
            if (b == 0){
                Candidates.push_back(p);
            }
        }


        if (Candidates.size() > 0)
        {
            auto where = Candidates.getPoint(getBotLeftIndex(Candidates));
            it->translate(where - it->getPoint(getTopRightIndex(*it)));
            placedPolygons.push_back(*it);
        }
        else {
            cout <<endl << inpNumber << " polygon can't be placed";

        }
        ++it;
        ++inpNumber;
    }

}

double OrientAreaTwice (const Vector2dd &a, const Vector2dd &b, const Vector2dd &c)
{

    return (a.x() - b.x()) * (c.y() - a.y()) - (a.x() - c.x()) * (b.y() - a.y());

}

bool isClockOrP(const Polygon &A)
{
    if(OrientAreaTwice(A.getPoint(0), A.getPoint(1), A.getPoint(2)) >= 0)
        return true;
    else return false;

}

void doClockOrP(Polygon &A) // do not look at realisation pls, to be redone
{
    if(!isClockOrP(A))
    {
        Polygon B;
        for(int i = A.size() - 1; i >= 0; --i){
            B.push_back(A.getPoint(i));
        }
        A = B;
    }

}

void rotatePolAngle(Polygon &A, double Phi) // to be redone, saw smthing useful for rotating in lib
{
    for (auto &v : A)
    {
        auto d = v.x() * cos(Phi) - v.y() * sin(Phi);
        v.y() = v.x() * sin(Phi) + v.y() * cos(Phi);
        v.x() = d;
    }
}


Vector2dd massCenter(const Polygon &A)
{
    Vector2dd Result (0,0);
    for (auto &v : A)
    {
        Result += v;
    }
    return Result/A.size();
}


void lowerMassCenter(Polygon& A)
{
    A.translate(-A.getPoint(getBotLeftIndex(A)));
    double phi = 3.14 / 320.0;
    double best = 0;
    double bestMassCenter = massCenter(A).y();


    A.translate(-massCenter(A));
    Polygon B = A;


    for( double i = 1; i < 320 ;++i)
    {
        rotatePolAngle(B, phi);
        B.translate(-massCenter(B));
        Polygon C = B;
        C.translate(-C.getPoint(getBotLeftIndex(C)));
        if (bestMassCenter > massCenter(C).y() )
        {
            best = i;
            bestMassCenter = massCenter(C).y();
        }

    }

    if (best != 0)
        rotatePolAngle(A, best * phi);

}






void bottomLeftPlacementProtected(list <corecvs :: Polygon> &inp, corecvs :: Rectangled &Bin, int i = 1)
{
    bool seemsOK = 1;
    if(i >= 1)
    {
        for (Polygon &p : inp)
        {
            if(!isClockOrP(p))
                doClockOrP(p);
        }
        if (Bin.height() < 0 || Bin.width() < 0)
        {
            cout << "Bin is wrong oriented";
            seemsOK = 0;
        }

    }
    if(i > 1)
    {
        inp.sort([](Polygon &A, Polygon &B) -> bool
        {
        return (A.area()) > (B.area());
        });

    }

    if(i > 2)
    {
        for (Polygon &p : inp){
            lowerMassCenter(p);
        }

    }

    if (seemsOK){
        bottomLeftPlacement(inp, Bin);
    }
}


void showPolygon(const Polygon &A)
{
    cout << endl;
    for(size_t i = 0; i < A.size(); ++i)
    {
        cout << A.getPoint(i).x() <<" " << A.getPoint(i).y() << endl;
    }
}


