#include "cstdlib"
#include "iostream"
#include "vector3d.h"
using namespace std;
namespace corecvs{
Vector3dd Projection1(double koordx,double koordy, double koordz,double koofx,double koofy, double koofz,double free)

{
    /*Tests:d
    1:
    Core 3d(-2;4;4);
    Ploskost 2x-3y+z-2=0;
    projection  (0,1,5);
    2:
    Core 3d(5;2;1)
    Ploskost 2õ-y+3z+23=0
    projection (1,4,-7);
    3:
    Core 3d(2;1;-3)
    Ploskost x+2y-z-3=0
    projection (3;1;2)
    */


    double a11, a12, a13, a21, a22, a23, a31, a32, a33;
    a11 = koofy*koofz;
    a12 = -koofx*koofz;
    a13 = 0;
    a21 = 0;
    a22 = koofx*koofz;
    a23 = -koofx*koofy;
    a31 = koofx;
    a32 = koofy;
    a33 = koofz;

    double maindet = a11*(a22*a33 - a23*a32) - a12*(a21*a33 - a23*a31) + a13*(a21*a32 - a22*a31);

    double stroka1 = koordx*koofz*koofy - koofz*koordy*koofx;
    double stroka2 = koofx*koofz*koordy - koofx*koofy*koordz;
    double stroka3 = -free;

    a11 = stroka1;
    a12 = -koofx*koofz;
    a13 = 0;
    a21 = stroka2;
    a22 = koofx*koofz;
    a23 = -koofx*koofy;
    a31 = stroka3;
    a32 = koofy;
    a33 = koofz;
    double det1 = a11*(a22*a33 - a23*a32) - a12*(a21*a33 - a23*a31) + a13*(a21*a32 - a22*a31);

    a11 = koofy*koofz;
    a12 = stroka1;
    a13 = 0;
    a21 = 0;
    a22 = stroka2;
    a23 = -koofx*koofy;
    a31 = koofx;
    a32 = stroka3;
    a33 = koofz;
    double det2 = a11*(a22*a33 - a23*a32) - a12*(a21*a33 - a23*a31) + a13*(a21*a32 - a22*a31);

    a11 = koofy*koofz;
    a12 = -koofx*koofz;
    a13 = stroka1;
    a21 = 0;
    a22 = koofx*koofz;
    a23 = stroka2;
    a31 = koofx;
    a32 = koofy;
    a33 = stroka3;
    double det3 = a11*(a22*a33 - a23*a32) - a12*(a21*a33 - a23*a31) + a13*(a21*a32 - a22*a31);

    double proectionx = det1 / maindet;
    double proectiony = det2 / maindet;
    double proectionz = det3 / maindet;


    return Vector3dd(proectionx,proectiony,proectionz);
}


Vector3dd Projection2(double koordx,double koordy, double koordz,double x,double y, double z,double x2,double y2, double z2,double a,double b,double c, double d)
{

    double X, Y, Z;

    double xo = koordx, yo = koordy, zo = koordz, p = a, q = b, r = c, p1 = x2 - x, q1 = y2 - y, r1 = z2 - z, x1 = x2, y1 = y2, z1 = z2;

    X = (xo*q*p1 - x1*q1*p - yo*p*p1 + y1*p*p1) / (q*p1 - q1*p);
    Y = (yo*p*q1 - y1*p1*q - xo*q*q1 + x1*q*q1) / (p*q1 - p1*q);
    Z = (zo*q*r1 - z1*q1*r - yo*r*r1 + y1*r*r1) / (q*r1 - q1*r);



    return Vector3dd(X,Y,Z);
}
}
