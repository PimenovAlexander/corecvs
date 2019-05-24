#include "vertexsquare.h"

//  B   C
//  +---+
//  |   |
//  +---+
//  A   D

VertexSquare::VertexSquare()
{
    setVertex(-239,-239,-239,-239,-239,-239,-239,-239);
}

VertexSquare::VertexSquare(int a0,int a1,int b0,int b1,int c0,int c1,int d0,int d1)
{
    setVertex(a0,a1,b0,b1,c0,c1,d0,d1);
}


//  (0,1)    (1,1)                  // counted pixels coords of angles (a = [0][0] ,  b = [0][1] , c =[1][1] , d = [1][0] )
//      +---+
//      |   |
//      +---+
//  (0,0)   (1,0)

void VertexSquare::setVertex(int a0,int a1,int b0,int b1,int c0,int c1,int d0,int d1)
{
    vertexCoords[0][0] = Vector3dd(a0,a1,0);
    vertexCoords[0][1] = Vector3dd(b0,b1,0);
    vertexCoords[1][0] = Vector3dd(d0,d1,0);
    vertexCoords[1][1] = Vector3dd(c0,c1,0);
}
