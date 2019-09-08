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
void VertexSquare::testVertex(int a0,int a1,int b0,int b1,int c0,int c1,int d0,int d1)
{
    vector<vector<int>> vertex = {{a0,a1},{b0,b1},{c0,c1},{d0,d1}};

    std::cout<<vertex.at(0).at(0)<<"---"<<vertex.at(0).at(1)<<"_____"<<vertex.at(1).at(0)<<"---"<<vertex.at(1).at(1)<<"_____"<<vertex.at(2).at(0)<<"---"<<vertex.at(2).at(1)<<"_____"<<vertex.at(3).at(0)<<"---"<<vertex.at(3).at(1)<<std::endl;
    sortVertex(&vertex);
    std::cout<<"after"<<std::endl;
    std::cout<<vertex.at(0).at(0)<<"---"<<vertex.at(0).at(1)<<"_____"<<vertex.at(1).at(0)<<"---"<<vertex.at(1).at(1)<<"_____"<<vertex.at(2).at(0)<<"---"<<vertex.at(2).at(1)<<"_____"<<vertex.at(3).at(0)<<"---"<<vertex.at(3).at(1)<<std::endl;

}

void VertexSquare::setVertex(int a0,int a1,int b0,int b1,int c0,int c1,int d0,int d1)
{
    vector<vector<int>> vertex = {{a0,a1},{b0,b1},{c0,c1},{d0,d1}};

    std::cout<<vertex.at(0).at(0)<<"---"<<vertex.at(0).at(1)<<"_____"<<vertex.at(1).at(0)<<"---"<<vertex.at(1).at(1)<<"_____"<<vertex.at(2).at(0)<<"---"<<vertex.at(2).at(1)<<"_____"<<vertex.at(3).at(0)<<"---"<<vertex.at(3).at(1)<<std::endl;
    sortVertex(&vertex);
    std::cout<<"after"<<std::endl;
    std::cout<<vertex.at(0).at(0)<<"---"<<vertex.at(0).at(1)<<"_____"<<vertex.at(1).at(0)<<"---"<<vertex.at(1).at(1)<<"_____"<<vertex.at(2).at(0)<<"---"<<vertex.at(2).at(1)<<"_____"<<vertex.at(3).at(0)<<"---"<<vertex.at(3).at(1)<<std::endl;
    vertexCoords[0][0] = Vector3dd(a0,a1,0);
    vertexCoords[0][1] = Vector3dd(b0,b1,0);
    vertexCoords[1][0] = Vector3dd(d0,d1,0);
    vertexCoords[1][1] = Vector3dd(c0,c1,0);
}

void VertexSquare::sortVertex(vector<vector<int>> *vertex)
{
    //add array [4][2]
    for (int i = 1;i < 4;i++)
    {
        vector<int> key = vertex->at(i);
        int  j = i-1;
        while (j > 0 && vertex->at(j).at(1) > key.at(1))
        {
            vertex->at(j+1) = vertex->at(j);
            j--;
        }
        vertex->at(j+1) = key;
    }


    /*for j = 2 to A.length do
        key = A[j]
        i = j-1
        while (i > 0 and A[i] > key) do
            A[i + 1] = A[i]
            i = i - 1
        end while
        A[i+1] = key
    end for
*/
}

void VertexSquare::swapVectors(vector<int> *a,vector <int> *b)
{
    vector<int> c = *a;
    *a = *b;
    *b = c;
}
