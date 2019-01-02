#include "bspRenderer.h"

using namespace corecvs;

using namespace BSPRenderer;

void BSPNode2d::BSPNodeBuilder(Level& level)
{
    vector<Ray2d> allRays;
    Ray2d *bestSeparator = nullptr;
    int bestCost = INT_MAX;
    Level leftLevel,
          rightLevel;
    std::ofstream log;

#if DEBUG
    int h = 720;
    int w = 720;
    RGB24Buffer buffer(h, w, RGBColor::Black());
    std::string debugPrefix(std::to_string(nodeDepth)  + "-" +
                            std::to_string(nodeNumber) + "-");
    unsigned long i = 0;

    DrawLevel(level, buffer, true);
    BMPLoader().save(std::string(debugPrefix + "level.bmp"), &buffer);

    log.open("BSPNodeBuilder.log", std::ios::app);
#endif

    for (Sector& curSector : level.sectors) {
        Polygon poly = curSector.space;
        for (int i = 0; i < int(poly.size()); ++i) {
            allRays.push_back(poly.getRay(i));
        }
    }

    for (Ray2d& curRay : allRays) {
        Level curLeftLevel,
              curRightLevel;
        int cost,
            splits,
            sectorDiff,
            leftSectors,
            rightSectors;

        splits = SplitLevel(level, curRay,
                            curLeftLevel, curRightLevel,
                            log);

        leftSectors  = int(curLeftLevel.sectors.size());
        rightSectors = int(curRightLevel.sectors.size());
        sectorDiff = abs(leftSectors - rightSectors);
        cost = 3 * splits + 2 * sectorDiff;

#if DEBUG
        buffer.fillWith(RGBColor::Black());
        DrawLevel(curLeftLevel, buffer, true);
        buffer.drawLine(curRay.getStart(), curRay.getEnd(),
                        RGBColor::Yellow());
        BMPLoader().save(std::string(debugPrefix + "curLeftLevel-" +
                                     std::to_string(i) + ".bmp"),
                         &buffer);

        buffer.fillWith(RGBColor::Black());
        DrawLevel(curRightLevel, buffer, true);
        buffer.drawLine(curRay.getStart(), curRay.getEnd(),
                        RGBColor::Yellow());
        BMPLoader().save(std::string(debugPrefix + "curRightLevel-" +
                                     std::to_string(i) + ".bmp"),
                         &buffer);
        log << "------------------------" << endl
            << "Current ray number: " << i << endl
            << "Ray: " << curRay.getStart() << " -> " << curRay.getEnd() << endl
            << "splits: " << splits << endl
            << "leftSectors: " << leftSectors << endl
            << "rightSectors: " << rightSectors << endl
            << "sectorDiff: " << sectorDiff << endl
            << "cost: " << cost << endl
            << "bestCost: " << bestCost << endl << endl;

        ++i;
#endif

        if (cost < bestCost &&
            leftSectors != 0 &&
            rightSectors != 0)
        {
            bestCost = cost;
            bestSeparator = &curRay;
            leftLevel = curLeftLevel;
            rightLevel = curRightLevel;
        }
    }

    if (bestSeparator == nullptr) {
        printf("\n\nSOMETHING WENT WRONG!!!"
               "\nNO BEST SEPARATOR!!!\n\n");
        exit(EXIT_FAILURE);
    }

    leftBoundingBox = GetBoundingBox(leftLevel);
    rightBoundingBox = GetBoundingBox(rightLevel);
    separator = new Ray2d(bestSeparator->a, bestSeparator->p);
    if (leftLevel.sectors.size() > 1) {
        leftTree = new BSPNode2d(leftLevel, nodeDepth + 1, nodeNumber * 2);
    } else {
        leftSector = new Sector;
        *leftSector = leftLevel.sectors[0];
    }
    if (rightLevel.sectors.size() > 1) {
        rightTree = new BSPNode2d(rightLevel, nodeDepth + 1, nodeNumber * 2 + 1);
    } else {
        rightSector = new Sector;
        *rightSector = rightLevel.sectors[0];
    }

}

int BSPNode2d::SplitLevel(Level& level, Ray2d& separator,
                          Level& leftPart, Level& rightPart,
                          std::ofstream& log)
{
    Line2d sepLine(separator);
    int splits = 0;

    for (Sector& curSector : level.sectors) {
        Polygon poly = curSector.space;
        double t1, t2;

        separator.clip(poly, t1, t2);

#if DEBUG
        log << "+++ SplitLevel +++" << endl
            << "Poly: ";
        for (int i = 0; i < int(poly.size()); ++i)
            log << poly.getPoint(i) << " ";
        log << endl
            << "t1 = " << t1 << " t2 = " << t2 << endl
            << "p1 = " << separator.getPoint(t1)
            << " p2 = " << separator.getPoint(t2) << endl;

#endif

        if (t2 > t1 &&
            t2 <  numeric_limits<double>::infinity() &&
            t1 > -numeric_limits<double>::infinity())
        {
            Sector leftSector,
                   rightSector;
            Vector2dd p1 = separator.getPoint(t1);
            Vector2dd p2 = separator.getPoint(t2);

            if (SplitSector(curSector, sepLine,
                            p1, p2,
                            leftSector, rightSector,
                            log))
            {
                ++splits;
            }

            if (leftSector.space.size() != 0)
                leftPart.sectors.push_back(leftSector);

            if (rightSector.space.size() != 0)
                rightPart.sectors.push_back(rightSector);
        } else {
            int i = 0;

            while (sepLine.side(poly.getPoint(i)) == 0)
                ++i;

            if (sepLine.side(poly.getPoint(i)) == 1) {
                rightPart.sectors.push_back(curSector);
            } else {
                leftPart.sectors.push_back(curSector);
            }
        }
    }

    return splits;
}

bool BSPNode2d::SplitSector(Sector& sector, Line2d& sepLine,
                            Vector2dd& enterPt, Vector2dd& exitPt,
                            Sector& leftPart, Sector& rightPart,
                            std::ofstream& log)
{
    Polygon poly = sector.space;
    int enterIdx = INT_MAX,
        exitIdx = INT_MAX;
    vector<Linedef> origLinedefs = sector.linedefs;

#if DEBUG
    log << "*** SplitSector ***" << endl;
#endif
    /* Check if tangent */
    for (int i = 0; i < int(poly.size()); ++i) {
        Segment2d polySeg = poly.getSegment(i);
        Line2d polyLine(polySeg);

        if (polyLine.side(enterPt) == 0 &&
            polyLine.side(exitPt) == 0)
        {

            while (sepLine.side(poly.getNextPoint(i)) == 0)
                ++i;

            if (sepLine.side(poly.getNextPoint(i)) == 1)
                rightPart = sector;
            else
                leftPart = sector;

            return false;
        }

#if DEBUG
    log << "polySeg: " << polySeg << endl
        << "enterPt: " << enterPt
        << " On: " << OnSegment(polySeg, enterPt)
        << " On line: " << polyLine.side(enterPt) << endl
        << "exitPt: " << exitPt
        << " On: " << OnSegment(polySeg, exitPt)
        << " On line: " << polyLine.side(exitPt) << endl << endl;
#endif

        if (OnSegment(polySeg, enterPt) &&
            enterPt != polySeg.b)
        {
            enterIdx = i;
        }

        if (OnSegment(polySeg, exitPt) &&
            exitPt != polySeg.b)
        {
            exitIdx = i;
        }
    }

#if DEBUG
    log << "enterIdx = " << enterIdx
        << " exitIdx = " << exitIdx << endl;
#endif

    if (enterIdx >= int(poly.size()) ||
        exitIdx  >= int(poly.size()))
    {
        printf("\n\nSOMETHING WENT WRONG!!!"
               "\nNO ENTER AND EXIT!!!\n\n");
        exit(EXIT_FAILURE);
    }

    if (sepLine.side(poly.getNextPoint(enterIdx)) == 0) {
        printf("\n\nSOMETHING WENT WRONG!!!"
               "\nENTER POINT MESSED UP!!!\n\n");
        exit(EXIT_FAILURE);
    }

    /* 1 -- to the right, -1 -- to the left */
    if (sepLine.side(poly.getNextPoint(enterIdx)) == 1) {
            rightPart = SectorFromPoly(poly, origLinedefs,
                                       enterIdx, exitIdx,
                                       enterPt, exitPt,
                                       log);
            leftPart  = SectorFromPoly(poly, origLinedefs,
                                       exitIdx, enterIdx,
                                       exitPt, enterPt,
                                       log);
    } else {
            leftPart  = SectorFromPoly(poly, origLinedefs,
                                       enterIdx, exitIdx,
                                       enterPt, exitPt,
                                       log);
            rightPart = SectorFromPoly(poly, origLinedefs,
                                       exitIdx, enterIdx,
                                       exitPt, enterPt,
                                       log);
    }

    return true;
}

Sector BSPNode2d::SectorFromPoly(Polygon& poly, vector<Linedef>& origLinedefs,
                             int& startIdx, int& endIdx,
                             Vector2dd& startPt, Vector2dd& endPt,
                             std::ofstream& log)
{
    Sector outSector;
    Polygon outPoly;
    int i;
    Vector2dd curPt;

#if DEBUG
    log << "=== SectorFromPoly ===" << endl
        << "Poly: ";
    for (int i = 0; i < int(poly.size()); ++i)
         log << poly.getPoint(i) << " ";
    log << endl << "origLinedefs: ";
    for (unsigned long i = 0; i < origLinedefs.size(); ++i)
        log << origLinedefs[i].sidedef << " ";
    log << endl << "startIdx = " << startIdx
                << " endIdx = " << endIdx << endl
                << " startPt = " << startPt << endl
                << " endPt = " << endPt << endl << endl;
#endif

    outPoly.push_back(startPt);
    i = startIdx;
    curPt = poly.getNextPoint(i);
    while (i != endIdx && curPt != endPt) {
#if DEBUG
        log << " curPt = " << curPt << endl
            << " i = " << i << endl << endl;
#endif
        outPoly.push_back(curPt);
        i = poly.getNextIndex(i);
        curPt = poly.getNextPoint(i);
    }
    outPoly.push_back(endPt);
    outSector.space.resize(outPoly.size());
    outSector.space = outPoly;

    outSector.linedefs.resize(outPoly.size());
    for (unsigned long i = 0; i < outPoly.size() - 1; ++i) {
#if DEBUG
        log << " i = " << i << endl
            << " j = " << (i + unsigned(startIdx)) % poly.size() << endl;
#endif
        outSector.linedefs[i] = origLinedefs[(i + unsigned(startIdx)) % poly.size()];
    }
    outSector.linedefs[outPoly.size() - 1] = Linedef {0};

    return outSector;
}

Polygon BSPNode2d::GetBoundingBox(Level& level)
{
    Polygon boundingBox;
    double xMin = DBL_MAX, xMax = 0.0,
           yMin = DBL_MAX, yMax = 0.0;

    for (Sector& curSector : level.sectors) {
        for (Vector2dd& curPoint : curSector.space) {
            if (curPoint.x() < xMin) xMin = curPoint.x();
            if (curPoint.x() > xMax) xMax = curPoint.x();
            if (curPoint.y() < yMin) yMin = curPoint.y();
            if (curPoint.y() > yMax) yMax = curPoint.y();
        }
    }

    boundingBox.push_back(Vector2dd(xMin, yMin));
    boundingBox.push_back(Vector2dd(xMin, yMax));
    boundingBox.push_back(Vector2dd(xMax, yMax));
    boundingBox.push_back(Vector2dd(xMax, yMin));

    return boundingBox;
}

bool BSPRenderer::OnSegment(Segment2d& segment, const Vector2dd& point)
{
    Vector2dd v1(segment.a.x() - point.x(),
                 segment.a.y() - point.y()),
              v2(segment.b.x() - point.x(),
                 segment.b.y() - point.y());
    Line2d line(segment);

//    cout << "*** OnSegment ***" << endl
//         << "segment: " << segment << " , point: " << point << endl
//         << "v1: " << v1 << " , v2: " << v2 << endl
//         << "(normal() & point): " << (line.normal() & point)
//         << " , line.last():" << line.last()
//         << " , scalar: " << ((v1.x() * v2.x() + v1.y() * v2.y()) <= 0) << endl << endl;

    return ((line.side(point) == 0) &&
            ((v1.x() * v2.x() + v1.y() * v2.y()) <= 0));
}

void BSPRenderer::DrawSector(Sector& sector, RGB24Buffer& buffer,
                             bool withInfo)
{
    AbstractPainter<RGB24Buffer> painter(&buffer);
    Polygon p = sector.space;

    if (withInfo) {
        for (unsigned long i = 0; i < p.size(); i++) {
            Ray2d ray = p.getRay(int(i));
            Vector2dd center = ray.getPoint(0.5);
            Vector2dd decal = p.getNormal(int(i)).normalised();
            Vector2dd number = center + decal * 15;

            switch (sector.linedefs[i].sidedef) {
                case 0 :
                    buffer.drawLine(ray.getStart(), ray.getEnd(),
                                    RGBColor::Blue());
                    break;
                case 1 :
                    buffer.drawLine(ray.getStart(), ray.getEnd(),
                                    RGBColor::White());
                    break;
                case 2 :
                    buffer.drawLine(ray.getStart(), ray.getEnd(),
                                    RGBColor::Red());
                    break;
            }

            decal = center + decal * 5;
            buffer.drawLine(center, decal, RGBColor::White());

            painter.drawFormat(uint16_t(round(number.x())),
                               uint16_t(round(number.y())),
                               RGBColor::Purple(), 2, "%i", i);
        }
    } else {
        painter.drawPolygon(p, RGBColor::White());
    }
}

void BSPRenderer::DrawLevel(Level& level, RGB24Buffer& buffer,
                            bool withInfo)
{
    for (BSPRenderer::Sector& s : level.sectors) {
        DrawSector(s, buffer, withInfo);
    }
}

void BSPRenderer::DrawSector(BSPNode2d& node, RGB24Buffer& buffer,
                             bool withInfo)
{
    if (node.leftSector == nullptr)
        DrawSector(*(node.leftTree), buffer, withInfo);
    else
        DrawSector(*(node.leftSector), buffer, withInfo);

    if (node.rightSector == nullptr)
        DrawSector(*(node.rightTree), buffer, withInfo);
    else
        DrawSector(*(node.rightSector), buffer, withInfo);
}

void BSPRenderer::DrawBSPNode(BSPNode2d& node, RGB24Buffer& buffer,
                              bool withInfo)
{
    AbstractPainter<RGB24Buffer> painter(&buffer);
    std::string file(std::to_string(node.nodeDepth)  + "-" +
                     std::to_string(node.nodeNumber) + "-" +
                     "BSPNode.bmp");

    buffer.fillWith(RGBColor::Black());
    DrawSector(node, buffer, withInfo);
    painter.drawPolygon(node.leftBoundingBox, RGBColor::Green());
    painter.drawPolygon(node.rightBoundingBox, RGBColor::Red());
    buffer.drawLine(node.separator->getStart(),
                     node.separator->getEnd(),
                     RGBColor::Yellow());
    BMPLoader().save(file, &buffer);

    if (node.leftTree != nullptr)
        DrawBSPNode(*(node.leftTree), buffer, withInfo);
    if (node.rightTree != nullptr)
        DrawBSPNode(*(node.rightTree), buffer, withInfo);
}
