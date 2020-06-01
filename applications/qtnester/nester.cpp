#include <fstream>
#include <list>
#include "core/utils/utils.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/svgLoader.h"
#include <fstream>
#include <list>
#include "core/utils/utils.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/svgLoader.h"
#include <core/reflection/commandLineSetter.h>
#include <algorithm>
#include <core/buffers/bufferFactory.h>
#include "core/fileformats/dxf_support/dxfLoader.h"

#define EPSIL 0.0001

using namespace corecvs;
using namespace std;
/** =========================================== **/
void drawPolygons(vector<Polygon> inputPolygons,
                  int h, int w, string bmpname)
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

void drawPolygons(list <Polygon> inputPolygons,
                  int h, int w, string bmpname)
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

bool isInteriorROConvexPolBinSearch(const Vector2dd &point,
                                    const Polygon &A) //RO means right-oriented, bs used cause no-fit-polygons have quite a few vertexes
{
    size_t len = A.size();
    Vector2dd O = A.getPoint(0);
    if (distL1(point, O) < EPSIL)
        return false;
    size_t left = 1;
    size_t mid = A.size() / 2;
    size_t right = A.size() - 1;
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
    return ((normal & (point - leftP)) > 0 &&
           ((A.getPoint(1) - O).rightNormal() & diff) > EPSIL &&
           ((A.getPoint(len - 1) - O).leftNormal() & diff) > EPSIL);
}

bool isInteriorConvexPol(const Vector2dd &point,
                         const Polygon &A) //if discarding an idea of bs
{
    double oldsign = (A.getPoint(1) - A.getPoint(0)).rightNormal() & (point - A.getPoint(0)); //need to initialized
    int len = A.size();
    for (size_t i = 1; i < len; i++)
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

bool hasBiggerLOArg(const Vector2dd &v1,
                    const Vector2dd &v2) // antiClockwise to vector -(OX)
{
    double minusRadian1 = v1.x()/ v1.l2Metric();
    double minusRadian2 = v2.x()/ v2.l2Metric();

    if(v1.y() <= 0) // inmportant moment
        minusRadian1 = 2 - minusRadian1;
    if(v2.y() <= 0)
        minusRadian2 = 2 - minusRadian2;
    return (minusRadian1 < minusRadian2);
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
    return A[pythonDir(i, (int)A.size())];
}

size_t getTopRightIndex(const Polygon &A)
{
    size_t result = 0;
    for (size_t i = 0; i < A.size(); ++i)
    {
        if(A.getPoint(i).y() > A.getPoint(result).y())
        {
            result = i;
        } else if (A.getPoint(i).y() == A.getPoint(result).y())
        {
            if (A.getPoint(i).x() > A.getPoint(result).x())
                result = i;
        }
    }
    return result;
}

int getBotLeftIndex(const Polygon &A)
{
    size_t result = 0;
    for (size_t i = 0; i < A.size(); ++i)
    {
        if (A.getPoint(i).y() < A.getPoint(result).y())
        {
            result = i;
        }
        else if (A.getPoint(i).y() == A.getPoint(result).y() &&
                 A.getPoint(i).x() < A.getPoint(result).x())
        {
            result = i;
        }
    }
    return result;
}

Polygon convexNFPSaturated(const Polygon &A, const Polygon &B, size_t steps = 4)
//A walks around B, begining in his bottom-left point going counterclockwise, top-right point of A leaves the trace
//assuming both Polygons are RO
{
    Polygon convexNFPSaturated;
    size_t i = getTopRightIndex(A);
    size_t j = getBotLeftIndex(B);
    convexNFPSaturated.push_back(getPointByGenInd(B, j));
    size_t length1 = A.size() + i;
    size_t length2 = B.size() + j;
    size_t place = 0;
    while(i < length1 && j < length2 )
    {
        Vector2dd candidateFromA = -getPointByGenInd(A, i + 1) + getPointByGenInd(A, i);
        Vector2dd candidateFromB = getPointByGenInd(B, j + 1) - getPointByGenInd(B, j);

        if (hasBiggerLOArg(candidateFromA, candidateFromB))
        {
            for (int k = 1; k <= steps; ++k)
            convexNFPSaturated.push_back(convexNFPSaturated[place] + (double)k * 0.2 * candidateFromA);
            convexNFPSaturated.push_back(convexNFPSaturated[place] + candidateFromA);

            place += 5;
            ++i;
        }
        else
        {
            for (int k = 1; k <= steps; ++k)
            convexNFPSaturated.push_back(convexNFPSaturated[place] + (double)k * 0.2 * candidateFromB);
            convexNFPSaturated.push_back(convexNFPSaturated[place] + candidateFromB);

            place += 5;
            ++j;
        }
    }

    for (; i < length1; ++i, ++place)
    {
        Vector2dd candidateFromA = -getPointByGenInd(A, i + 1) + getPointByGenInd(A,i);
        convexNFPSaturated.push_back(convexNFPSaturated[place] + candidateFromA);
    }
    for (; j < length2; ++j, ++place)
    {
        Vector2dd candidateFromB = getPointByGenInd(B, j + 1) - getPointByGenInd(B, j);
        convexNFPSaturated.push_back(convexNFPSaturated[place] + candidateFromB);
    }
    convexNFPSaturated.pop_back(); // not to dublicate first vertex
    return convexNFPSaturated;
}

Polygon convexNFP(const Polygon &A, const Polygon &B)
//A walks around B, begining in his bottom-left point going counterclockwise, top-right point of A leaves the trace
//assuming both Polygons are RO
{
    Polygon convexNFP;
    size_t i = getTopRightIndex(A);
    size_t j = getBotLeftIndex(B);
    convexNFP.push_back(getPointByGenInd(B, j));
    size_t length1 = A.size() + i;
    size_t length2 = B.size() + j;
    size_t place = 0;
    while(i < length1 && j < length2 )
    {
        Vector2dd candidateFromA = -getPointByGenInd(A, i + 1) + getPointByGenInd(A, i);
        Vector2dd candidateFromB = getPointByGenInd(B, j + 1) - getPointByGenInd(B, j);

        if (hasBiggerLOArg(candidateFromA, candidateFromB))
        {
            convexNFP.push_back(convexNFP[place] + candidateFromA);
            ++place;
            ++i;
        }
        else
        {
            convexNFP.push_back(convexNFP[place] + candidateFromB);
            ++place;
            ++j;
        }
    }

    for (; i < length1; ++i, ++place)
    {
        Vector2dd candidateFromA = -getPointByGenInd(A, i + 1) + getPointByGenInd(A,i);
        convexNFP.push_back(convexNFP[place] + candidateFromA);
    }
    for (; j < length2; ++j, ++place)
    {
        Vector2dd candidateFromB = getPointByGenInd(B, j + 1) - getPointByGenInd(B, j);
        convexNFP.push_back(convexNFP[place] + candidateFromB);
    }
    convexNFP.pop_back(); // not to dublicate first vertex
    return convexNFP;
}

Rectangled innerFitPolygon(const Polygon &A,
                           const Rectangled &R) // all RO
{
    double leftOfA = A.getPoint(0).x();
    double rightOfA = A.getPoint(0).x();
    double topOfA = A.getPoint(0).y();
    double botOfA = A.getPoint(0).y();
    double markedPointAbciss = A.getPoint(0).x(); //marked vertex is top-right of A
    for (size_t i = 1; i < A.size(); ++i)
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

bool pointDoRefRec(Vector2dd const &point,
                   Rectangled const &R) //RO Rect
{
    double up = R.corner.y() + R.height();
    double low = R.corner.y();
    double left = R.corner.x();
    double right = R.corner.x() + R.width();
    return (point.x() >= left && point.x() <= right && point.y() >= low && point.y() <= up);
}

Polygon polFromRec(const Rectangled &R) //rectangled is RO and corner is ll one
{
    return {R.ulCorner(), R.llCorner(), R.lrCorner(), R.urCorner()};
}

double OrientAreaTwice (const Vector2dd &a,
                        const Vector2dd &b,
                        const Vector2dd &c)
{
    return (a.x() - b.x()) * (c.y() - a.y()) - (a.x() - c.x()) * (b.y() - a.y());
}

bool isClockOrP(const Polygon &A)
{
    return (OrientAreaTwice(A.getPoint(0), A.getPoint(1), A.getPoint(2)) >= 0);
}


void doClockOrP(Polygon &A)
{
    if(!isClockOrP(A))
    {
        Polygon B;
        for(int i = (int)A.size() - 1; i >= 0; --i){
            B.push_back(A.getPoint(i));
        }
        A = B;
    }
}

void rotatePolAngle(Polygon &A, double Phi)
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
    for (auto &v : A){
        Result += v;
    }
    return Result/A.size();
}

void lowerMassCenter(Polygon& A)
{
    A.translate(-A.getPoint(getBotLeftIndex(A)));
    double phi = 3.1415 / 320.0;
    double best = 0;
    double bestMassCenter = massCenter(A).y();
    A.translate(-massCenter(A));
    Polygon B = A;

    for (double i = 1; i < 320 ;++i)
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


void bottomLeftPlacement(list <corecvs :: Polygon> &inp,
                         corecvs :: Rectangled &bin)
{
    size_t inpNumber = 0;
    //suppose initialisation is ok
    auto it = inp.begin();
    list <Polygon> placedPolygons;
    Rectangled innerFP = innerFitPolygon(*it, bin);

    if (innerFP.height() >= 0 && innerFP.width() >= 0) {
        Vector2dd topRightInitiate = it->getPoint(getTopRightIndex(*it));
        it->translate(innerFP.corner - topRightInitiate);
        placedPolygons.push_back(*it);
    }
    else {
        cout << endl << "0 polygon can't be placed" << endl;
    }
    auto prevIt = it;
    ++it;
    ++inpNumber;

    for (;it != inp.end();++it, ++prevIt, ++inpNumber)
    {
        innerFP = innerFitPolygon(*it, bin);
        list <Polygon> nfpList;
        for (Polygon c : placedPolygons){
            nfpList.push_back(convexNFPSaturated(*it, c));
        }
        Polygon Candidates; //cause there is getBotLeft function

        for (auto it2 = nfpList.begin(); it2 != nfpList.end(); ++it2)
        {
            for (size_t i = 0; i < it2->size(); ++i)
            {
                auto Candidate = it2->getPoint(i);
                bool b = 0;
                if (pointDoRefRec(Candidate, innerFP))
                {
                    auto it3 = nfpList.begin();
                    while(it3 != nfpList.end() && b == 0)
                    {
                        if(isInteriorConvexPol(Candidate, *it3)){
                            b = 1;
                        }
                        ++it3;
                    }
                    if (b == 0)
                    {
                        Candidates.push_back(Candidate);
                    }
                }
            }
        }
        Polygon ifpClone = polFromRec(innerFP);

        for (Vector2dd &p : ifpClone)
        {
            bool b = 0;

            for (auto it3 = nfpList.begin(); it3 != nfpList.end() && b == 0; ++it3)
            {
                if(isInteriorConvexPol(p, *it3)){
                    b = 1;
                }
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
    }
}



void bruteBL(list <corecvs :: Polygon> &inp,
             corecvs :: Rectangled &bin, size_t rotatesAmount)
{
    size_t inpNumber = 0;
    //suppose initialisation is ok
    auto it = inp.begin();
    list <Polygon> placedPolygons;
    Rectangled innerFP = innerFitPolygon(*it, bin);

    if (innerFP.height() >= 0 && innerFP.width() >= 0) {
        Vector2dd topRightInitiate = it->getPoint(getTopRightIndex(*it));
        it->translate(innerFP.corner - topRightInitiate);
        placedPolygons.push_back(*it);
    }
    else {
        cout << endl << "0 polygon can't be placed" << endl;
    }
    auto prevIt = it;
    ++it;
    ++inpNumber;

    for (;it != inp.end();++it, ++prevIt, ++inpNumber)
    {
        auto curPolygon = *it;
        vector<pair<Vector2dd, int>> Candidates;
        for (int c = 0; c < rotatesAmount; ++c) {
            auto curPolygon = *it;
            double angle = ((double)c) * 6.29 / ((double)rotatesAmount);
            rotatePolAngle(curPolygon, angle);
            innerFP = innerFitPolygon(curPolygon, bin);
            list <Polygon> nfpList;
            for (Polygon c : placedPolygons){
                nfpList.push_back(convexNFPSaturated(curPolygon, c));
            }

            for (auto it2 = nfpList.begin(); it2 != nfpList.end(); ++it2) {
                for (size_t i = 0; i < it2->size(); ++i) {
                    auto Candidate = it2->getPoint(i);
                    bool b = 0;
                    if (pointDoRefRec(Candidate, innerFP)) {
                        auto it3 = nfpList.begin();
                        while(it3 != nfpList.end() && b == 0) {
                            if(isInteriorConvexPol(Candidate, *it3)) {
                                b = 1;
                            }
                            ++it3;
                        }
                        if (b == 0) {
                            Candidates.push_back({Candidate, c});
                        }
                    }
                }
            }
            Polygon ifpClone = polFromRec(innerFP);

            for (Vector2dd &p : ifpClone)
            {
                bool b = 0;

                for (auto it3 = nfpList.begin(); it3 != nfpList.end() && b == 0; ++it3)
                {
                    if(isInteriorConvexPol(p, *it3)){
                        b = 1;
                    }
                }
                if (b == 0){
                    Candidates.push_back({p, c});
                }
            }

        }
        if (Candidates.size() > 0) {
            double minUpperVertValue = 100000;
            double xcord;
            size_t goodRotation = -1;
            for (auto &pairPI : Candidates) {
                if (minUpperVertValue > pairPI.first.y()) {
                    minUpperVertValue  = pairPI.first.y();
                    xcord =  pairPI.first.x();
                    goodRotation = pairPI.second;
                }
            }
            Vector2dd where (xcord, minUpperVertValue);
            rotatePolAngle(*it, ((double)goodRotation) * 6.29 / ((double)rotatesAmount));
            it->translate(where - it->getPoint(getTopRightIndex(*it)));
            placedPolygons.push_back(*it);
        }
        else {
            cout <<endl << inpNumber << " polygon can't be placed";

        }
    }
}

double heightOfPolygon(const corecvs::Polygon& pol) {
    return abs(getPointByGenInd(pol, getTopRightIndex(pol)).y() - getPointByGenInd(pol, getBotLeftIndex(pol)).y());
}

void bruteHeightBL(list <corecvs :: Polygon> &inp,
                   corecvs :: Rectangled &bin, size_t rotatesAmount,
                   double heightCost)
{
    size_t inpNumber = 0;
    //suppose initialisation is ok
    auto it = inp.begin();
    list <Polygon> placedPolygons;
    Rectangled innerFP = innerFitPolygon(*it, bin);

    if (innerFP.height() >= 0 && innerFP.width() >= 0) {
        Vector2dd topRightInitiate = it->getPoint(getTopRightIndex(*it));
        it->translate(innerFP.corner - topRightInitiate);
        placedPolygons.push_back(*it);
    }
    else {
        cout << endl << "0 polygon can't be placed" << endl;
    }
    auto prevIt = it;
    ++it;
    ++inpNumber;

    for (;it != inp.end();++it, ++prevIt, ++inpNumber)
    {
        auto curPolygon = *it;
        vector<pair<Vector2dd, int>> Candidates;
        Polygon lowerMassCands;
        for (int c = 0; c < rotatesAmount; ++c) {
            auto curPolygon = *it;
            double angle = ((double)c) * 6.29 / ((double)rotatesAmount);
            rotatePolAngle(curPolygon, angle);
            innerFP = innerFitPolygon(curPolygon, bin);
            list <Polygon> nfpList;
            for (Polygon c : placedPolygons){
                nfpList.push_back(convexNFPSaturated(curPolygon, c));
            }

            for (auto it2 = nfpList.begin(); it2 != nfpList.end(); ++it2) {
                for (size_t i = 0; i < it2->size(); ++i) {
                    auto Candidate = it2->getPoint(i);
                    bool b = 0;
                    if (pointDoRefRec(Candidate, innerFP)) {
                        auto it3 = nfpList.begin();
                        while(it3 != nfpList.end() && b == 0) {
                            if(isInteriorConvexPol(Candidate, *it3)) {
                                b = 1;
                            }
                            ++it3;
                        }
                        if (b == 0) {
                            Candidates.push_back({Candidate, c});
                            if (c == 0)
                                lowerMassCands.push_back(Candidate);
                        }
                    }
                }
            }
            Polygon ifpClone = polFromRec(innerFP);

            for (Vector2dd &p : ifpClone)
            {
                bool b = 0;

                for (auto it3 = nfpList.begin(); it3 != nfpList.end() && b == 0; ++it3)
                {
                    if(isInteriorConvexPol(p, *it3)){
                        b = 1;
                    }
                }
                if (b == 0){
                    Candidates.push_back({p, c});
                    if (c == 0)
                        lowerMassCands.push_back(p);
                }
            }

        }
        if (Candidates.size() > 0) {
            double minUpperVertValue = 100000;
            double xcord;
            size_t goodRotation = -1;
            for (auto &pairPI : Candidates) {
                if (minUpperVertValue > pairPI.first.y()) {
                    minUpperVertValue  = pairPI.first.y();
                    xcord =  pairPI.first.x();
                    goodRotation = pairPI.second;
                }
            }
            Vector2dd where (xcord, minUpperVertValue);
            auto where2 = lowerMassCands.getPoint(getBotLeftIndex(lowerMassCands));
            if (abs(where2.y() - where.y()) / heightOfPolygon(*it) < heightCost) {
                it->translate(where2 - it->getPoint(getTopRightIndex(*it)));
                placedPolygons.push_back(*it);
            } else {
            rotatePolAngle(*it, ((double)goodRotation) * 6.29 / ((double)rotatesAmount));
            it->translate(where - it->getPoint(getTopRightIndex(*it)));
            placedPolygons.push_back(*it);
            }
        }
        else {
            cout <<endl << inpNumber << " polygon can't be placed";

        }
    }
}

void bruteMassBL (list <corecvs :: Polygon> &inp,
                         corecvs :: Rectangled &bin, size_t rotatesAmount,
                  double massCost)
{
    size_t inpNumber = 0;
    //suppose initialisation is ok
    auto it = inp.begin();
    list <Polygon> placedPolygons;
    Rectangled innerFP = innerFitPolygon(*it, bin);

    if (innerFP.height() >= 0 && innerFP.width() >= 0) {
        Vector2dd topRightInitiate = it->getPoint(getTopRightIndex(*it));
        it->translate(innerFP.corner - topRightInitiate);
        placedPolygons.push_back(*it);
    }
    else {
        cout << endl << "0 polygon can't be placed" << endl;
    }
    auto prevIt = it;
    ++it;
    ++inpNumber;

    for (;it != inp.end();++it, ++prevIt, ++inpNumber)
    {
        auto curPolygon = *it;
        vector<pair<Vector2dd, int>> Candidates;
        Polygon lowerMassCands;
        for (int c = 0; c < rotatesAmount; ++c) {
            auto curPolygon = *it;
            double angle = ((double)c) * 6.29 / ((double)rotatesAmount);
            rotatePolAngle(curPolygon, angle);
            innerFP = innerFitPolygon(curPolygon, bin);
            list <Polygon> nfpList;
            for (Polygon c : placedPolygons){
                nfpList.push_back(convexNFPSaturated(curPolygon, c));
            }

            for (auto it2 = nfpList.begin(); it2 != nfpList.end(); ++it2) {
                for (size_t i = 0; i < it2->size(); ++i) {
                    auto Candidate = it2->getPoint(i);
                    bool b = 0;
                    if (pointDoRefRec(Candidate, innerFP)) {
                        auto it3 = nfpList.begin();
                        while(it3 != nfpList.end() && b == 0) {
                            if(isInteriorConvexPol(Candidate, *it3)) {
                                b = 1;
                            }
                            ++it3;
                        }
                        if (b == 0) {
                            Candidates.push_back({Candidate, c});
                            if (c == 0)
                                lowerMassCands.push_back(Candidate);
                        }
                    }
                }
            }
            Polygon ifpClone = polFromRec(innerFP);

            for (Vector2dd &p : ifpClone)
            {
                bool b = 0;

                for (auto it3 = nfpList.begin(); it3 != nfpList.end() && b == 0; ++it3)
                {
                    if(isInteriorConvexPol(p, *it3)){
                        b = 1;
                    }
                }
                if (b == 0){
                    Candidates.push_back({p, c});
                    if (c == 0)
                        lowerMassCands.push_back(p);
                }
            }

        }
        if (Candidates.size() > 0) {
            double minUpperVertValue = 100000;
            double xcord;
            size_t goodRotation = -1;
            for (auto &pairPI : Candidates) {
                if (minUpperVertValue > pairPI.first.y()) {
                    minUpperVertValue  = pairPI.first.y();
                    xcord =  pairPI.first.x();
                    goodRotation = pairPI.second;
                }
            }
            Vector2dd where (xcord, minUpperVertValue);
            auto goodPolygon = *it;
            rotatePolAngle(goodPolygon, ((double)goodRotation) * 6.29 / ((double)rotatesAmount));
            goodPolygon.translate(where - goodPolygon.getPoint(getTopRightIndex(*it)));

            if (abs(massCenter(*it).y() - massCenter(goodPolygon).y()) / heightOfPolygon(*it) < massCost) {
                auto where2 = lowerMassCands.getPoint(getBotLeftIndex(lowerMassCands));
                it->translate(where2 - it->getPoint(getTopRightIndex(*it)));
                placedPolygons.push_back(*it);
            } else {
            rotatePolAngle(*it, ((double)goodRotation) * 6.29 / ((double)rotatesAmount));
            it->translate(where - it->getPoint(getTopRightIndex(*it)));
            placedPolygons.push_back(*it);
            }
        }
        else {
            cout <<endl << inpNumber << " polygon can't be placed";

        }
    }
}

void showPolygon(const Polygon &A)
{
    cout << endl;
    for(size_t i = 0; i < A.size(); ++i){
        cout << A.getPoint(i).x() <<" " << A.getPoint(i).y() << endl;
    }
}

double ro(Vector2dd A, Vector2dd B) {
    return sqrt((A.x() - B.x()) * (A.x() - B.x()) + (A.y() - B.y()) * (A.y() - B.y()));
}

Polygon getHomotheticPolygon(Polygon& p, double epsil) {
    auto mc = massCenter(p);
    double minHeight = 1000000;
    for (size_t i = 0; i < p.size(); ++i) {
         auto first = getPointByGenInd(p, i);
         auto second =  getPointByGenInd(p, i + 1);
         double edge = ro(first, second);
         double areatwice = abs(OrientAreaTwice(mc, first, second));
         double minHeightCandidate = areatwice / edge;
         minHeight = minHeight > minHeightCandidate ? minHeightCandidate : minHeight;
    }
    minHeight /= 2;
    double koef = (minHeight + epsil)/ minHeight;
    Polygon result;
    for (auto& vert: p) {
        result.push_back(mc + (vert - mc) * koef);
    }

    return result;
}

void vinilPlacementNester (list<Polygon> &inputList, Rectangled &bin,
                           double epsil, bool lowTheMasses, int whichPlacement,
                           double costParameter, size_t rotations) {
    if (bin.height() < 0 || bin.width() < 0)
    {
        cout << "Bin is wrong oriented";
        return;
    }
    for (Polygon &p : inputList) {
        if(!isClockOrP(p))
            doClockOrP(p);
    }
    inputList.sort([](Polygon &A, Polygon &B) -> bool
    { return (A.area()) > (B.area()); } );
    if (lowTheMasses) {
    for (auto &pol : inputList)
        lowerMassCenter(pol);
    }
    if (epsil == 0) {
        switch (whichPlacement) {
        case 1: bruteHeightBL(inputList, bin, rotations , costParameter); break;
        case 2: bruteMassBL(inputList, bin, rotations, costParameter);
        }
        return;
    }
    auto sz = inputList.size();
    list<Polygon> inputListTransofrmed;
    auto it = inputList.begin();
    for (auto i = 0; i < sz; ++i) {
        inputListTransofrmed.push_back(getHomotheticPolygon(*it, epsil));
        ++it;
    }
    switch (whichPlacement) {
    case 1: bruteHeightBL(inputListTransofrmed, bin, rotations, costParameter); break;
    case 2: bruteMassBL(inputListTransofrmed, bin, rotations, costParameter);
    }
    it = inputListTransofrmed.begin();
    for (auto i = 0; i < sz; ++i) {
        inputListTransofrmed.push_back(getHomotheticPolygon(*it, -epsil));
        ++it;
    }
    inputList = inputListTransofrmed;
}

double getTopY(const Polygon &p) {
    return getPointByGenInd(p, getTopRightIndex(p)).y();
}

double getMaxValueY(const std::list<corecvs::Polygon> &inputList) {
    double max = 0;
    for (auto &p : inputList) {
        if (max < getTopY(p)) {
            max = getTopY(p);
        }
    }
    return max;
}

Vector2dd getNextPoint(const list<DxfEntity*> &listOfPoints, const Vector2dd &curPoint) {
    using namespace corecvs;
    using namespace std;
    for (auto &point : listOfPoints) {
        auto entData = (DxfLineData)(dynamic_cast<DxfLineEntity*>(point)->data);
        double curX = entData.startPoint.x();
        double curY = entData.startPoint.y();
        Vector2dd candP = {curX, curY};
        if ((curPoint - candP).getLengthStable() < EPSIL) {
            return {entData.endPoint.x(), entData.endPoint.y()};
        }
    }
}


std::list<corecvs::Polygon> loadPolygonListDXF(const std::string &name) {
    using namespace corecvs;
    using namespace std;
    list<Polygon> polygonList {};
    DxfBuilder builder;
    DxfLoader loader(builder);
    loader.load(name);
    auto layerEntitties = builder.layerEntities;
    for (auto &layer : layerEntitties) {
        auto listOfEntitites = layer.second;
        auto first = listOfEntitites.begin();
        if (auto firstPtr = dynamic_cast<DxfLineEntity*>(*first)) {
            Polygon p;
            auto entData = ((DxfLineData)(firstPtr->data));
            double curX = entData.startPoint.x();
            double curY = entData.startPoint.y();
            p.push_back({curX, curY});

            curX = entData.endPoint.x();
            curY = entData.endPoint.y();
            Vector2dd curPoint = {curX, curY};
            p.push_back(curPoint);
            int restPoints = (int)listOfEntitites.size() - 2;
            for (int i = 0; i < restPoints; ++i) {
                curPoint = getNextPoint(listOfEntitites, curPoint);
                p.push_back(curPoint);
            }
            polygonList.push_back(p);
        }
    }
    return polygonList;
}

void addPolygonsFromSVGShape(SvgShape* sh, std::list<corecvs::Polygon>& polygonList) {
    using namespace corecvs;
    using namespace std;
    if (sh->type == 2) {
        polygonList.push_back(((SvgPolygon*)sh)->polygon);
    } else if (sh->type == 7) {
        for (auto s : ((SvgGroup*)sh)->shapes) {
            addPolygonsFromSVGShape(s, polygonList);
        }
    }
}

std::list<corecvs::Polygon> loadPolygonListSVG(const std::string &name) {
    using namespace corecvs;
    using namespace std;

    list<Polygon> polygonList {};
    std::ifstream proxi(name, std::ifstream::binary);
    SvgFile svgFile;
    SvgLoader loader;
    loader.loadSvg(proxi, svgFile);
    for (auto sh : svgFile.shapes) {
        addPolygonsFromSVGShape(sh, polygonList);
    }
    return polygonList;
}



