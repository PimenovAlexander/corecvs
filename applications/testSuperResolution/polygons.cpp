#include "polygons.h"
#include <QPolygonF>
#include <QVector>
#include <QPointF>
#include <cmath>

double area(QPolygonF rec1, QPolygonF rec2)
{
    QPolygonF result;
    result = rec1.intersected(rec2);
    double res = 0;
    for(int i = 0; i < result.count() - 1; i++)
    {
        res += ((double)result.at(i).x() + (double)result.at(i + 1).x()) * ((double)result.at(i).y() - (double)result.at(i + 1).y());
    }
    return std::abs(res) / 2;
}

double areaForPixels(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, int x0, int y0)
{
    QPolygonF rec1;
    QPolygonF rec2;
    double x = (double) x0;
    double y = (double) y0;
    rec1<<QPointF(x1, y1)<<QPointF(x2, y2)<<QPointF(x3, y3)<<QPointF(x4, y4);
    rec2<<QPointF(x, y)<<QPointF(x + 1, y)<<QPointF(x + 1, y + 1)<<QPointF(x, y + 1);
    return area(rec1, rec2);
}
