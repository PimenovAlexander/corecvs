#ifndef VECTOR3DD_UTILS_H
#define VECTOR3DD_UTILS_H

#include "core/math/vector/vector3d.h"

#include <algorithm>

typedef Vector2d<double> vector2dd;

Vector3dd minVec3dd(Vector3dd f, Vector3dd s){
    return Vector3dd(std::min(f.x(), s.x()), std::min(f.y(), s.y()), std::min(f.z(), s.z()));
}

Vector3dd maxVec3dd(Vector3dd f, Vector3dd s){
    return Vector3dd(std::max(f.x(), s.x()), std::max(f.y(), s.y()), std::max(f.z(), s.z()));
}

Vector3dd absVec3dd(Vector3dd f){
    return Vector3dd(std::abs(f.x()), std::abs(f.y()), std::abs(f.z()));
}

double lengthVec3dd(Vector3dd f) {
    return sqrt((f).sumAllElementsSq());
}


Vector2dd minVec2dd(Vector2dd f, Vector2dd s){
    return Vector2dd(std::min(f.x(), s.x()), std::min(f.y(), s.y()));
}

Vector2dd maxVec2dd(Vector2dd f, Vector2dd s){
    return Vector2dd(std::max(f.x(), s.x()), std::max(f.y(), s.y()));
}

double dotVec2dd(Vector2dd f, Vector2dd s){
    return f.x() * s.x() + f.y()*s.y();
}

Vector2dd absVec2dd(Vector2dd f){
    return Vector2dd(std::abs(f.x()), std::abs(f.y()));
}

double lengthVec2dd(Vector2dd f) {
    return sqrt((f).sumAllElementsSq());
}

double length8Vec2dd(Vector2dd f) {
    Vector2dd pow8 = Vector2dd(pow(f.x(), 8.00), pow(f.y(), 8.00));
    return pow(pow8.x() + pow8.y(), 1.00 / 8.00);
}


#endif // VECTOR3DD_UTILS_H

