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
    //  (0,1) 1  (1,1)                  // counted pixels coords of angles (a = [0][0] ,  b = [0][1] , c =[1][1] , d = [1][0] )
    //      +---+
    //    0 |   | 2
    //      +---+
    //  (0,0) 3 (1,0)
    int edges [4];

    vector<vector<int>> squareVertex;
    VertexSquare(int a0, int a1, int b0, int b1, int c0, int c1, int d0, int d1);
    void sortVertex();

    void testVertex(int a0, int a1, int b0, int b1, int c0, int c1, int d0, int d1);
    void setEdges();
private:
    void setVertex(int a0, int a1, int b0, int b1, int c0, int c1, int d0, int d);
    void sortVertex(vector<vector<int> > *vertex);
     void swapVectors(vector<int> *a, vector<int> *b);
};

#endif // VERTEXSQUARE_H
