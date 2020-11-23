#include "structures.h"


int Point::getX() {
    return this->x;
}

Point::Point(int x, int y) {
    this->x = x;
    this->y = y;
}

int Point::getY() {
    return this->y;
}

Interval::Interval(double begin, double end) {
    this->begin = begin;
    this->end = end;
}

double Interval::getBegin() {
    return this->begin;
}

double Interval::getEnd() {
    return this->end;
}

Interval::Interval() = default;
