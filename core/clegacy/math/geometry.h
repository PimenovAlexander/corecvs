#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_
/**
 * \file geometry.h
 * \brief Header for geomtery.c
 *
 *
 * \ingroup corefiles
 * \date Apr 20, 2009
 * \author Alexander Pimenov
 */

#ifdef __cplusplus
    extern "C" {
#endif



#include <stdint.h>
namespace corecvs {

typedef struct vector2Ds16_tag {
    int16_t x;
    int16_t y;
} vector2Ds16;

typedef struct vector2Ds32_tag {
    int32_t x;
    int32_t y;
} vector2Ds32;


typedef struct vector2Du16_tag {
    uint16_t x;
    uint16_t y;
} vector2Du16;

typedef struct vector2Dd_tag {
    double x;
    double y;
} vector2Dd;

typedef struct vector3D_tag {
    uint16_t x;
    uint16_t y;
    uint16_t z;
} vector3D;

typedef struct vector3Dd_tag {
    double x;
    double y;
    double z;
} vector3Dd;

#define CLIP_HAD_INTERSECTIONS (0)
#define CLIP_NO_INTERSECTIONS (-1)

int clipCohenSutherland ( int16_t x1, int16_t y1, int16_t x2, int16_t y2, vector2Ds16 *a, vector2Ds16 *b);
vector3Dd crossProduct(vector3Dd *a, vector3Dd *b);
double dotProduct(vector3Dd *a, vector3Dd *b);
void vectorPrint(vector3Dd *v);

vector3Dd vector3DdCreate(double x, double y, double z);
vector2Dd vector2DdCreate(double x, double y);

void vectorNormalize(vector3Dd *a);
void vectorHomogeniousNormalize(vector3Dd *a);

void vector2DdIncrement(vector2Dd *a, vector2Dd *b);
double vector2DdLength(vector2Dd *a);

vector3Dd vector3DdAdd(vector3Dd a, vector3Dd b);
vector3Dd vectorMultiplyConst(vector3Dd a, double b);

/**
 * This class implements the triangle iterator that iterates over all the integer points in the triangle
 *
 **/
typedef struct TriangleIterator_TAG
{
    vector2Dd p1;
    vector2Dd p2;
    vector2Dd p3;
    int y;
    int x;
    double lpos;
    double rpos;
} TriangleIterator;

TriangleIterator triangleIteratorCreate(vector2Dd p1, vector2Dd p2, vector2Dd p3);
int triangleIteratorNext(TriangleIterator *iterator, vector2Ds32 *pos);


#ifdef __cplusplus
    } //     extern "C"

#endif
} //namespace corecvs
#endif /* _GEOMETRY_H_ */

