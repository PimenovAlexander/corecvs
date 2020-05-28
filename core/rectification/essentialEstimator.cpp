/**
 * \file essentialEstimator.cpp
 * \brief Add Comment Here
 *
 * \date Oct 19, 2011
 * \author alexander
 */


#include "rectification/essentialEstimator.h"
#include "math/quaternion.h"
#include "math/levenmarq.h"
#include "math/gradientDescent.h"
#include "kalman/classicKalman.h"
#include "polynomial/polynomialSolver.h"
#include "math/matrix/matrixOperations.h"
#include "meta/packedDerivative.h"
#include "meta/astNode.h"

//#include "kalman.h"

namespace corecvs {

using std::flush;

#define DEEP_TRACE

EssentialEstimator::EssentialEstimator()
{
}


/**
 *  Both essential and fundamental matrices can be searched with 8 point algorithm
 *
 * \code
 *             [ a_11   a_12  a_13 ]  [ x']    [ 0 ]
 * [x, y ,1]   [ a_21   a_22  a_23 ]  [ y'] =  [ 0 ]
 *             [ a_31   a_32  a_33 ]  [ 1 ]    [ 0 ]
 * \endcode
 *
 * \f[
 *      \pmatrix{ x & y & 1 }
 *      \pmatrix{
 *        F_{1,1} & F_{1,2} & F_{1,3}\cr
 *        F_{2,1} & F_{2,2} & F_{2,3}\cr
 *        F_{3,1} & F_{3,2} & F_{3,3}}
 *      \pmatrix{ x' \cr y' \cr 1} = \pmatrix{ 0 \cr 0 \cr 0}
 * \f]
 *
 *
 *  We reconstruct the model by LSE estimation.
 *
 * \f[
 *      F_{1,1} x x' + F_{1,2} x y' + F_{1,3} x +
 *      F_{2,1} y x' + F_{2,2} y y' + F_{2,3} y +
 *      F_{3,1} x' + F_{3,2} y' + F_{3,3} = 0
 * \f]
 *
 *
 * \f[
 *     \left( \begin{array}{ccccccccc}
 *       x_1 x_1' & x_1 y_1' & x_1 & y_1 x_1' & y_1 y_1' & y_1 & x_1' & y_1' & 1 \\
 *       \multicolumn{9}{c}{\cdots} \\
 *       x_n x_n' & x_n y_n' & x_n & y_n x_n' & y_n y_n' & y_n & x_n' & y_n' & 1
 *    \end{array} \right )
 *
 *    \pmatrix{
 *      F_{1,1} \cr F_{1,2} \cr F_{1,3} \cr
 *      F_{2,1} \cr F_{2,2} \cr F_{2,3} \cr
 *      F_{3,1} \cr F_{3,2} \cr F_{3,3}
 *    }
 *    = \vec 0
 * \f]
 **/
EssentialMatrix EssentialEstimator::getEssentialLSE(const vector<Correspondence*> & samples)
{
    Matrix X(std::max(9, (int)samples.size()), 9, 0.0);
    Matrix W(1,9);
    Matrix V(9,9);
    Matrix33 F(0.0);

#if 1
    corecvs::Vector2dd meanL(0.0, 0.0), meanR(0.0, 0.0);
    corecvs::Vector2dd stdevL(0.0, 0.0), stdevR(0.0, 0.0);
    for (size_t i = 0; i < samples.size(); ++i)
    {
        auto L = samples[i]->start;
        auto R = samples[i]->end;
        meanL += L;
        meanR += R;

        stdevL += L * L;
        stdevR += R * R;
    }
    double N = 1.0 / samples.size();
    meanL *= N;
    meanR *= N;
    stdevL = stdevL * N - meanL * meanL;
    stdevR = stdevR * N - meanR * meanR;
    for (int i = 0; i < 2; ++i)
    {
        stdevL[i] = std::sqrt(stdevL[i]);       // TODO: this must be present in statistics/approx blocks...
        stdevR[i] = std::sqrt(stdevR[i]);
    }
#if 1
    corecvs::Matrix33 TL = corecvs::Matrix33(
            stdevL[0],       0.0, meanL[0],
                  0.0, stdevL[1], meanL[1],
                  0.0,       0.0,      1.0).inv();
    corecvs::Matrix33 TR = corecvs::Matrix33(
            stdevR[0],       0.0, meanR[0],
                  0.0, stdevR[1], meanR[1],
                  0.0,       0.0,      1.0).inv();
#else
    corecvs::Matrix33 TL(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
    corecvs::Matrix33 TR(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
#endif
#endif

    corecvs::Vector2dd ml(0.0, 0.0), mr(0.0, 0.0);
    for (unsigned i = 0; i < samples.size(); i++)
    {
        Vector2dd first  = samples[i]->start;
        Vector2dd second = samples[i]->end;
#if 1
        auto firstP = TL * corecvs::Vector3dd(first.x(), first.y(), 1.0);
        auto secondP = TR * corecvs::Vector3dd(second.x(), second.y(), 1.0);
        firstP /= firstP[2];
        secondP /= secondP[2];
        first = corecvs::Vector2dd(firstP[0], firstP[1]);
        second = corecvs::Vector2dd(secondP[0], secondP[1]);
        ml += first;
        mr += second;
#endif
        X.fillLineWithArgs(i,
        first.x() * second.x(), first.x() * second.y(), first.x(),
        first.y() * second.x(), first.y() * second.y(), first.y(),
        second.x(), second.y(),  1.0);
    }
//    X.a(samples.size(), 0) = 1.0;
//    std::cout << ml << std::endl << mr << std::endl;

#ifdef DEEP_TRACE
//        X.print();
//        cout << endl;
//        fflush(stdout);
#endif

    /** \todo TODO Here the transform should be done to increase the precision.
     * The choice should be centered to 0 and scaled to average of sqrt(2) */

    Matrix::svd(&X, &W, &V);

#ifdef DEEP_TRACE
//    printf("The singular values:\n");
//    W.print();
//    printf("\n");
//    X.print();
//    printf("\n");
//    V.print();
//    printf("\n");
#endif


    int min = -1;
    double minval = std::numeric_limits<double>::max();
    for (unsigned i = 0; i < 9; i ++)
    {
        if (W.a(0, i) <= minval)
        {
            min = i;
            minval = W.a(0, i);
        }
    }

    F = Matrix33(
        V.a(0,min), V.a(1,min), V.a(2,min),
        V.a(3,min), V.a(4,min), V.a(5,min),
        V.a(6,min), V.a(7,min), V.a(8,min)
    );

    F = TL.transposed() * F * TR;
    if (F.a(2,1) < 0) {
        F = -F;
    }

    return EssentialMatrix(F);
}

// Note that in case of 7 point-point correspondences we can get up to 3 models
std::vector<EssentialMatrix> EssentialEstimator::getEssential7point(const vector<Correspondence*> &samples)
{
    /*
     * Here we go:
     *  + creating standard LSE matrix
     *  + get 2d null-space
     *  + solve polynomial equation
     */
    Matrix X(std::max(9, (int)samples.size()), 9, 0.0);
    Matrix W(1,9);
    Matrix V(9,9);

    corecvs::Vector2dd meanL(0.0, 0.0), meanR(0.0, 0.0);
    corecvs::Vector2dd stdevL(0.0, 0.0), stdevR(0.0, 0.0);
    for (size_t i = 0; i < samples.size(); ++i)
    {
        auto L = samples[i]->start;
        auto R = samples[i]->end;
        meanL += L;
        meanR += R;

        stdevL += L * L;
        stdevR += R * R;
    }
    double N = 1.0 / samples.size();
    meanL *= N;
    meanR *= N;
    stdevL = stdevL * N - meanL * meanL;
    stdevR = stdevR * N - meanR * meanR;
    for (int i = 0; i < 2; ++i)
    {
        stdevL[i] = std::sqrt(stdevL[i]);
        stdevR[i] = std::sqrt(stdevR[i]);
    }
    corecvs::Matrix33 TL = corecvs::Matrix33(
            stdevL[0],       0.0, meanL[0],
                  0.0, stdevL[1], meanL[1],
                  0.0,       0.0,      1.0).inv();
    corecvs::Matrix33 TR = corecvs::Matrix33(
            stdevR[0],       0.0, meanR[0],
                  0.0, stdevR[1], meanR[1],
                  0.0,       0.0,      1.0).inv();

    corecvs::Vector2dd ml(0.0, 0.0), mr(0.0, 0.0);
    for (unsigned i = 0; i < samples.size(); i++)
    {
        Vector2dd first  = samples[i]->start;
        Vector2dd second = samples[i]->end;
        auto firstP = TL * corecvs::Vector3dd(first.x(), first.y(), 1.0);
        auto secondP = TR * corecvs::Vector3dd(second.x(), second.y(), 1.0);
        firstP /= firstP[2];
        secondP /= secondP[2];
        first = corecvs::Vector2dd(firstP[0], firstP[1]);
        second = corecvs::Vector2dd(secondP[0], secondP[1]);
        ml += first;
        mr += second;

        X.fillLineWithArgs(i,
        first.x() * second.x(), first.x() * second.y(), first.x(),
        first.y() * second.x(), first.y() * second.y(), first.y(),
        second.x(), second.y(),  1.0);
    }

    Matrix::svd(&X, &W, &V);

    int minIdx[] = {0, 1};
    double minval[] = { W.a(0, 0), W.a(0, 1) };
    for (unsigned i = 2; i < 9; i ++)
    {
        if (W.a(0, i) < minval[1])
        {
            minIdx[1] = i;
            minval[1] = W.a(0, i);
        }
        if (minval[0] > minval[1])
        {
            std::swap(minIdx[0], minIdx[1]);
            std::swap(minval[0], minval[1]);
        }
    }

    corecvs::Matrix33 F1(
            V.a(0, minIdx[0]), V.a(1, minIdx[0]), V.a(2, minIdx[0]),
            V.a(3, minIdx[0]), V.a(4, minIdx[0]), V.a(5, minIdx[0]),
            V.a(6, minIdx[0]), V.a(7, minIdx[0]), V.a(8, minIdx[0]));
    corecvs::Matrix33 F2(
            V.a(0, minIdx[1]), V.a(1, minIdx[1]), V.a(2, minIdx[1]),
            V.a(3, minIdx[1]), V.a(4, minIdx[1]), V.a(5, minIdx[1]),
            V.a(6, minIdx[1]), V.a(7, minIdx[1]), V.a(8, minIdx[1]));
    double f111 = F1(0, 0),
           f112 = F1(0, 1),
           f113 = F1(0, 2),
           f121 = F1(1, 0),
           f122 = F1(1, 1),
           f123 = F1(1, 2),
           f131 = F1(2, 0),
           f132 = F1(2, 1),
           f133 = F1(2, 2);
    double f211 = F2(0, 0),
           f212 = F2(0, 1),
           f213 = F2(0, 2),
           f221 = F2(1, 0),
           f222 = F2(1, 1),
           f223 = F2(1, 2),
           f231 = F2(2, 0),
           f232 = F2(2, 1),
           f233 = F2(2, 2);
    /*
     * Auto-generated stub. Do not touch or it will explode.
     * Here we just enforce 2-rank constraint in form of
     * det(alpha * F1 + (1-alpha) * F2) = 0
     */
    double alpha_0 = f211*f222*f233 - f211*f223*f232 - f212*f221*f233 + f212*f223*f231 + f213*f221*f232 - f213*f222*f231;
    double alpha_3 = f111*f122*f133 - f111*f123*f132 - f112*f121*f133 + f112*f123*f131 + f113*f121*f132 - f113*f122*f131 - f111*f122*f233 + f111*f123*f232 + f111*f132*f223 - f111*f133*f222 + f112*f121*f233 - f112*f123*f231 - f112*f131*f223 + f112*f133*f221 - f113*f121*f232 + f113*f122*f231 + f113*f131*f222 - f113*f132*f221 - f121*f132*f213 + f121*f133*f212 + f122*f131*f213 - f122*f133*f211 - f123*f131*f212 + f123*f132*f211 + f111*f222*f233 - f111*f223*f232 - f112*f221*f233 + f112*f223*f231 + f113*f221*f232 - f113*f222*f231 - f121*f212*f233 + f121*f213*f232 + f122*f211*f233 - f122*f213*f231 - f123*f211*f232 + f123*f212*f231 + f131*f212*f223 - f131*f213*f222 - f132*f211*f223 + f132*f213*f221 + f133*f211*f222 - f133*f212*f221 - f211*f222*f233 + f211*f223*f232 + f212*f221*f233 - f212*f223*f231 - f213*f221*f232 + f213*f222*f231;
    double alpha_2 = f111*f122*f233 - f111*f123*f232 - f111*f132*f223 + f111*f133*f222 - f112*f121*f233 + f112*f123*f231 + f112*f131*f223 - f112*f133*f221 + f113*f121*f232 - f113*f122*f231 - f113*f131*f222 + f113*f132*f221 + f121*f132*f213 - f121*f133*f212 - f122*f131*f213 + f122*f133*f211 + f123*f131*f212 - f123*f132*f211 - 2*f111*f222*f233 + 2*f111*f223*f232 + 2*f112*f221*f233 - 2*f112*f223*f231 - 2*f113*f221*f232 + 2*f113*f222*f231 + 2*f121*f212*f233 - 2*f121*f213*f232 - 2*f122*f211*f233 + 2*f122*f213*f231 + 2*f123*f211*f232 - 2*f123*f212*f231 - 2*f131*f212*f223 + 2*f131*f213*f222 + 2*f132*f211*f223 - 2*f132*f213*f221 - 2*f133*f211*f222 + 2*f133*f212*f221 + 3*f211*f222*f233 - 3*f211*f223*f232 - 3*f212*f221*f233 + 3*f212*f223*f231 + 3*f213*f221*f232 - 3*f213*f222*f231;
    double alpha_1 = f111*f222*f233 - f111*f223*f232 - f112*f221*f233 + f112*f223*f231 + f113*f221*f232 - f113*f222*f231 - f121*f212*f233 + f121*f213*f232 + f122*f211*f233 - f122*f213*f231 - f123*f211*f232 + f123*f212*f231 + f131*f212*f223 - f131*f213*f222 - f132*f211*f223 + f132*f213*f221 + f133*f211*f222 - f133*f212*f221 - 3*f211*f222*f233 + 3*f211*f223*f232 + 3*f212*f221*f233 - 3*f212*f223*f231 - 3*f213*f221*f232 + 3*f213*f222*f231;
    double coeff[] = { alpha_0, alpha_1, alpha_2, alpha_3 };
    double roots[3];
    int realRoots = (int)PolynomialSolver::solve(coeff, roots, 3);

    std::vector<EssentialMatrix> hypothesis;
    for (int i = 0; i < realRoots; ++i)
    {
        hypothesis.emplace_back(TL.transposed() * (F1 * roots[i] + (1.0 - roots[i]) * F2) * TR);
    }

    return hypothesis;
}

std::vector<EssentialMatrix> EssentialEstimator::getEssential5point(const vector<Correspondence*> &samples)
{
    Matrix XLSE(std::max(9, (int)samples.size()), 9, 0.0);
    Matrix WLSE(1,9);
    Matrix V(9,9);
    
    for (unsigned i = 0; i < samples.size(); i++)
    {
        Vector2dd first  = samples[i]->start;
        Vector2dd second = samples[i]->end;

        XLSE.fillLineWithArgs(i,
        first.x() * second.x(), first.x() * second.y(), first.x(),
        first.y() * second.x(), first.y() * second.y(), first.y(),
        second.x(), second.y(),  1.0);
    }

    Matrix::svd(&XLSE, &WLSE, &V);

    std::vector<int> ids;
    for (int i = 0; i < 9; ++i)
        ids.push_back(i);
    std::sort(ids.begin(), ids.end(), [&](const int &a, const int &b) { return WLSE.a(0, a) < WLSE.a(0, b); });

    corecvs::Vector X(9), Y(9), Z(9), W(9);
    for (int i = 0; i < 9; ++i)
    {
        X[i] = V.a(i, ids[0]);
        Y[i] = V.a(i, ids[1]);
        Z[i] = V.a(i, ids[2]);
        W[i] = V.a(i, ids[3]);
    }

    /*
     * And here goes black magick. We know that X,Y,Z,W form nullspace of XLSE.
     * So Essential matrix (up to some scale factor) has form x*X+y*Y+z*Z+W and we would like to get x, y, z
     * Now we enforce constraints: det(E)=0 and 2EE'E-trace(EE')E=0 and use
     * monomial bases {x^3 y^3 x^2y x^2z x^2 y^2z y^2 xyz xy | x y 1} where for first 10 bases we get
     * 10 numerical elements and for last 3 we get 10 polynomial (w.r.t z) elements from each equaion
     *
     * Then we eliminate left 10x10 (numerical part) to diagonal matrix and apply this transformation
     * to the right 10x3 part of matrix.
     *
     * Then we enforce xy*z=xyz y^2*z=y^2z and x^2*z=x^2z and get 3x3 polynomial (w.r.t z) matrix which has
     * eigen-value 0 and thus it's determinant (10th degree polynomial) gives us z (and from z we get x and y)
     *
     * This gives us up to ten hypotheses
     */
    //Let's construct numerical part of matrix (2EE'E... goes first 9 eqs, then det(E) = 0).
    const double x1 = X[0], x2 = X[1], x3 = X[2], x4 = X[3], x5 = X[4], x6 = X[5], x7 = X[6], x8 = X[7], x9 = X[8];
    const double y1 = Y[0], y2 = Y[1], y3 = Y[2], y4 = Y[3], y5 = Y[4], y6 = Y[5], y7 = Y[6], y8 = Y[7], y9 = Y[8];
    const double z1 = Z[0], z2 = Z[1], z3 = Z[2], z4 = Z[3], z5 = Z[4], z6 = Z[5], z7 = Z[6], z8 = Z[7], z9 = Z[8];
    const double w1 = W[0], w2 = W[1], w3 = W[2], w4 = W[3], w5 = W[4], w6 = W[5], w7 = W[6], w8 = W[7], w9 = W[8];
    // Please, do not touch unused variables for the sake of readability
    const double x12 = x1 * x1, x22 = x2 * x2, x32 = x3 * x3, x42 = x4 * x4, x52 = x5 * x5, x62 = x6 * x6, x72 = x7 * x7, x82 = x8 * x8, x92 = x9 * x9;
    const double y12 = y1 * y1, y22 = y2 * y2, y32 = y3 * y3, y42 = y4 * y4, y52 = y5 * y5, y62 = y6 * y6, y72 = y7 * y7, y82 = y8 * y8, y92 = y9 * y9;
    const double z12 = z1 * z1, z22 = z2 * z2, z32 = z3 * z3, z42 = z4 * z4, z52 = z5 * z5, z62 = z6 * z6, z72 = z7 * z7, z82 = z8 * z8, z92 = z9 * z9;
    const double w12 = w1 * w1, w22 = w2 * w2, w32 = w3 * w3, w42 = w4 * w4, w52 = w5 * w5, w62 = w6 * w6, w72 = w7 * w7, w82 = w8 * w8, w92 = w9 * w9;

    corecvs::Matrix A(10, 10);
#include "rectification/p5pNumericPart.h"
// Now we fill polynomial part of matrix
    corecvs::PolynomialMatrix B(10, 3);
#include "rectification/p5pPolynomialPart.h"

    A = A.inv();

    /*
     * Actually, we need only x^2z, x^2, y^2z, y^2, xyz, xy rows of A^{-1}*B
     */
    A.data = &A.element(4, 0);
    A.h = 6;

    auto BB = A * B;
    corecvs::PolynomialMatrix Bf(3, 3);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            Bf.a(i, j) = BB.a(2 * i, j)  - corecvs::Polynomial::X() * BB.a(2 * i + 1, j);
        }
    }

    // Now Bf is a final polynomial-matrix and we can start solving z, x, y
    auto P1 = Bf.a(0, 1) * Bf.a(1, 2) - Bf.a(0, 2) * Bf.a(1, 1);
    auto P2 = Bf.a(0, 2) * Bf.a(1, 0) - Bf.a(0, 0) * Bf.a(1, 2);
    auto P3 = Bf.a(0, 0) * Bf.a(1, 1) - Bf.a(0, 1) * Bf.a(1, 0);
    auto P = P1 * Bf.a(2, 0) + P2 * Bf.a(2, 1) + P3 * Bf.a(2, 2);
    std::vector<double> roots;
    corecvs::PolynomialSolver::solve(P, roots);

    std::vector<corecvs::EssentialMatrix> hypothesis;
    for (auto& z: roots)
    {
        auto A = Bf(z);
        double a = A.a(0, 0), b = A.a(0, 1), c = A.a(0, 2), 
               d = A.a(1, 0), e = A.a(1, 1), f = A.a(1, 2),
               g = A.a(2, 0), h = A.a(2, 1), i = A.a(2, 2),
               x, y;
        double d1 = b * g - a * h,
               d2 = e * g - d * h,
               d3 = b * d - a * e;
        double md = std::abs(d1), ab;
        int idd = 1;
        if ((ab = std::abs(d2)) > md) { md = ab; idd = 2; }
        if ((ab = std::abs(d3)) > md) { md = ab; idd = 3; }
//        CORE_ASSERT_TRUE_S(md > 1e-9);
        switch (idd)
        {
            case 1:
                x = (c * h - i * b) / d1;
                y = (a * i - c * g) / d1;
                break;
            case 2:
                x = (f * h - i * e) / d2;
                y = (d * i - f * g) / d2;
                break;
            case 3:
                x = (c * e - f * b) / d3;
                y = (f * a - c * d) / d3;
                break;
            default:
                CORE_ASSERT_TRUE_S(false);
        }
        corecvs::Vector v(3);
        v[0] = x;
        v[1] = y;
        v[2] = 1.0;
        auto Av = A*v;
        auto EE = x * X + y * Y + z * Z + W;
        corecvs::Matrix33 E(EE[0], EE[1], EE[2],
                            EE[3], EE[4], EE[5],
                            EE[6], EE[7], EE[8]);
        bool isOk = true;
        for (int iii = 0; iii < 9 && isOk; ++iii)
            if (std::isnan(EE[iii]) || std::isinf(EE[iii]))
            {
                isOk = false;
            }
        if (isOk)
            hypothesis.emplace_back(E);
    }
    return hypothesis;
}


EssentialMatrix EssentialEstimator::getEssentialLM(const vector<Correspondence*> & samples, const Quaternion &rotation, const Vector3dd &translation)
{
    //CostFunction7toN costFunction(&samples);            //      41009
    //CostFunction7toNPacked costFunction(&samples);      //      296492s
    // CostFunction7toNGenerated1 costFunction(&samples); //      327208s
    CostFunction7toNGenerated1 costFunction(&samples);    // cse  332843s
                                                          // man1 321038
                                                          // man2 271804
                                                          // man3 268170



    NormalizeFunction normalise;
    LevenbergMarquardt LMfit;

    LMfit.f = &costFunction;
    LMfit.normalisation = &normalise;
    LMfit.maxIterations = 100000;
    LMfit.startLambda = 10;
    LMfit.lambdaFactor = 20.0;

    LMfit.trace = false;
    LMfit.traceProgress = false;

    vector<double> input(CostFunctionBase::VECTOR_SIZE);
    /* Left camera is in the negative direction of right camera */
 // Quaternion rotation    = Quaternion::RotationIdentity();
 //// Vector3dd  translation = Vector3dd(-1.0, 0.0, 0.0);
    CostFunctionBase::packState(&input[0], rotation, translation);

    vector<double> output(samples.size());
    for (unsigned i = 0; i < samples.size(); i++)
        output[i] = 0.0;

    vector<double> optInput = LMfit.fit(input, output);

    PrinterVisitor visitor;

   /* cout << "Axis" << endl;
    CostFunctionBase::getRotationAxis(&optInput[0]).accept(visitor);
    cout << "Angle:" << endl;
    cout << radToDeg(CostFunctionBase::getRotationAngle(&optInput[0]))  << "deg" << endl;
    cout << "Baseline:" << endl;
    CostFunctionBase::getTranslation(&optInput[0]).accept(visitor);
*/

    return CostFunctionBase::getEssential(&optInput[0]);
}

EssentialMatrix EssentialEstimator::getEssentialGrad(const vector<Correspondence*> & samples)
{
    CostFunction7to1 costFunction(&samples);
    NormalizeFunction normalise;
    GradientDescent GDfit;

    GDfit.f = &costFunction;
    GDfit.normalisation = &normalise;
    GDfit.maxIterations = 10000000;
    GDfit.lambda = 0.003;

    vector<double> input(CostFunctionBase::VECTOR_SIZE);
    Quaternion rotation    = Quaternion::RotationIdentity();
    Vector3dd  translation = Vector3dd(-1.0, 0.0, 0.0);
    CostFunctionBase::packState(&input[0], rotation, translation);

    vector<double> optInput = GDfit.minimise(input);
    return CostFunctionBase::getEssential(&optInput[0]);
}

EssentialMatrix EssentialEstimator::getEssentialGradToRm(const vector<Correspondence*> & samples)
{
    CostFunction7toN costFunction(&samples);
    NormalizeFunction normalise;
    GradientDescentRnToRm GDfit;
    GDfit.f = &costFunction;
    GDfit.normalisation = &normalise;
    GDfit.maxIterations = 1000000;
    GDfit.lambda = 0.003;

    vector<double> input(CostFunctionBase::VECTOR_SIZE);
    Quaternion rotation    = Quaternion::RotationIdentity();
    Vector3dd  translation = Vector3dd(-1.0, 0.0, 0.0);
    CostFunctionBase::packState(&input[0], rotation, translation);

    vector<double> output(samples.size(), 0.0);

    vector<double> optInput = GDfit.fit(input, output);
    return CostFunctionBase::getEssential(&optInput[0]);
}

EssentialMatrix EssentialEstimator::getEssentialSimpleKalman (const vector<Correspondence *> &samples)
{
    IdentityFunction F(CostFunctionBase::VECTOR_SIZE);
    CostFunction7to1 H(&samples);
    NormalizeFunction N;

    // We know almost nothing about the essential matrix
    Matrix P(CostFunctionBase::VECTOR_SIZE, CostFunctionBase::VECTOR_SIZE, 500.0);
    // Our model evolution is precise
    Matrix Q(CostFunctionBase::VECTOR_SIZE, CostFunctionBase::VECTOR_SIZE, 0.0);
    // Our measurement is precise up to 2px.
    Matrix R(1,1, 2.0);

    Vector input(CostFunctionBase::VECTOR_SIZE);
    Quaternion rotation    = Quaternion::RotationIdentity();
    Vector3dd  translation = Vector3dd(-1.0, 0.0, 0.0);
    CostFunctionBase::packState(&input[0], rotation, translation);

    ClassicKalman kalmanFilter(
        &F,
        &H,
        Q,
        R,
        P,
        input);


    Vector normalised(CostFunctionBase::VECTOR_SIZE);

    static int maxIterations = 2500000;

    cout << "================== Starting Simple Kalman optimisation ================== " << endl;
    cout << "[" << flush;

    for (int step = 0; step < maxIterations; step++)
    {
        if ((step % ((maxIterations / 100) + 1) == 0))
        {
            cout << "#" << flush;
        }

        double measurement = 0.0;
        kalmanFilter.predict();
        N(kalmanFilter.xPredicted, normalised);
        kalmanFilter.xPredicted = normalised;
        kalmanFilter.z[0] = measurement;
        kalmanFilter.update();
    }
    cout << "]" << endl;

    return CostFunctionBase::getEssential(&kalmanFilter.x[0]);
}

EssentialMatrix EssentialEstimator::getEssential             (
        const vector<Correspondence *> &samples,
        OptimisationMethod method)
{
    switch (method)
    {
        case METHOD_SVD_LSE:
            return getEssentialLSE(samples);
        case METHOD_GRAD_DESC:
            return getEssentialGrad(samples);
        case METHOD_MARQ_LEV:
            return getEssentialLM(samples);
        case METHOD_CLASSIC_KALMAN:
            return getEssentialSimpleKalman(samples);
        case METHOD_GRAD_DESC_RM:
           return getEssentialGradToRm(samples);

        default:
            return getEssentialLM(samples);
    }

    return EssentialMatrix();
}

std::vector<EssentialMatrix> EssentialEstimator::getEssentials(const vector<Correspondence *> &samples, EssentialEstimator::OptimisationMethod method)
{
    std::vector<EssentialMatrix> result;

    switch (method)
    {
        case METHOD_SVD_LSE:
            result.push_back(getEssentialLSE(samples)); break;
        case METHOD_GRAD_DESC:
            result.push_back(getEssentialGrad(samples)); break;
        case METHOD_GRAD_DESC_RM:
            result.push_back(getEssentialGradToRm(samples)); break;

        case METHOD_5_POINT:
            result = getEssential5point(samples); break;
        case METHOD_7_POINT:
            result = getEssential5point(samples); break;
        case METHOD_MARQ_LEV:
            result.push_back(getEssentialLM(samples)); break;
        case METHOD_CLASSIC_KALMAN:
            result.push_back(getEssentialSimpleKalman(samples)); break;
        default:
            result.push_back(getEssentialLM(samples)); break;
    }

    return result;
}



Quaternion EssentialEstimator::CostFunctionBase::getRotationQ(const double in[])
{
    return Quaternion(in[ROTATION_Q_X], in[ROTATION_Q_Y], in[ROTATION_Q_Z], in[ROTATION_Q_T]).normalised();
}

Vector3dd EssentialEstimator::CostFunctionBase::getTranslation(const double in[])
{
    return Vector3dd(in[TRANSLATION_X], in[TRANSLATION_Y], in[TRANSLATION_Z]).normalised();
}

Matrix33 EssentialEstimator::CostFunctionBase::getRotationM(const double in[])
{
    return getRotationQ(in).toMatrix();
}

double EssentialEstimator::CostFunctionBase::getRotationAngle(const double in[])
{
    return getRotationQ(in).getAngle();
}

Vector3dd EssentialEstimator::CostFunctionBase::getRotationAxis(const double in[])
{
    return getRotationQ(in).getAxis();
}

EssentialMatrix EssentialEstimator::CostFunctionBase::getEssential(const double in[])
{
    return EssentialMatrix::compose(getRotationM(in), getTranslation(in));
}

void EssentialEstimator::CostFunctionBase::packState(double out[], const Quaternion &q, const Vector3dd &t)
{
    out[CostFunctionBase::ROTATION_Q_X]  = q.x();
    out[CostFunctionBase::ROTATION_Q_Y]  = q.y();
    out[CostFunctionBase::ROTATION_Q_Z]  = q.z();
    out[CostFunctionBase::ROTATION_Q_T]  = q.t();
    out[CostFunctionBase::TRANSLATION_X] = t.x();
    out[CostFunctionBase::TRANSLATION_Y] = t.y();
    out[CostFunctionBase::TRANSLATION_Z] = t.z();
}

double EssentialEstimator::CostFunction7to1::getCost(const EssentialMatrix &matrix) const
{
    double cost = 0.0;
    for (unsigned i = 0; i < samples->size(); i++)
    {
        double fitness = matrix.epipolarDistance(*(samples->at(i)));
        cost += fitness * fitness;
    }
    return cost;
}

void EssentialEstimator::CostFunction7to1::operator()(const double in[], double out[])
{
    EssentialMatrix m = getEssential(in);
    out[0] = getCost(m);
}

/**
 **/
Matrix EssentialEstimator::CostFunction7to1::getJacobian(const double in[], double delta)
{
    Matrix result(outputs, inputs);
    // is it needed here?
//    EssentialMatrix m = getEssential(in);

    vector<double> xc(inputs);
    EssentialMatrix m_minus(Matrix33(1.0));
    EssentialMatrix m_plus (Matrix33(1.0));

    for (int i = 0; i < inputs; i++)
    {
       xc[i] = in[i];
    }

    for (int i = 0; i < inputs; i++)
    {
       xc[i] = in[i] - delta;
       m_minus = getEssential(&xc[0]);
       xc[i] = in[i] + delta;
       m_plus  = getEssential(&xc[0]);
       xc[i] = in[i];

       double sum_plus  = 0.0;
       double sum_minus = 0.0;

       for (unsigned j = 0; j < samples->size(); j++)
       {
           Correspondence *corr = samples->at(j);
           double value_plus  = m_plus .epipolarDistance(*corr);
           double value_minus = m_minus.epipolarDistance(*corr);
           sum_plus  += value_plus  * value_plus ;
           sum_minus += value_minus * value_minus;
       }
       result.element(0,i) = (sum_plus - sum_minus) / (2.0 * delta);

/*
       double d = 0.0;

       for (unsigned j = 0; j < samples->size(); j++)
       {
           Correspondence *corr = samples->at(j);
           double value_plus  = m_plus .epipolarDistance(*corr);
           double value_minus = m_minus.epipolarDistance(*corr);
           double value       = m.      epipolarDistance(*corr);

           double dvalue = value * (value_plus - value_minus) / delta;
           d += dvalue;
       }
       result.element(0,i) = d;
*/
/*
       double d = 0.0;

       for (unsigned j = 0; j < samples->size(); j++)
       {
           Correspondence *corr = samples->at(j);
           double value_plus  = m_plus .epipolarDistance(*corr);
           double value_minus = m_minus.epipolarDistance(*corr);
           double value       = m.      epipolarDistance(*corr);

           d += value * (value_plus - value_minus);
       }
       result.element(0,i) = d / delta;
*/
/*       double d = 0.0;
       double sum_plus  = 0.0;
       double sum_minus = 0.0;

       for (unsigned j = 0; j < samples->size(); j++)
       {
           Correspondence *corr = samples->at(j);
           double value_plus  = m_plus .epipolarDistance(*corr);
           double value_minus = m_minus.epipolarDistance(*corr);
           double value       = m.      epipolarDistance(*corr);

           sum_plus  += value * value_plus ;
           sum_minus += value * value_minus;
       }
       result.element(0,i) = (sum_plus - sum_minus) / delta;*/


    }

//    Matrix result1 = FunctionArgs::getJacobian(in);
//    cout << result  << endl;
//    cout << result1 << endl;

    return result;
}

#if 0
template<typename ElementType>
typename FixMatrixFixed<ElementType, 3, 3> matrixFromModel(const double in[EssentialEstimator::CostFunctionBase::VECTOR_SIZE])
{
    typedef FixMatrixFixed<PackedDerivative<EssentialEstimator::CostFunctionBase::VECTOR_SIZE>, 3, 3> Matrix33Diff;
    typedef GenericQuaternion< PackedDerivative<EssentialEstimator::CostFunctionBase::VECTOR_SIZE> >   QuaternionDiff;
    typedef Vector3d< PackedDerivative<EssentialEstimator::CostFunctionBase::VECTOR_SIZE> >   VectorDiff;

    Matrix result(outputs, inputs);
    QuaternionDiff inRot(
            PackedDerivative<VECTOR_SIZE>::ID(in[ROTATION_Q_X], ROTATION_Q_X),
            PackedDerivative<VECTOR_SIZE>::ID(in[ROTATION_Q_Y], ROTATION_Q_Y),
            PackedDerivative<VECTOR_SIZE>::ID(in[ROTATION_Q_Z], ROTATION_Q_Z),
            PackedDerivative<VECTOR_SIZE>::ID(in[ROTATION_Q_T], ROTATION_Q_T));

    VectorDiff   inTrans(
            PackedDerivative<VECTOR_SIZE>::ID(in[TRANSLATION_X], TRANSLATION_X),
            PackedDerivative<VECTOR_SIZE>::ID(in[TRANSLATION_Y], TRANSLATION_Y),
            PackedDerivative<VECTOR_SIZE>::ID(in[TRANSLATION_Z], TRANSLATION_Z)
            );

    Matrix33Diff matTrans;
    matTrans.atm(0,0) =  0.0;         matTrans.atm(0,1) = -inTrans.z(); matTrans.atm(0,2) =  inTrans.y();
    matTrans.atm(1,0) =  inTrans.z(); matTrans.atm(1,1) =          0.0; matTrans.atm(1,2) = -inTrans.x();
    matTrans.atm(2,0) = -inTrans.y(); matTrans.atm(2,1) =  inTrans.x(); matTrans.atm(2,2) =          0.0;

    Matrix33Diff matRot = inRot.toMatrixGeneric<Matrix33Diff>();

    Matrix33Diff input = matTrans.mul(matRot);
    return input;

}


template<typename ElementType>
    void essentialDerivative(
        const double in[EssentialEstimator::CostFunctionBase::VECTOR_SIZE],
        ElementType out[EssentialEstimator::CostFunctionBase::VECTOR_SIZE],
        const Correspondence &correspondance
    )
{
    return;
}
#endif

Matrix EssentialEstimator::CostFunction7toNPacked::getJacobian(const double in[], double delta)
{
    CORE_UNUSED(delta);

    typedef FixMatrixFixed<PackedDerivative<EssentialEstimator::CostFunctionBase::VECTOR_SIZE>, 3, 3> Matrix33Diff;
    typedef GenericQuaternion< PackedDerivative<EssentialEstimator::CostFunctionBase::VECTOR_SIZE> >   QuaternionDiff;
    typedef Vector3d< PackedDerivative<EssentialEstimator::CostFunctionBase::VECTOR_SIZE> >   VectorDiff;

    Matrix result(outputs, inputs, false);
    QuaternionDiff inRot(
            PackedDerivative<VECTOR_SIZE>::ID(in[ROTATION_Q_X], ROTATION_Q_X),
            PackedDerivative<VECTOR_SIZE>::ID(in[ROTATION_Q_Y], ROTATION_Q_Y),
            PackedDerivative<VECTOR_SIZE>::ID(in[ROTATION_Q_Z], ROTATION_Q_Z),
            PackedDerivative<VECTOR_SIZE>::ID(in[ROTATION_Q_T], ROTATION_Q_T));

    VectorDiff   inTrans(
            PackedDerivative<VECTOR_SIZE>::ID(in[TRANSLATION_X], TRANSLATION_X),
            PackedDerivative<VECTOR_SIZE>::ID(in[TRANSLATION_Y], TRANSLATION_Y),
            PackedDerivative<VECTOR_SIZE>::ID(in[TRANSLATION_Z], TRANSLATION_Z)
            );

    Matrix33Diff matTrans;
    matTrans.atm(0,0) =  0.0;         matTrans.atm(0,1) = -inTrans.z(); matTrans.atm(0,2) =  inTrans.y();
    matTrans.atm(1,0) =  inTrans.z(); matTrans.atm(1,1) =          0.0; matTrans.atm(1,2) = -inTrans.x();
    matTrans.atm(2,0) = -inTrans.y(); matTrans.atm(2,1) =  inTrans.x(); matTrans.atm(2,2) =          0.0;

    Matrix33Diff matRot = inRot.toMatrixGeneric<Matrix33Diff>();

    Matrix33Diff input = matTrans.mul(matRot);


    for (unsigned j = 0; j < samples->size(); j++)
    {
        /**
         * Vector3dd line = this->mulBy2dLeft(right);
         * Line2d epiline(line);
         * return epiline.distanceTo(left);
         **/

        Correspondence *corr = samples->at(j);
        Vector2dd start = corr->start;
        Vector2dd end   = corr->end;

        VectorDiff line (
            start.x() * input.atm(0,0) + start.y() * input.atm(1,0) + input.atm(2,0),
            start.x() * input.atm(0,1) + start.y() * input.atm(1,1) + input.atm(2,1),
            start.x() * input.atm(0,2) + start.y() * input.atm(1,2) + input.atm(2,2)
        );

        /**
         *  double d = pointWeight(point);
            if (d < 0) d = -d;
            double l = normal().l2Metric();
            return (d / l);
        **/


        PackedDerivative<VECTOR_SIZE> weight = line.x() * end.x() + line.y() * end.y() + line.z();
        PackedDerivative<VECTOR_SIZE> l = line.xy().l2Metric();
        PackedDerivative<VECTOR_SIZE> error = weight / l;

        if (error.value < 0 ) error = -error;

        for (int i = 0; i < inputs; i++)
        {
            result.element(j,i) = error.derivative[i];
        }
    }
    return result;
}

#if 1

EssentialEstimator::CostFunction7toNPacked::Matrix33Diff EssentialEstimator::CostFunction7toNPacked::essentialAST()
{

    QuaternionDiff inRot(
            ASTNode("Qx"),
            ASTNode("Qy"),
            ASTNode("Qz"),
            ASTNode("Qt"));

    Vector3dDiff  inTrans(
            ASTNode("Tx"),
            ASTNode("Ty"),
            ASTNode("Tz")
            );

    Matrix33Diff matTrans;
    matTrans.atm(0,0) =  ASTNode(0.0); matTrans.atm(0,1) = -inTrans.z() ; matTrans.atm(0,2) =  inTrans.y();
    matTrans.atm(1,0) =  inTrans.z() ; matTrans.atm(1,1) =  ASTNode(0.0); matTrans.atm(1,2) = -inTrans.x();
    matTrans.atm(2,0) = -inTrans.y() ; matTrans.atm(2,1) =  inTrans.x() ; matTrans.atm(2,2) = ASTNode(0.0);

    Matrix33Diff matRot = inRot.toMatrixGeneric<Matrix33Diff>();
    Matrix33Diff input = matTrans.mul(matRot);
    return input;
}

ASTNodeInt *EssentialEstimator::CostFunction7toNPacked::derivative(const EssentialEstimator::CostFunction7toNPacked::Matrix33Diff &input)
{
    //Matrix33Diff input = essentialAST();

    Vector2dDiff start(ASTNode("startx"), ASTNode("starty"));
    Vector2dDiff end  (ASTNode("endx")  , ASTNode("endy"));

    Vector3dDiff line (
            start.x() * input.atm(0,0) + start.y() * input.atm(1,0) + input.atm(2,0),
            start.x() * input.atm(0,1) + start.y() * input.atm(1,1) + input.atm(2,1),
            start.x() * input.atm(0,2) + start.y() * input.atm(1,2) + input.atm(2,2)
        );

        /**
         *  double d = pointWeight(point);
            if (d < 0) d = -d;
            double l = normal().l2Metric();
            return (d / l);
        **/


    ASTNode weight = line.x() * end.x() + line.y() * end.y() + line.z();
    //if (weight.value < 0 ) weight = -weight;

    ASTNode l = sqrt(line.x() * line.x() + line.y() * line.y());
    ASTNode error = weight / l;
    return error.p;
}
#endif


void EssentialEstimator::CostFunction7toN::operator()(const double in[], double out[])
{
    EssentialMatrix matrix = getEssential(in);
    for (unsigned i = 0; i < samples->size(); i++)
    {
        out[i] = matrix.epipolarDistance(*(samples->at(i)));
    }
#if 0
    cout << endl;
    for (unsigned i = 0; i < VECTOR_SIZE; i++)
    {
        cout << in[i] << " ";
    }
    cout << endl;
#endif
}

/*Matrix EssentialEstimator::MultioutCostFunction::getJacobian(const double in[], double delta = 1e-7)
{


}*/

void EssentialEstimator::NormalizeFunction::operator()(const double in[], double out[])
{

    Quaternion rotation    = CostFunctionBase::getRotationQ(in);
    Vector3dd  translation = CostFunctionBase::getTranslation(in);
    rotation.normalise();
    translation.normalise();
    CostFunctionBase::packState(out, rotation, translation);
}


EssentialEstimator::~EssentialEstimator()
{
}

void   derivative (const double in[], double out[], Correspondence *c);
Matrix derivative (const double in[], const vector<Correspondence *> *samples);
Matrix derivative2(const double in[], const vector<Correspondence *> *samples);

Matrix EssentialEstimator::CostFunction7toNGenerated::getJacobian(const double in[], double delta)
{
    CORE_UNUSED(delta);
    Matrix result(outputs, inputs);
    for (unsigned j = 0; j < samples->size(); j++)
    {
        //double out[7];
        Correspondence *corr = samples->at(j);
        derivative(in, &result.element(j, 0), corr);
        /*for (int i = 0; i < 7; i++)
        {
            result.element(j, i) = out[i];
        }*/
    }
    return result;
}

Matrix EssentialEstimator::CostFunction7toNGenerated1::getJacobian(const double in[], double /*delta*/)
{
    return derivative2(in, samples);
}


} //namespace corecvs

