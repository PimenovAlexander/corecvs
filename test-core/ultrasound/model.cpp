#ifdef WITH_EIGEN

#include "model.h"
#include <iostream>
#include <fstream>
#include <vector>

#include <Eigen/Sparse>
#include <Eigen/SparseQR>
#include <Eigen/Dense>


using namespace std;
using namespace Eigen;

typedef Eigen::SparseMatrix<double> SpMat;
typedef Eigen::Triplet<double> T;

const double R = 10.0;
const double D = 20.0;

typedef struct {
    double x;
    double y;
}point;

vector<point> sensor_coord (){
    vector<point> sen;
    double step = D / 32.0;
    double x, y;
    point p;

    for(x = 0; x < D; x+=step){
        y = sqrt(100.0 - (x - R) * (x - R)) + R;
        p.x = x;
        p.y = y;
        sen.push_back(p);
        //cout<<x<<" "<<y<<"\n";
    }
    for(x = D; x > 0; x-=step){
        y = sqrt(100.0 - (x - R) * (x - R));
        p.x = x;
        p.y = -y + R;
        sen.push_back(p);
        //cout<<x<<" "<<y<<"\n";
    }
    return sen;
}


SpMat A(640, 400);
VectorXd Y(640);
vector<T> tripA;

Eigen::VectorXd count(VectorXd X){
    vector <point> sensor = sensor_coord();

    double l, r;
    int x, y;

    double minx,miny,maxx,maxy;

    for(int i = 0; i < 64; i++ ){
        for(int p = 0; p < 10; p++){

            int j = (i + 27 + p) % 64;

            minx = min(sensor[i].x,sensor[j].x);
            maxx = max(sensor[i].x,sensor[j].x);

            if (max(sensor[i].x,sensor[j].x) - (double)maxx != 0) maxx ++;

            miny = min(sensor[i].y,sensor[j].y);
            maxy = max(sensor[i].y,sensor[j].y);

            if ( max(sensor[i].y,sensor[j].y) - (double)maxy != 0) maxy ++;

            if (sensor[i].x == sensor[j].x){
                for(y = 0; y < 20; y++){

                    tripA.emplace_back(T(i * 10 + p, y * (int)D + int(R), 1));

                    Y(i * 10 + p) += X(y * (int)D + int(R));

                }
            } else
            if (sensor[i].y == sensor[j].y){
                for(x = 0; x < 20; x++){

                    tripA.emplace_back(T(i * 10 + p, 200 + x, 1));

                    Y(i * 10 + p) += X(200 + x);

                }
            } else
                for(x = (int)minx; x < maxx; x++){

                    l = ((double)x - sensor[i].x) * (sensor[j].y - sensor[i].y) / (sensor[j].x - sensor[i].x) + sensor[i].y ;
                    r = ((double)(x + 1) - sensor[i].x) * (sensor[j].y - sensor[i].y) / (sensor[j].x - sensor[i].x) + sensor[i].y;

                    if (l - 1 > r) swap(l,r);
                    l--;

                    for(y = (int)max(l + 1, miny); y < min(r, maxy); y++){

                        tripA.emplace_back(T(i * 10 + p, y * (int)D + x, 1));

                        Y(i * 10 + p) += X(y * (int)D + x);

                    }
                }
        }
    }

/*
    ofstream file;
    file.open("normal_ls/A.txt");
    for(int i=0;i<640;i++){
        for(int j=0;j<400;j++){
            file<<AA(i,j)<<" ";
        }
        file<<"\n";
    }

    file.close();
    std::vector<T> c;

    ifstream f;

    f.open("QR/A.txt");

    double p;

    for(int i=0;i<640;i++){
        for(int j = 0;j<400;j++){
            f>>p;
            if (p!=0)  c.push_back(T(i,j,p));
        }
    }
    f.close();

    A.setFromTriplets(c.begin(), c.end());
    Eigen::VectorXd b = A * X;
*/

    A.setFromTriplets(tripA.begin(), tripA.end());

    Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>  > solverA;
    A.makeCompressed();
    solverA.analyzePattern(A);
    solverA.factorize(A);

    Eigen::VectorXd x1 = solverA.solve(Y);

    return x1;

}

#endif
