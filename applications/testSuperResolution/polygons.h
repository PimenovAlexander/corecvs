#pragma once
#include <QVector>
#include <QPolygonF>

double area(QPolygonF rec1, QPolygonF rec2);
double areaForPixels(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, int x0, int y0);
