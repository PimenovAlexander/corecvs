#include "core/geometry/twoViewOptimalTriangulation.h"

#include "core/cameracalibration/cameraModel.h"
#include "core/polynomial/polynomialSolver.h"

namespace corecvs {

TwoViewOptimalTriangulor::TwoViewOptimalTriangulor(bool L2, const Matrix44 &P1, const Vector2dd &v1, const Matrix44 &P2, const Vector2dd &v2)
    : P1(P1), P2(P2), v1(v1), v2(v2), L2(L2)
{
    F0 = CameraModel::Fundamental(P1, P2);
}

TwoViewOptimalTriangulor::TwoViewOptimalTriangulor(bool L2, const CameraModel &l, const Vector2dd &v1, const CameraModel &r, const Vector2dd &v2)
    : P1(l.getCameraMatrix()), P2(r.getCameraMatrix()), v1(v1), v2(v2), L2(L2)
{
    F0 = l.fundamentalTo(r);
}

Vector3dd TwoViewOptimalTriangulor::triangulate()
{    
    auto res = F0.rank2Nullvectors();
    e1 = res[0]; e2 = res[1];

    transform(v1, e1, T1, T1inv);
    transform(v2, e2, T2, T2inv);

    f1 = (T1 * e1)[2] / (T1 * e1)[0]; f2 = (T2 * e2)[2] / (T2 * e2)[0];

    F1 = T1inv.t() * F0 * T2inv;

    auto t = solveT();
    auto vv = solveVectors(t);
    auto v1 = T1inv * vv[0], v2 = T2inv * vv[1];

    return ftransform(v1, v2);
}

Vector3dd TwoViewOptimalTriangulor::ftransform(const Vector2dd &l, const Vector2dd &r)
{
    Matrix M(4, 4);
    for (int i = 0; i < 4; ++i)
    {
        M.a(0, i) = l[0] * P1.a(2, i) - P1.a(0, i);
        M.a(1, i) = l[1] * P1.a(2, i) - P1.a(1, i);
        M.a(2, i) = r[0] * P2.a(2, i) - P2.a(0, i);
        M.a(3, i) = r[1] * P2.a(2, i) - P2.a(1, i);
    }
    Matrix W(1, 4), V(4, 4);

    Matrix::svd(&M, &W, &V);
    int minId = 0;
    double minV = W.a(0, 0);
    for (int i = 0; i < 4; ++i)
        if (W.a(0, i) < minV)
        {
            minV = W.a(0, i);
            minId = i;
        }
    Vector3dd res(V.a(0, minId), V.a(1, minId), V.a(2, minId));
    res /= V.a(3, minId);
    return res;
}

std::array<Vector2dd, 2> TwoViewOptimalTriangulor::solveVectors(double T)
{
    auto a = F1.a(1, 1), b = F1.a(1, 2), c = F1.a(2, 1), d = F1.a(2, 2);
    Vector3dd l1(T*f1, 1, -T), l2(-f2*(c*T+d), a*T+b, c*T+d);
    Vector3dd u1(0, 0, 1.0), u2(0, 0, 1.0);
    double d1 = u1 & l1, d2 = u2 & l2;
    Vector2dd uu1 = - Vector2dd(l1[0], l1[1]) * d1 / (l1[0] * l1[0] + l1[1] * l1[1]);
    Vector2dd uu2 = - Vector2dd(l2[0], l2[1]) * d2 / (l2[0] * l2[0] + l2[1] * l2[1]);
    return std::array<Vector2dd, 2>({uu1, uu2});
}

double TwoViewOptimalTriangulor::solveT()
{
    return L2 ? solveTL2() : solveTL1();
}

double TwoViewOptimalTriangulor::solvePoly(std::vector<double> &c, std::function<double(double)> f)
{
    std::vector<double> roots;
    Polynomial pp(c);
    PolynomialSolver::solve(pp, roots);
    double bestT = roots[0], minF = std::numeric_limits<double>::max(), v;

    for (auto& t: roots)
    {
        t = pp.polishRoot(t);
        if ((v = f(t)) < minF)
        {
            minF = v;
            bestT = t;
        }
    }
    bestT = pp.polishRoot(bestT);
    //std::cout/g << "MinF: " << minF << std::endl;
    return bestT;
}

double TwoViewOptimalTriangulor::solveTL1()
{
    auto a = F1.a(1, 1), b = F1.a(1, 2), c = F1.a(2, 1), d = F1.a(2, 2);
    auto a2 = a * a, b2 = b * b, c2 = c * c, d2 = d * d, f12 = f1 * f1, f22 = f2 * f2;
    auto a3 = a2* a, b3 = b2* b, c3 = c2* c, d3 = d2* d;
    auto a4 = a2*a2, b4 = b2*b2, c4 = c2*c2, d4 = d2*d2;
    auto a5 = a3*a2, b5 = b3*b2, c5 = c3*c2, d5 = d3*d2;
    auto a6 = a3*a3, b6 = b3*b3, c6 = c3*c3, d6 = d3*d3;
    auto f14 = f12 * f12, f24 = f22 * f22;
    auto f16 = f14 * f12, f26 = f24 * f22;
    std::vector<double> cc = {
        -a4*d2*f16 + 2*a3*b*c*d*f16 - a2*b2*c2*f16
        , -2*a3*b*d2*f16 + 4*a2*b2*c*d*f16 - 2*a*b3*c2*f16
        , a6 + 3*a4*c2*f22 - 3*a4*d2*f14 + 6*a3*b*c*d*f14 - 3*a2*b2*c2*f14 - a2*b2*d2*f16 + 3*a2*c4*f24 + 2*a*b3*c*d*f16 - b4*c2*f16 + c6*f26
        , 6*a5*b + 6*a4*c*d*f22 + 12*a3*b*c2*f22 - 6*a3*b*d2*f14 + 12*a2*b2*c*d*f14 + 12*a2*c3*d*f24 - 6*a*b3*c2*f14 + 6*a*b*c4*f24 + 6*c5*d*f26
        , 15*a4*b2 - 3*a4*d2*f12 + 3*a4*d2*f22 + 6*a3*b*c*d*f12 + 24*a3*b*c*d*f22 - 3*a2*b2*c2*f12 + 18*a2*b2*c2*f22 - 3*a2*b2*d2*f14 + 18*a2*c2*d2*f24 + 6*a*b3*c*d*f14 + 24*a*b*c3*d*f24 - 3*b4*c2*f14 + 3*b2*c4*f24 + 15*c4*d2*f26
        , 20*a3*b3 - 6*a3*b*d2*f12 + 12*a3*b*d2*f22 + 12*a2*b2*c*d*f12 + 36*a2*b2*c*d*f22 + 12*a2*c*d3*f24 - 6*a*b3*c2*f12 + 12*a*b3*c2*f22 + 36*a*b*c2*d2*f24 + 12*b2*c3*d*f24 + 20*c3*d3*f26
        , -a4*d2 + 2*a3*b*c*d + 15*a2*b4 - a2*b2*c2 - 3*a2*b2*d2*f12 + 18*a2*b2*d2*f22 + 3*a2*d4*f24 + 6*a*b3*c*d*f12 + 24*a*b3*c*d*f22 + 24*a*b*c*d3*f24 - 3*b4*c2*f12 + 3*b4*c2*f22 + 18*b2*c2*d2*f24 + 15*c2*d4*f26
        , -2*a3*b*d2 + 4*a2*b2*c*d + 6*a*b5 - 2*a*b3*c2 + 12*a*b3*d2*f22 + 6*a*b*d4*f24 + 6*b4*c*d*f22 + 12*b2*c*d3*f24 + 6*c*d5*f26
        , -a2*b2*d2 + 2*a*b3*c*d + b6 - b4*c2 + 3*b4*d2*f22 + 3*b2*d4*f24 + d6*f26
    };
    for (size_t i = 0; i * 2 < cc.size(); ++i)
        std::swap(cc[i], cc[cc.size() - i - 1]);

    auto f = [&](double t)
    {
      //auto t2 = t * t;
        auto tf = t * f1, at = a * t, ct = c * t;
        auto tf2= tf*tf, atpb=at+ b, ctpd=ct+ d;
        auto ctpd2 = ctpd * ctpd;
        auto tf21=tf2+1, atpb2=atpb*atpb, f22ctpd2= f2*f2*ctpd2;
        return std::abs(t) / std::sqrt(tf21) + std::abs(ctpd) / std::sqrt(atpb2 + f22ctpd2);
    };

    return solvePoly(cc, f);
}

double TwoViewOptimalTriangulor::solveTL2()
{
    auto a = F1.a(1, 1), b = F1.a(1, 2), c = F1.a(2, 1), d = F1.a(2, 2);
    auto a2 = a * a, b2 = b * b, c2 = c * c, d2 = d * d, f12 = f1 * f1, f22 = f2 * f2;
    auto a3 = a2* a, b3 = b2* b, c3 = c2* c, d3 = d2* d;
    auto a4 = a2*a2, b4 = b2*b2, c4 = c2*c2, d4 = d2*d2;
    auto f14 = f12 * f12, f24 = f22 * f22;
    
    std::vector<double> cc = {
        -a2*c*d*f14 + a*b*c2*f14
        , a4 + 2*a2*c2*f22 - a2*d2*f14 + b2*c2*f14 + c4*f24
        , 4*a3*b - 2*a2*c*d*f12 + 4*a2*c*d*f22 + 2*a*b*c2*f12 + 4*a*b*c2*f22 - a*b*d2*f14 + b2*c*d*f14 + 4*c3*d*f24
        , 6*a2*b2 - 2*a2*d2*f12 + 2*a2*d2*f22 + 8*a*b*c*d*f22 + 2*b2*c2*f12 + 2*b2*c2*f22 + 6*c2*d2*f24, -a2*c*d + 4*a*b3 + a*b*c2 - 2*a*b*d2*f12 + 4*a*b*d2*f22 + 2*b2*c*d*f12 + 4*b2*c*d*f22 + 4*c*d3*f24
        , -a2*d2 + b4 + b2*c2 + 2*b2*d2*f22 + d4*f24, -a*b*d2 + b2*c*d
    };
    for (size_t i = 0; i * 2 < cc.size(); ++i)
        std::swap(cc[i], cc[cc.size() - i - 1]);

    auto f = [&](double t)
    {
        auto t2 = t * t;
        auto tf = t * f1, at = a * t, ct = c * t;
        auto tf2= tf*tf, atpb=at+ b, ctpd=ct+ d;
        auto ctpd2 = ctpd * ctpd;
        auto tf21=tf2+1, atpb2=atpb*atpb, f22ctpd2= f2*f2*ctpd2;
        return t2 / tf21 + ctpd2 / (atpb2 + f22ctpd2);
    };
    return solvePoly(cc, f);
}

void  TwoViewOptimalTriangulor::transform(const Vector2dd &v, const Vector3dd &e, Matrix33 &fwd, Matrix33 &bwd)
{
    Matrix33 L(1.0, 0.0, -v[0],
               0.0, 1.0, -v[1],
               0.0, 0.0,  1.0),
            Li(1.0, 0.0,  v[0],
               0.0, 1.0,  v[1],
               0.0, 0.0,  1.0);
    auto ac = e[0] - e[2] * v[0],
         as = e[1] - e[2] * v[1];
    auto norm = std::sqrt(ac * ac + as * as);
    ac /= norm;
    as /= norm;
    Matrix33 R(ac,  as, 0.0,
              -as,  ac, 0.0,
              0.0, 0.0, 1.0),
            Ri(ac, -as, 0.0,
               as,  ac, 0.0,
              0.0, 0.0, 1.0);
    fwd = R * L;
    bwd =Li * Ri;
}

}
