#include "core/utils/global.h"
#include "iostream"

#include "mainObject.h"

#ifndef VERTEXSQUARE_H
#define VERTEXSQUARE_H

//  B   C
//  +---+
//  |   |
//  +---+
//  A   D

//here i will count coords relative to the squere

class VertexSquare
{
public:
    VertexSquare();
    Vector3dd vertexCoords [2][2];
    //  (0,1)    (1,1)                  // counted pixels coords of angles (a = [0][0] ,  b = [0][1] , c =[1][1] , d = [1][0] )
    //      +---+
    //      |   |
    //      +---+
    //  (0,0)   (1,0)


    VertexSquare(int a0, int a1, int b0, int b1, int c0, int c1, int d0, int d1);
private:
    void setVertex(int a0, int a1, int b0, int b1, int c0, int c1, int d0, int d);
};

#endif // VERTEXSQUARE_H
