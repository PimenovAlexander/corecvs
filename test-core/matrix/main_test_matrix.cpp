/**
 * \file main_test_matrix.cpp
 * \brief This is the main file for the test matrix
 *
 * \date Aug 21, 2010
 * \author alexander
 *
 * \ingroup autotest
 */

#include <sstream>
#include <iostream>
#include <limits>
#include <random>
#include <chrono>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/math/mathUtils.h"
#include "core/math/matrix/matrix33.h"
#include "core/math/matrix/matrix.h"
#include "core/math/sparseMatrix.h"
#include "core/utils/preciseTimer.h"
#include "core/function/function.h"
#include "core/iterative/minresQLP.h"
#include "core/iterative/pcg.h"

#if 0
#include "core/math/em.h"
#endif

using namespace corecvs;

const int SEED = 1337;

std::pair<bool, SparseMatrix>  NaiveIncompleteCholesky(SparseMatrix &B)
{
    auto A = B;
    int N = A.h;
    int n = N;
    for (int k = 0; k < N; ++k)
    {
        double pivot = A.a(k, k);
        if (pivot <= 0.0)
        {
            std::cout << "Non-positive pivot N" << std::endl;
            return std::make_pair(false, SparseMatrix());
        }
        CORE_ASSERT_TRUE_S(A.a(k, k) == pivot);
        pivot = std::sqrt(pivot);
        A.a(k, k) = pivot;


        for (int i = k + 1; i < n; ++i)
            if (A.a(k, i) != 0.0)
                A.a(k, i) /= pivot;


        for (int j = k + 1; j < n; ++j)
            for (int i = j; i < n; ++i)
                if (A.a(j, i) != 0.0)
                    A.a(j, i) -= A.a(k, i) * A.a(k, j);
    }

    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j)
            A.a(j, i) = 0.0;
    return std::make_pair(true, A);
}
TEST(Matrix, dtrsv_ut)
{
    std::mt19937 rng(SEED);
    std::uniform_real_distribution<double> runif(-10, 10);
    double nnz = 0.01;
    int N = 100;
    for (int it= 0; it< 1000; ++it)
    {
        int realN = N + rng() % N;
        Matrix U(realN, realN);
        for (int k = 0; k < nnz * realN * realN; ++k)
        {
            int I = rng() % (realN * realN + realN);
            int ii = I < realN * realN ? I / realN : I - realN * realN;
            int jj = I < realN * realN ? I % realN : I - realN * realN;
            if (ii > jj)
                std::swap(ii, jj);
            U.a(ii, jj) = runif(rng);
        }
        Vector x(realN);
        for (int i = 0; i < realN; ++i)
        {
            U.a(i, i) = runif(rng);
            x[i] = runif(rng);
        }
        Matrix su(U);
        auto rhs = x * su;
        auto xx = su.dtrsv(rhs, true, false);
        ASSERT_LE(!(xx-x)/!x , 1e-6);
    }
}

TEST(Matrix, dtrsv_lt)
{
    std::mt19937 rng(SEED);
    std::uniform_real_distribution<double> runif(-10, 10);
    double nnz = 0.01;
    int N = 100;
    for (int it= 0; it< 1000; ++it)
    {
        int realN = N + rng() % N;
        Matrix U(realN, realN);
        for (int k = 0; k < nnz * realN * realN; ++k)
        {
            int I = rng() % (realN * realN + realN);
            int ii = I < realN * realN ? I / realN : I - realN * realN;
            int jj = I < realN * realN ? I % realN : I - realN * realN;
            if (ii < jj)
                std::swap(ii, jj);
            U.a(ii, jj) = runif(rng);
        }
        Vector x(realN);
        for (int i = 0; i < realN; ++i)
        {
            U.a(i, i) = runif(rng);
            x[i] = runif(rng);
        }
        Matrix su(U);
        auto rhs = x * su;
        auto xx = su.dtrsv(rhs, false, false);
        ASSERT_LE(!(xx-x)/!x , 1e-6);
    }
}


TEST(Matrix, dtrsv_un)
{
    std::mt19937 rng(SEED);
    std::uniform_real_distribution<double> runif(-10, 10);
    double nnz = 0.01;
    int N = 100;
    for (int it= 0; it< 1000; ++it)
    {
        int realN = N + rng() % N;
        Matrix U(realN, realN);
        for (int k = 0; k < nnz * realN * realN; ++k)
        {
            int I = rng() % (realN * realN + realN);
            int ii = I < realN * realN ? I / realN : I - realN * realN;
            int jj = I < realN * realN ? I % realN : I - realN * realN;
            if (ii > jj)
                std::swap(ii, jj);
            U.a(ii, jj) = runif(rng);
        }
        Vector x(realN);
        for (int i = 0; i < realN; ++i)
        {
            U.a(i, i) = runif(rng);
            x[i] = runif(rng);
        }
        Matrix su(U);
        auto rhs = su * x;
        auto xx = su.dtrsv(rhs, true, true);
        ASSERT_LE(!(xx-x)/!x , 1e-6);

    }
}

TEST(Matrix, dtrsv_ln)
{
    std::mt19937 rng(SEED);
    std::uniform_real_distribution<double> runif(-1, 1);
    double nnz = 0.01;
    int N = 100;
    for (int it= 0; it< 1000; ++it)
    {
        int realN = N + rng() % N;
        Matrix U(realN, realN);
        for (int k = 0; k < nnz * realN * realN; ++k)
        {
            int I = rng() % (realN * realN + realN);
            int ii = I < realN * realN ? I / realN : I - realN * realN;
            int jj = I < realN * realN ? I % realN : I - realN * realN;
            if (ii < jj)
                std::swap(ii, jj);
            U.a(ii, jj) = runif(rng);
        }
        Vector x(realN);
        for (int i = 0; i < realN; ++i)
        {
            U.a(i, i) = runif(rng);
            x[i] = runif(rng);
        }
        Matrix su(U);
        auto rhs = su * x;
        auto xx = su.dtrsv(rhs, false, true);
        ASSERT_LE(!(xx-x)/!x , 1e-6);

    }
}
TEST(SparseMatrix, dtrsv_ut)
{
    std::mt19937 rng(SEED);
    std::uniform_real_distribution<double> runif(-100, 100);
    double nnz = 0.001;
    int N = 1000;
    for (int it= 0; it< 1000; ++it)
    {
        int realN = N + rng() % N;
        Matrix U(realN, realN);
        for (int k = 0; k < nnz * realN * realN; ++k)
        {
            int I = rng() % (realN * realN + realN);
            int ii = I < realN * realN ? I / realN : I - realN * realN;
            int jj = I < realN * realN ? I % realN : I - realN * realN;
            if (ii > jj)
                std::swap(ii, jj);
            U.a(ii, jj) = runif(rng);
        }
        Vector x(realN);
        for (int i = 0; i < realN; ++i)
        {
            U.a(i, i) = runif(rng);
            x[i] = runif(rng);
        }
        SparseMatrix su(U);
        auto rhs = x * su;
        auto xx = su.dtrsv_ut(rhs);
        ASSERT_LE(!(xx-x)/!x , 1e-6);
    }
}

TEST(SparseMatrix, dtrsv_lt)
{
    std::mt19937 rng(SEED);
    std::uniform_real_distribution<double> runif(-100, 100);
    double nnz = 0.001;
    int N = 1000;
    for (int it= 0; it< 1000; ++it)
    {
        int realN = N + rng() % N;
        Matrix U(realN, realN);
        for (int k = 0; k < nnz * realN * realN; ++k)
        {
            int I = rng() % (realN * realN + realN);
            int ii = I < realN * realN ? I / realN : I - realN * realN;
            int jj = I < realN * realN ? I % realN : I - realN * realN;
            if (ii < jj)
                std::swap(ii, jj);
            U.a(ii, jj) = runif(rng);
        }
        Vector x(realN);
        for (int i = 0; i < realN; ++i)
        {
            U.a(i, i) = runif(rng);
            x[i] = runif(rng);
        }
        SparseMatrix su(U);
        auto rhs = x * su;
        auto xx = su.dtrsv_lt(rhs);
        ASSERT_LE(!(xx-x)/!x , 1e-6);
    }
}


TEST(SparseMatrix, dtrsv_un)
{
    std::mt19937 rng(SEED);
    std::uniform_real_distribution<double> runif(-100, 100);
    double nnz = 0.001;
    int N = 1000;
    for (int it= 0; it< 1000; ++it)
    {
        int realN = N + rng() % N;
        Matrix U(realN, realN);
        for (int k = 0; k < nnz * realN * realN; ++k)
        {
            int I = rng() % (realN * realN + realN);
            int ii = I < realN * realN ? I / realN : I - realN * realN;
            int jj = I < realN * realN ? I % realN : I - realN * realN;
            if (ii > jj)
                std::swap(ii, jj);
            U.a(ii, jj) = runif(rng);
        }
        Vector x(realN);
        for (int i = 0; i < realN; ++i)
        {
            U.a(i, i) = runif(rng);
            x[i] = runif(rng);
        }
        SparseMatrix su(U);
        auto rhs = su * x;
        auto xx = su.dtrsv_un(rhs);
        ASSERT_LE(!(xx-x)/!x , 1e-6);

    }
}

TEST(SparseMatrix, dtrsv_ln)
{
    std::mt19937 rng(SEED);
    std::uniform_real_distribution<double> runif(-100, 100);
    double nnz = 0.001;
    int N = 1000;
    for (int it= 0; it< 1000; ++it)
    {
        int realN = N + rng() % N;
        Matrix U(realN, realN);
        for (int k = 0; k < nnz * realN * realN; ++k)
        {
            int I = rng() % (realN * realN + realN);
            int ii = I < realN * realN ? I / realN : I - realN * realN;
            int jj = I < realN * realN ? I % realN : I - realN * realN;
            if (ii < jj)
                std::swap(ii, jj);
            U.a(ii, jj) = runif(rng);
        }
        Vector x(realN);
        for (int i = 0; i < realN; ++i)
        {
            U.a(i, i) = runif(rng);
            x[i] = runif(rng);
        }
        SparseMatrix su(U);
        auto rhs = su * x;
        auto xx = su.dtrsv_ln(rhs);
        ASSERT_LE(!(xx-x)/!x , 1e-6);

    }
}

TEST(SparseMatrix, IncompleteCholesky)
{
    std::mt19937 rng(SEED);
    std::uniform_real_distribution<double> runif(-1, 1);
    double a[] = {
        2.0, 0.0, 1.0, 0.0,
        0.0, 2.0, 0.0, 0.0,
        1.0, 0.0, 2.0, 0.0,
        0.0, 0.0, 0.0, 2.0
    };
    SparseMatrix sm(Matrix(4, 4, a));
    auto resNaive = NaiveIncompleteCholesky(sm);
    auto res      = sm.incompleteCholseky();
    std::cout << res.second << std::endl;
    ASSERT_EQ(resNaive.first, res.first);
    ASSERT_TRUE(resNaive.first);
    ASSERT_LE(Matrix(resNaive.second - res.second).frobeniusNorm() , 1e-9);
    ASSERT_LE(Matrix(res.second.ata() - sm).frobeniusNorm(), 1e-3);
}


#if defined(WITH_FMA) && defined(WITH_BLAS)
TEST(Iterative, MinresQLPDaxpby)
{
    std::mt19937 rng(SEED);
    const int N =15000;

    double golden_total = 0.0, daxpby_total = 0.0, blas_total = 0.0;
    for (int i = 0; i < 10000; ++i)
    {
        std::uniform_real_distribution<double> runif(-1000, 1000);
        int NN = N + (rng() % 1000);

        Vector x(NN), y(NN), res(NN);
        double a = runif(rng), b = runif(rng);
        for (int i = 0; i < NN; ++i)
        {
            x[i] = runif(rng);
            y[i] = runif(rng);
        }

        auto golden_start = std::chrono::high_resolution_clock::now();
        auto golden = a * x + b * y;
        auto golden_stop  = std::chrono::high_resolution_clock::now();
        auto golden_time = (golden_stop - golden_start).count() * 1.0;

        auto daxpby_start = std::chrono::high_resolution_clock::now();
        MinresQLP<SparseMatrix>::Daxpby(res, a, x, b, y);
        auto daxpby_stop = std::chrono::high_resolution_clock::now();
        auto daxpby_time = (daxpby_stop - daxpby_start).count() * 1.0;

        ASSERT_LE(!(res - golden)/!golden, 1e-15);

        auto blas_start = std::chrono::high_resolution_clock::now();
        res = x;
        cblas_dscal(res.size(), a, &res[0], 1);
        cblas_daxpy(res.size(), b, &y[0], 1, &res[0], 1);
        auto blas_stop = std::chrono::high_resolution_clock::now();
        auto blas_time = (blas_stop - blas_start).count() * 1.0;
        blas_total += blas_time;
        golden_total += golden_time;
        daxpby_total += daxpby_time;


    }
    std::cout << "golden/daxpby: " << golden_total / daxpby_total << "x slower" << std::endl;
    std::cout << "blas/daxpby: " << blas_total / daxpby_total << "x slower" << std::endl;
}
#endif

TEST(Iterative, MinresQLP)
{
    int N = 32768;

    int val[] = {1, 2, 5, 2, 1};
    std::vector<double> values;
    std::vector<int> columns, rowPointers(N + 1);
    for (int i = 0; i < N; ++i)
    {
        for (int j = -2; j <= 2; ++j)
            if (i + j >= 0 && i + j < N)
            {
                columns.push_back(i + j);
                values.push_back(val[j + 2]);
            }
        rowPointers[i + 1] = (int)values.size();
    }
    corecvs::SparseMatrix M(N, N, values, columns, rowPointers);
    corecvs::Vector xx(N);
    for (int i = 0; i < N; ++i)
        xx[i] = i % 10;
    auto b = M * xx;
    corecvs::Vector x;
    MinresQLP<SparseMatrix>::Solve(M, b, x);
    ASSERT_LE(!(M*x-b)/!b, 1e-9);
//    PCG<SparseMatrix>::Solve(M, b, x);
//    ASSERT_LE(!(M*x-b)/!b, 1e-9);
}

TEST(Iterative, MinresQLPPreconditioned)
{
    int N = 32768;

    int val[] = {1, 2, 5, 2, 1};
    std::vector<double> values;
    std::vector<int> columns, rowPointers(N + 1);
    for (int i = 0; i < N; ++i)
    {
        for (int j = -2; j <= 2; ++j)
            if (i + j >= 0 && i + j < N)
            {
                columns.push_back(i + j);
                values.push_back(val[j + 2]);
            }
        rowPointers[i + 1] = (int)values.size();
    }
    corecvs::SparseMatrix M(N, N, values, columns, rowPointers);
    corecvs::Vector xx(N);
    for (int i = 0; i < N; ++i)
        xx[i] = i % 10;
    auto b = M * xx;
    auto P = M.incompleteCholseky();
    if (!P.first)
    {
        std::cout << "NAH: ICP0 failed" << std::endl;
        ASSERT_TRUE(false);
    }
    corecvs::Vector x;
    MinresQLP<SparseMatrix>::Solve(M, [&](const Vector &x) { return P.second.dtrsv_un(P.second.dtrsv_ut(x)); }, b, x);
    ASSERT_LE(!(M*x-b)/!b, 1e-9);
    PCG<SparseMatrix>::Solve(M, [&](const Vector &x) { return P.second.dtrsv_un(P.second.dtrsv_ut(x)); }, b, x);
    ASSERT_LE(!(M*x-b)/!b, 1e-9);
}

struct Blah : public FunctionArgs
{
    Blah() : FunctionArgs(10, 10)
    {
    }
    virtual void operator() (const double *in, double *out)
    {
        for (int i = 0; i < 10; ++i)
            out[i] = in[i];
    }
};

TEST(VectorTest, testVector)
{
    Vector v(10);
    for (auto& vv: v)
        vv = &vv - v.begin();
    Vector vv(v);
    Vector vvv(10);
    vvv = v;
    v[0] = 10;
    vv[1] = 10;
    vvv[2] = 10;
    std::cout << v << std::endl;
    std::cout << vv << std::endl;
    std::cout << vvv << std::endl;

    Vector blah {1, 2, 3, 4, 5, 6};
    std::cout << blah << std::endl;

    Blah b;
    b(vv, vvv);
    std::cout << vv << vvv << std::endl;

    Vector dc(10);
    std::cout << dc << std::endl;
}

TEST(MatrixTest, testMatrix33)
{
    // Test case 1: Test general constructors
    cout << "Testing Matrix33\n";
    double data[] = {
            1.0,2.0,3.0,
            4.0,5.0,7.0,
            8.0,9.0,10.0 };
    Matrix33 a1(data);
    Matrix33 a2(
            1.0,2.0,3.0,
            4.0,5.0,7.0,
            8.0,9.0,10.0);

    CORE_ASSERT_TRUE(a1 == a2, "Matrix should be equal");
    CORE_ASSERT_FALSE(a1 != a2, "Matrix should not be unequal");


    // Test case 2 : Test inversion and multiplication
    Matrix33 b1 = Matrix33 (
             1,  2,  3,
             0,  1,  4,
             5,  6,  0);
    Matrix33 b1inv = b1.inv();
    Matrix33 b1res = Matrix33(
             -24,  18,   5,
              20, -15,  -4,
              -5,   4,   1 );

    Matrix33 b2 = b1inv * b1;

    CORE_ASSERT_TRUE(b1inv.notTooFar(b1res, 1e-5), "Inversion gives wrong result");
    CORE_ASSERT_TRUE(b2.notTooFar(Matrix33(1.0), 1e-5), "Inversion of inverse should give unit matrix gives wrong result");

    // Test case 3 : Test constructors
    Matrix33 c1 = Matrix33(
             2.0,0.0,0.0,
             0.0,2.0,0.0,
             0.0,0.0,2.0 );
    Matrix33 c2 = Matrix33(2.0);
    CORE_ASSERT_TRUE(c1 == c2, "Matrix should be equal");

    // Test case 4: Determinant
    Matrix33 d1 = Matrix33 (
             1,  2,  3,
             0,  1,  4,
             5,  6,  0);
    double d1det = d1.det();
    double d1res = 1.0;

    Matrix33 d2 = d1;
    d2.transpose();
    double d2det = d2.det();
    double d2res = 1.0;

    Matrix33 d3 = d1 * 0.5;
    double d3det = d3.det();
    double d3res = 0.5 * 0.5 * 0.5;

    Matrix33 d4 = d1 / 0.5;
    double d4det = d4.det();
    double d4res = 2.0 * 2.0 * 2.0;

    Matrix33 d5 = d1.transposed();
    double d5det = d5.det();
    double d5res = 1.0;

    CORE_ASSERT_TRUE_P(d1det < d1res + 1e-5 && d1det > d1res - 1e-5, ("Determinant should be %lf but is %lf", d1res, d1det));
    CORE_ASSERT_TRUE_P(d2det < d2res + 1e-5 && d2det > d2res - 1e-5, ("Determinant should be %lf but is %lf", d2res, d2det));
    CORE_ASSERT_TRUE_P(d3det < d3res + 1e-5 && d3det > d3res - 1e-5, ("Determinant should be %lf but is %lf", d3res, d3det));
    CORE_ASSERT_TRUE_P(d4det < d4res + 1e-5 && d4det > d4res - 1e-5, ("Determinant should be %lf but is %lf", d4res, d4det));
    CORE_ASSERT_TRUE_P(d5det < d5res + 1e-5 && d5det > d5res - 1e-5, ("Determinant should be %lf but is %lf", d5res, d5det));
}

TEST(MatrixTest, testMatrix44)
{
    Matrix44 a = Matrix44(1.0);
    Matrix44 ares = Matrix44(1.0);

    a *= Matrix33::RotationX(0.05);
    a *= Matrix33::RotationY(0.05);
    a *= Matrix33::RotationZ(-0.05);
    a *= Matrix33::RotationZ(0.05);
    a *= Matrix33::RotationY(-0.05);
    a *= Matrix33::RotationX(-0.05);

    cout << a;
    cout << ares;

    CORE_ASSERT_TRUE(a.notTooFar(ares, 1e-5), "Rotation problem\n");
}

TEST(MatrixTest, testMatrixVectorProduct)
{
    Matrix33 a1 = Matrix33 (
             1,  2,  3,
             0,  1,  4,
             5,  6,  0);
    Vector3dd a2 = Vector3dd(1,2,3);

    Vector3dd a3 = a1 * a2;
    Vector3dd a3res = Vector3dd(1 + 4 + 9, 2 + 12, 5 + 12);
    CORE_ASSERT_TRUE(a3.notTooFar(a3res, 1e-5), "Av matrix vector multiplication fails");


    Vector3dd a4 = a2 * a1;
    Vector3dd a4res = Vector3dd(1 + 15, 2 + 2 + 18, 3 + 8);
    CORE_ASSERT_TRUE(a4.notTooFar(a4res, 1e-5), "vA matrix vector multiplication fails");
}

TEST(MatrixTest, testMatrix44VectorProduct)
{
    Matrix44 matrix = Matrix44 (
             2,  0,  0,  0,
             0,  2,  0,  0,
             0,  0,  2,  0,
             0,  0,  2,  0);


    FixedVector<double, 4> vector;
    vector.element[0] = 3.0;
    vector.element[1] = 4.0;
    vector.element[2] = 5.0;
    vector.element[3] = 1.0;
    FixedVector<double, 4> output = matrix * vector;

    cout << "Input :"  << vector << endl;
    cout << "Output:"  << output << endl;
}

TEST(MatrixTest, testMatrixVectorMult)
{
    double vec1data[] = {1.0, 2.0, 0.5};
    Vector a(3, vec1data);

    double vec2data[] = {1.0, 2.0};
    Vector b(2, vec2data);

    double mat1data[] = {
            -1.0, 0.5,  3.0,
             1.0, 1.5, -2.0 };

    Matrix M(2, 3, mat1data);

    Vector res1 = M * a;
    Vector res2 = b * M;

    cout << "M" << endl << M << endl;
    cout << "a" << endl << a << endl;
    cout << res1 << endl;
    cout << res2 << endl;


    double vec1rdata[2] = {1.5, 3.0};
    double vec2rdata[4] = {1.0, 3.5, -1.0};
    Vector vec1r(2, vec1rdata);
    Vector vec2r(3, vec2rdata);
    CORE_ASSERT_TRUE(vec1r.notTooFar(res1, 1e-7), "Wrong matrix x vector");
    CORE_ASSERT_TRUE(vec2r.notTooFar(res2, 1e-7), "Wrong vector x matrix");
}

TEST(MatrixTest, testMatrixStatics)
{
    Vector3dd a(3.0,-4.0,5.0);
    Matrix33 m1 = Matrix33::CrossProductLeft(a);
    Vector3dd b(-1.0,2.0,-2.0);

    Vector3dd c =  a ^ b;
    Vector3dd d = m1 * b;
    CORE_ASSERT_TRUE(c.notTooFar(d, 1e-5), "Matrix cross product left representation fails");

    Matrix33 m2 = Matrix33::CrossProductRight(a);

    Vector3dd c1 =  b ^ a;
    Vector3dd d1 = m2 * b;

    CORE_ASSERT_TRUE(c1.notTooFar(d1, 1e-5), "Matrix cross product right representation fails");
}

TEST(MatrixTest, testMatrixSVD)
{
    cout << "Testing SVD\n";
    // Test case 1: SVD with CMatrix
    double Adata[16] =
    {   7,      9,   10,  7.6,
       -5, 0.0001,    6, 2345,
        6,     45,  234,  -34,
      657, 0.0001,    0,  -1};

    Matrix *A     = new Matrix(4,4, Adata);
    Matrix *Acopy = new Matrix(A);

    Matrix *W     = new Matrix(1,4);
    Matrix *Wbig  = new Matrix(4,4);
    Matrix *V     = new Matrix(4,4);

    Matrix::svd(A, W, V);

    for (int i = 0; i < 4; i ++)
        Wbig->a(i, i) = W->a(0, i);

    Matrix *VT = V->transposed();

    Matrix *result = A->mul(*Wbig);
    Matrix *result1 = result->mul(*VT);

#ifdef TRACE_TEST
    cout << "A Matrix\n";
    A->print();
    cout << "Wbig Matrix\n";
    Wbig->print();
    cout << "V Matrix\n";
    V->print();
    cout << "Reconstruction Matrix\n";
    result1->print();
    cout << "Should be Matrix\n";
    Acopy->print();
#endif

    CORE_ASSERT_TRUE(result1->notTooFar(Acopy, 1e-7), "Matrix should be equal");
    delete A;
    delete Acopy;
    delete W;
    delete Wbig;
    delete V;

    delete result;
    delete result1;
    delete VT;
}

TEST(MatrixTest, testVector3d)
{
    cout << "Testing Vector3dd\n";

    /*
     * Test Case 1: Vector product
     *  [0.54, -0.7, 6] x [5, 0.4, -7] = [2.499999999999999,33.78,3.716]
     */
    Vector3dd v1 = Vector3dd(0.54, -0.7, 6);
    Vector3dd v2 = Vector3dd(5, 0.4, -7);

    Vector3dd v3   = v1 ^ v2;
    Vector3dd v3res(2.499999999999999,33.78,3.716);

    CORE_ASSERT_TRUE(v3.notTooFar(v3res, 1e-7), "Cross product is wrong");

    double angle1 = v3 & v1;
    double angle2 = v3 & v2;

    CORE_ASSERT_TRUE(angle1 < 1e-7 && angle1 > -1e-7, "Cross product must give orthogonal value");
    CORE_ASSERT_TRUE(angle2 < 1e-7 && angle2 > -1e-7, "Cross product must give orthogonal value");

    /* Test case 2 - arithmetic operations */
    Vector3dd in1(0);
    Vector3dd in2(1.0,-2.0, 3.0);
    Vector3dd in3(1.0, 2.0, 3.0);

    double dotProduct = in2 & in3;
    double dotProductRes = 6.0;
    CORE_ASSERT_DOUBLE_EQUAL(dotProduct, dotProductRes, "Wrong dot product\n");

    in1 = in2 * in3;
    Vector3dd prodRes = Vector3dd(1.0, -4.0, 9.0);
    CORE_ASSERT_TRUE(in1.notTooFar(prodRes, 1e-10), "Wrong elementwise product\n")

    in1 = in2 + in3;
    Vector3dd sumRes = Vector3dd(2.0, 0.0, 6.0);
    CORE_ASSERT_TRUE(in1.notTooFar(sumRes, 1e-10), "Wrong elementwise sum\n")
    in1 = in2;
    in1 += in3;
    CORE_ASSERT_TRUE(in1.notTooFar(sumRes, 1e-10), "Wrong elementwise += sum\n")

    in1 = in2 - in3;
    Vector3dd subRes = Vector3dd(0.0, -4.0, 0.0);
    CORE_ASSERT_TRUE(in1.notTooFar(subRes, 1e-10), "Wrong elementwise subtraction\n")
    in1 = in2;
    in1 -= in3;
    CORE_ASSERT_TRUE(in1.notTooFar(subRes, 1e-10), "Wrong elementwise subtraction\n")

    in1 = -in2;
    Vector3dd negRes = Vector3dd(-1.0,2.0,-3.0);
    CORE_ASSERT_TRUE(in1.notTooFar(negRes, 1e-10), "Wrong elementwise negation\n")

    in1 = in2 * 2;
    Vector3dd intconstRes = Vector3dd(2.0,-4.0,6.0);
    CORE_ASSERT_TRUE(in1.notTooFar(intconstRes, 1e-10), "Wrong elementwise int const mul\n")

    in1 = in2 * 2.0;
    Vector3dd constRes = Vector3dd(2.0,-4.0,6.0);
    CORE_ASSERT_TRUE(in1.notTooFar(constRes, 1e-10), "Wrong elementwise const mul\n")

    double length = v2.l2Metric();
    CORE_ASSERT_DOUBLE_EQUAL_EP(length, 8.61162, 1e-6, ("Wrong norm calculation %lf\n", length));

    double lengthS = v2.l2MetricStable();
    CORE_ASSERT_DOUBLE_EQUAL_EP(lengthS, 8.61162, 1e-6, ("Wrong stable norm calculation %lf\n", lengthS));

}

TEST(MatrixTest, testVector3dOrtogonal)
{
    Vector3dd in(100, 20, 30);
    Vector3dd ort1;
    Vector3dd ort2;

    in.orthogonal(ort1, ort2);
    ASSERT_TRUE( fabs(!ort1 - 1.0) < 1e-8);
    ASSERT_TRUE( fabs(!ort2 - 1.0) < 1e-8);

    ASSERT_TRUE( fabs((ort1 & in)) < 1e-8);
    ASSERT_TRUE( fabs((ort2 & in)) < 1e-8);
    ASSERT_TRUE( fabs((ort2 & ort1)) < 1e-8);
}


TEST(MatrixTest, testVector2d)
{
    cout << "Testing Vector2d\n";
    Vector2dd v1 = Vector2dd(cos(M_PI / 6.0), sin(M_PI / 6.0));
    Vector2dd v2 = Vector2dd(cos(M_PI / 6.0), -sin(M_PI / 6.0));

    double sine    = v1.sineTo(v2);
    double sineRes = -sqrt(3.0) / 2.0;

    DOTRACE(("Sine is %lg should be %lg\n", sine, sineRes));
    CORE_ASSERT_DOUBLE_EQUAL(sine, sineRes, "Sine calculation failed\n");

    double cosine    = v1.cosineTo(v2);
    double cosineRes = 1.0 / 2.0;
    DOTRACE(("Cosine is %lg should be %lg\n", cosine, sineRes));
    CORE_ASSERT_DOUBLE_EQUAL(cosine, cosineRes, "Cosine calculation failed\n");

    Vector2dd v3 = Vector2dd(2.0, 1.0);
    Vector2dd leftNormal  = Vector2dd(-1.0,  2.0);
    Vector2dd rightNormal = Vector2dd( 1.0, -2.0);

    CORE_ASSERT_TRUE(leftNormal.notTooFar(v3.leftNormal(), 1e-10), "Left  Normal Error\n");
    CORE_ASSERT_TRUE(rightNormal.notTooFar(v3.rightNormal(), 1e-10), "Right Normal Error\n");
}

TEST(MatrixTest, testGeneralMatrixVectorProduct)
{
    Matrix A(3, 3);
    Vector V(3);
    A.a(0, 0) = 1.0; A.a(0, 1) = 2.0; A.a(0, 2) = 3.0;
    A.a(1, 0) = 4.0; A.a(1, 1) =-3.0; A.a(1, 2) =-1.0;
    A.a(2, 0) =-1.0; A.a(2, 1) =-1.0; A.a(2, 2) = 3.0;
    V[0] = 2.0; V[1] = 3.0; V[2] = 4.0;

    Vector AV = A * V;
    Vector VA = V * A;
    ASSERT_NEAR(AV[0], 20.0, 1e-9);
    ASSERT_NEAR(AV[1], -5.0, 1e-9);
    ASSERT_NEAR(AV[2],  7.0, 1e-9);
    ASSERT_NEAR(VA[0], 10.0, 1e-9);
    ASSERT_NEAR(VA[1], -9.0, 1e-9);
    ASSERT_NEAR(VA[2], 15.0, 1e-9);
}

TEST(MatrixTest, testMatrixSerialisation)
{
    Matrix33 m = Matrix33::RotationX(0.1) * Matrix33::RotationY(0.1);
    Matrix33 m1;
    std::ostringstream oss;
    m.serialise(oss);
    std::istringstream iss(oss.str(), std::istringstream::in);
    iss >> m1;
    m.print();
    m1.print();
    CORE_ASSERT_TRUE(m1.notTooFar(m), "serialization fails\n");
}

// XXX: https://connect.microsoft.com/VisualStudio/feedback/details/863099/istringstream-fails-to-convert-certain-double-values-correctly
//      This test will never be working on MS VS 2013
#if _MSC_VER == 1800
TEST(MatrixTest, DISABLED_testDouble)
#else
TEST(MatrixTest, testDouble)
#endif
{
    cout << "Testing Double support\n";
    double vals[] = {numeric_limits<double>::min(), 1.0, numeric_limits<double>::max()};
    for (int i = 0; i < 3; i++)
    {
        std::ostringstream oss;
        //cout.precision(numeric_limits<double>::digits10 + 3);
        oss.precision(numeric_limits<double>::digits10 + 3);
        oss  << (double)vals[i];
        oss.flush();
        cout << oss.str() << endl;
        std::istringstream iss(oss.str(), std::istringstream::in);
        double result;
        iss >> result;
        CORE_ASSERT_TRUE_P(result == vals[i], ("Internal problem with double and stdout"));
    }
}

/* TODO: Use abstract buffer binaryOperation elementWize */
Matrix addMatrix(const Matrix &A, const Matrix &B)
{
    //CORE_ASSERT_TRUE (A.w == B.w && A.h == B.h, "Matrices have wrong sizes");
    Matrix result(2, 2, false);

    for (int row = 0; row < result.h ; row++)
    {
        for (int column = 0; column < result.w ; column++)
        {
            result.a(row, column) = A.a(row, column) + B.a(row, column);
        }
    }

    cout << result;

    return result;
}

TEST(MatrixTest, testMatrixOperations)
{
    /*Matrix result(5,5, false);

    printf("PTR: %p\n", &result);
    cout << result <<  endl;*/


    Matrix I2( 5, 5, 2.0);
    Matrix I3( 5, 5, 3.0);
    Matrix I6(5,5);
    Matrix I6a(5,5);
    I6a = (I6 = I2 * I3);
    cout << "I6" << endl << I6 << endl;

    double Adata[] = {
        1.0, 2.0, 3.0,
        0.0, 1.0, 4.0,
        5.0, 6.0, 0.0
    };

    double invAdata[] = {
        -24.0,  18.0,   5.0,
         20.0, -15.0,  -4.0,
         -5.0,   4.0,   1.0
    };

    Matrix A(3,3, Adata);
    Matrix Aorig(3,3, Adata);
    cout << "Org" << endl << A << endl;

    Matrix invA = A.inv();
    Matrix invAr(3,3, invAdata);
    CORE_ASSERT_TRUE(invA.notTooFar(&invAr, 1e-7), "Invalid inversion");
    CORE_ASSERT_TRUE(A.notTooFar(&Aorig, 1e-7), "inversion was not const");
    cout << "Inv" << endl << invA << endl;
    double Adata2[] = {
        1.0, 1.0, 1.0,
        0.0, 0.0, 1.0,
        0.0, 1.0, 1.0
    };

    double invAdata2[] = {
          1.0,   0.0,  -1.0,
          0.0,  -1.0,   1.0,
          0.0,   1.0,   0.0
    };

    Matrix A2(3,3, Adata2);
    Matrix Aorig2(3,3, Adata2);
    cout << "Org" << endl << A2 << endl;

    Matrix invA2 = A2.inv();
    Matrix invAr2(3,3, invAdata2);
    CORE_ASSERT_TRUE(invA2.notTooFar(&invAr2, 1e-7), "Invalid inversion");
    CORE_ASSERT_TRUE(A2.notTooFar(&Aorig2, 1e-7), "inversion was not const");

    Matrix invSVDA = A.invSVD();
    CORE_ASSERT_TRUE(invSVDA.notTooFar(&invAr, 1e-7), "Invalid inversion");
    CORE_ASSERT_TRUE(A.notTooFar(&Aorig, 1e-7), "inversion was not const");
    cout << "Inv SVD" << endl << invA << endl;


    Matrix Acopy = A;
    Acopy.transpose();
    Matrix Atrans = A.t();

    cout << "Trans" << endl << Atrans << endl;
    cout << "Trans inplace" << endl << Acopy << endl;

    CORE_ASSERT_TRUE(Acopy.notTooFar(&Atrans, 1e-7), "Invalid transposition");


    /* Test arithmetic operations */
    Matrix Arg1(2, 2);
    Arg1.fillWithArgs(-5.0, 4.0, 3.0, 2.5);
    Matrix Arg2(2, 2);
    Arg2.fillWithArgs(1.0, -2.0, 0.0, -1.5);

    Matrix SumResult(2,2);
    SumResult.fillWithArgs(-4.0, 2.0, 3.0, 1.0);
    CORE_ASSERT_TRUE(addMatrix(Arg1, Arg2).notTooFar(&SumResult, 1e-7), "Invalid addition");
    CORE_ASSERT_TRUE((Arg1 + Arg2).notTooFar(&SumResult, 1e-7), "Invalid addition");

    Matrix SubResult(2,2);
    SubResult.fillWithArgs(-6.0, 6.0, 3.0, 4.0);
    CORE_ASSERT_TRUE((Arg1 - Arg2).notTooFar(&SubResult, 1e-7), "Invalid subtracion");

    /* Scalar multplication and division */

    Matrix ScalarMultResult(2,2);
    ScalarMultResult.fillWithArgs(-10.0, 8.0, 6.0, 5.0);
    Matrix resMul = Arg1 * 2.0;
    cout << "Multiplication real result:"   << endl << resMul << endl;
    cout << "Multiplication exp  result:"   << endl << ScalarMultResult << endl;

    CORE_ASSERT_TRUE(resMul.notTooFar(&ScalarMultResult, 1e-7), "Invalid scalar Multiplication");

    /* In place multplication and division */
    Matrix ScalarDivResult(2,2);
    ScalarDivResult.fillWithArgs(-2.5, 2.0, 1.5, 1.25);

    Matrix Arg1ToDiv(Arg1);
    Matrix Arg1ToMul(Arg1);

    Arg1ToDiv /= 2.0;
    Arg1ToMul /= 0.5;

    cout << "Division result:"   << endl << Arg1ToDiv << endl;
    cout << "Second div result:" << endl << Arg1ToMul << endl;
    cout << std::flush;
    fflush(stdout);

    CORE_ASSERT_TRUE(Arg1ToDiv.notTooFar(&ScalarDivResult, 1e-7), "Invalid scalar Division1");
    CORE_ASSERT_TRUE(Arg1ToMul.notTooFar(&ScalarMultResult, 1e-7), "Invalid scalar Division2");
}

TEST(MatrixTest, testVectorMatrixConversions)
{
    Matrix44 input(Matrix33::RotationY(0.1), Vector3dd(6,7,8));
    Vector3dd pos(45.0,45.0,45.0);
    Vector2dd shift(4.0,5.0);

 //   Vector2dd out0 = input * pos - shift;
 //   Vector2dd out1 = ((Vector2dd)(input * pos)) - shift;

    Vector3dd product = Vector3dd(1.0,2.0,3.0);
    Vector2dd out2 = (product.project() - shift);
    Vector2dd out3 = product.project() - shift;

    Vector2dd t1   = product.project();
    Vector2dd out4 = t1 - shift;

//    cout << out0  << endl;
//    cout << out1 << endl;
    cout << out2 << endl;
    cout << out3 << endl;
    cout << out4 << endl;
}

/**
 *   We need matrix of 10x10 to trigger vector path
 *
 *
 **/
TEST(MatrixTest, testMatrixProps)
{
    double dataAnan[] = {NAN, 1, 0, 2};
    Matrix Anan(2, 2, dataAnan);


    double dataAinf[] = {5, 1, INFINITY, 2};
    Matrix Ainf(2, 2, dataAinf);

    double dataAok[] = {5, 1, -57245, 2};
    Matrix Aok(2, 2, dataAok);

    ASSERT_TRUE(!Ainf.isFinite());
    ASSERT_TRUE(!Anan.isFinite());
    ASSERT_TRUE( Aok .isFinite());
}

TEST(MatrixTest, test10MatrixMultiply)
{
     Matrix A(10,10);
     auto touch = [](int i, int j, double &el) -> void { el = ((i+1) * (j + 1)) + ((j + 1) / 5.0); };
     A.touchOperationElementwize(touch);

    // cout << A << endl;

     double data[] = {
       554.4, 1016.4,  1478.4, 1940.4, 2402.4, 2864.4, 3326.4,  3788.4,  4250.4,  4712.4,
      1016.4, 1863.4,  2710.4, 3557.4, 4404.4, 5251.4, 6098.4,  6945.4,  7792.4,  8639.4,
      1478.4, 2710.4,  3942.4, 5174.4, 6406.4, 7638.4, 8870.4, 10102.4, 11334.4, 12566.4,
      1940.4, 3557.4,  5174.4, 6791.4, 8408.4,10025.4,11642.4, 13259.4, 14876.4, 16493.4,
      2402.4, 4404.4,  6406.4, 8408.4,10410.4,12412.4,14414.4, 16416.4, 18418.4, 20420.4,
      2864.4, 5251.4,  7638.4,10025.4,12412.4,14799.4,17186.4, 19573.4, 21960.4, 24347.4,
      3326.4, 6098.4,  8870.4,11642.4,14414.4,17186.4,19958.4, 22730.4, 25502.4, 28274.4,
      3788.4, 6945.4, 10102.4,13259.4,16416.4,19573.4,22730.4, 25887.4, 29044.4, 32201.4,
      4250.4, 7792.4, 11334.4,14876.4,18418.4,21960.4,25502.4, 29044.4, 32586.4, 36128.4,
      4712.4, 8639.4, 12566.4,16493.4,20420.4,24347.4,28274.4, 32201.4, 36128.4, 40055.4
     };

     Matrix result(10,10, data);
     Matrix AT = A.t();
     Matrix AAT = A * AT;

     ASSERT_TRUE(AAT. notTooFar(&result, 1e-8, true));

     Matrix AATHpv = Matrix::multiplyHomebrew(A, AT, true, true);
     cout << "AATHpv" << endl << AATHpv << endl;
     cout << "result" << endl << result << endl;
     ASSERT_TRUE(AATHpv. notTooFar(&result, 1e-8, true));

     Matrix AATHp = Matrix::multiplyHomebrew(A, AT, true, false);
     ASSERT_TRUE(AATHp. notTooFar(&result, 1e-8, true));

     Matrix AATHv = Matrix::multiplyHomebrew(A, AT, false, true);
     ASSERT_TRUE(AATHv. notTooFar(&result, 1e-8, true));

     Matrix AATH  = Matrix::multiplyHomebrew(A, AT, false, false);
     ASSERT_TRUE(AATH. notTooFar(&result, 1e-8, true));

}


TEST(SparseMatrix, FromDense)
{
    corecvs::Matrix m(3, 3);
    m.a(0, 0) = 1.0; m.a(0, 1) = 0.0; m.a(0, 2) = 1.0;
    m.a(1, 0) = 0.0; m.a(1, 1) = 4.0; m.a(1, 2) = 0.0;
    m.a(2, 0) =-2.0; m.a(2, 1) = 0.1; m.a(2, 2) = 0.0;

    corecvs::SparseMatrix sm(m), sm1(m, 1.0);

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            ASSERT_TRUE(m.a(i, j) == sm.a(i, j));
            ASSERT_TRUE(m.a(i, j) == sm1.a(i, j) || std::abs(m.a(i, j)) <= 1.0);
        }
    }
}

TEST(SparseMatrix, FromMap)
{
    std::map<std::pair<int, int>, double> matrix =
    {
        std::make_pair(std::make_pair(0, 0), 1.0),
        std::make_pair(std::make_pair(0, 1), 1.0),
        std::make_pair(std::make_pair(0, 99), 1.0)
    };
    corecvs::SparseMatrix sm(100, 100, matrix);

    int nonZero = 0;
    for (int i = 0; i < 100; ++i)
    {
        for (int j = 0; j < 100; ++j)
        {
            if (sm.a(i, j) != 0.0)
            {
                nonZero++;
            }
        }
    }
    ASSERT_TRUE(nonZero == 3);
    ASSERT_EQ(1.0, sm.a(0, 0));
    ASSERT_EQ(1.0, sm.a(0, 1));
    ASSERT_EQ(1.0, sm.a(0, 99));
}

TEST(SparseMatrix, ToFromDense)
{
    corecvs::Matrix m(10, 10);

    int id = 0;
    for (int i = 0; i < 10; ++i)
    {
        for (int j = 0; j < 10; ++j)
        {
            if ((i + j) % 7 == 3 || (10 + i - j) % 5 == 2)
            {
                m.a(i, j) = id++;
            }
        }
    }

    corecvs::SparseMatrix sm(m);
    corecvs::Matrix m2 = (Matrix)sm;

    ASSERT_EQ(0.0, (m2 - m).frobeniusNorm());
}

corecvs::SparseMatrix randomSparse(int h = 100, int w = 100, double sparsity = 0.1)
{
    static std::mt19937 rng(1337);
    std::uniform_real_distribution<double> runif(0, 1), runif_el(-1e0, 1e0);
    std::map<std::pair<int, int>, double> map;
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            if (runif(rng) < sparsity)
            {
                map[std::make_pair(i, j)] = runif_el(rng);
            }
        }
    }
    return corecvs::SparseMatrix(h, w, map);
}

TEST(SparseMatrix, UnaryMinus)
{
    auto sm = -randomSparse();
    auto dm = -(corecvs::Matrix)sm;
    ASSERT_EQ(0.0, ((corecvs::Matrix)sm + dm).frobeniusNorm());
}

TEST(SparseMatrix, MulDouble)
{
    auto sm = randomSparse();
    auto smd= sm * 0.123;
    auto dsm= 0.123 * sm;
    auto sm2 = smd / 0.123;
    auto dm = (corecvs::Matrix)sm;

    ASSERT_EQ(dm * 0.123, (corecvs::Matrix)smd);
    ASSERT_EQ(0.123 * dm, (corecvs::Matrix)dsm);
    ASSERT_NEAR((dm - (corecvs::Matrix)sm2).frobeniusNorm(), 0.0, 1e-9);
}

TEST(SparseMatrix, PlusSparse)
{
    for (int i = 0; i < 100; ++i)
    {
        auto sm1 = randomSparse();
        auto sm2 = randomSparse();
        auto diff = sm1 + sm2;
        auto diff2= (corecvs::Matrix)sm1 + (corecvs::Matrix)sm2;

        ASSERT_NEAR(((corecvs::Matrix)diff - diff2).frobeniusNorm(), 0.0, 1e-9);
    }
}

TEST(SparseMatrix, MinusSparse)
{
    for (int i = 0; i < 100; ++i)
    {
        auto sm1 = randomSparse();
        auto sm2 = randomSparse();
        auto diff = sm1 - sm2;
        auto diff2= (corecvs::Matrix)sm1 - (corecvs::Matrix)sm2;

        ASSERT_NEAR(((corecvs::Matrix)diff - diff2).frobeniusNorm(), 0.0, 1e-9);
    }
}

TEST(SparseMatrix, TransposeSparse)
{
    for (int i = 0; i < 100; ++i)
    {
        auto sm = randomSparse();
        auto dense = (corecvs::Matrix)sm;
        auto denseT = dense.t();

        ASSERT_EQ(0.0, ((corecvs::Matrix)sm.t() - denseT).frobeniusNorm());
    }

}

TEST(SparseMatrix, MulVectorLhs)
{
    corecvs::Vector rhs(100);
    for (int i = 0; i < 100; ++i)
        rhs[i] = i - 50;

    for (int i = 0; i < 100; ++i)
    {
        auto sm = randomSparse();
        auto dense = (corecvs::Matrix)sm;
        auto smv = rhs * sm;
        auto dmv = rhs * dense;

        ASSERT_NEAR(!(smv - dmv), 0.0, 1e-9);
    }
}

TEST(SparseMatrix, MulVectorRhs)
{
    corecvs::Vector rhs(100);
    for (int i = 0; i < 100; ++i)
        rhs[i] = i - 50;

    for (int i = 0; i < 100; ++i)
    {
        auto sm = randomSparse();
        auto dense = (corecvs::Matrix)sm;
        auto smv = sm * rhs;
        auto dmv = dense * rhs;

        ASSERT_NEAR(!(smv - dmv), 0.0, 1e-9);
    }
}

TEST(SparseMatrix, MulMM)
{
    for (int i = 0; i < 100; ++i)
    {
        auto l = randomSparse();
        auto r = randomSparse();
        auto dl= (corecvs::Matrix)l;
        auto dr= (corecvs::Matrix)r;

        auto ps = l * r;
        auto pd = dl*dr;

        ASSERT_NEAR(((corecvs::Matrix)ps - pd).frobeniusNorm(), 0.0, 1e-9);
    }
}

TEST(SparseMatrix, TT)
{
    std::vector<double> values = {1.0, 2.0, 3.0, 4.0, 5.0};
    std::vector<int> columns = {1, 2, 1, 2, 0};
    std::vector<int> rowPointers = {0, 1, 2, 4, 5};
    SparseMatrix sm(4, 3, values, columns, rowPointers);
    std::cout << sm << std::endl << sm.t() << std::endl;
}

TEST(SparseMatrix, MulTimer)
{
    auto sl= randomSparse(4000,4000, 0.001);
    auto dl= (corecvs::Matrix)sl;
    auto sr= randomSparse(4000,4000, 0.001);
    auto dr= (corecvs::Matrix)sr;

    auto startSparse = std::chrono::high_resolution_clock::now();
    auto s = sl * sr;
    auto endSparse = std::chrono::high_resolution_clock::now();

    auto startDense = std::chrono::high_resolution_clock::now();
    auto d = dl * dr;
    auto endDense = std::chrono::high_resolution_clock::now();

#ifdef WITH_MKL
    auto startMKL = std::chrono::high_resolution_clock::now();
    auto mkll = (sparse_matrix_t)sl;
    auto mklr = (sparse_matrix_t)sr;
    sparse_matrix_t res;
    mkl_sparse_spmm(SPARSE_OPERATION_NON_TRANSPOSE, mkll, mklr, &res);
    auto endMKL = std::chrono::high_resolution_clock::now();
    mkl_sparse_destroy(mkll);
    mkl_sparse_destroy(mklr);
    mkl_sparse_destroy(res);
#endif

    double timeSparse = (endSparse - startSparse).count();
    double timeDense  = (endDense  - startDense ).count();
#ifdef WITH_MKL
    double timeMKL = (endMKL - startMKL).count();
#endif

    std::cout << "Sparse: " << timeSparse << " Dense: " << timeDense <<
#ifdef WITH_MKL
        "MKL: " << timeMKL <<
#endif
        std::endl;
    ASSERT_NEAR(((corecvs::Matrix)s - d).frobeniusNorm(), 0.0, 1e-9);
    ASSERT_LE(timeSparse, timeDense);
}

TEST(SparseMatrix, multest)
{
    std::vector<int> IA = {0, 2, 4, 6}, JA = {1, 2, 2, 4, 0, 3};
    std::vector<double> A = {1, 2, 3, 4, 5, 6};
    std::vector<int> IB = {0, 1, 3, 5, 7, 10}, JB = {3, 1, 3, 0, 2, 0, 1, 0, 1, 2};
    std::vector<double> B = {7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    SparseMatrix a(3, 5, A, JA, IA), b(5, 4, B, JB, IB);
    auto c = a * b;
    std::cout << ((Matrix)a) * ((Matrix)b) << std::endl << std::endl;
    std::cout << c << std::endl << ((Matrix)c - ((Matrix)a) * ((Matrix)b)) << std::endl;
}


#ifdef WITH_MKL
// XXX: testing mkl sparse casting
TEST(SparseMatrix, MKLCasts)
{
    auto sm = randomSparse(1000, 100, 0.01);
    auto mkl_sm = (sparse_matrix_t)sm;
    auto sm2= (corecvs::SparseMatrix)mkl_sm;

    ASSERT_NEAR(((corecvs::Matrix)sm - (corecvs::Matrix)sm2).frobeniusNorm(), 0.0, 1e-9);
    mkl_sparse_destroy(mkl_sm);
}
#endif

SparseMatrix generateTri(int n = 1000, double fill = 1.0)
{
    Matrix m(n, n);
    for (int i = 0; i < n; ++i)
        m.a(i, i) = fill;
    for (int i = 0; i < n; ++i)
    {
        if (i + 3 < n)
        {
        m.a(i, i + 3) = fill;
        m.a(i + 3, i) = fill;
        }
    }
    return SparseMatrix(m);
}

TEST(SparseMatrix, LinSolve)
{
    auto sm = generateTri(10000, 0.1);
    corecvs::Vector v(10000);
    for (int i = 0; i < 10000; ++i)
        v[i] = (i - 5000.0) / 5000.0;
    auto vm = sm * v;

#if 0
    auto res = Matrix::linSolve((Matrix)sm, vm);
#else
    corecvs::Vector res;
    CORE_ASSERT_TRUE_S(sm.linSolve(vm, res));
#endif
    ASSERT_NEAR(!(res - v), 0.0, 1e-6);
}

TEST(SparseMatrix, ATA)
{
    auto sm = randomSparse(1000, 100, 0.01);
    auto res = sm.ata();
    auto dres= ((Matrix)sm).t() * ((Matrix)sm);
    ASSERT_NEAR(((Matrix)res - dres).frobeniusNorm(), 0.0, 1e-9);
}

TEST(MatrixTest, ATA)
{
    auto m = (Matrix)randomSparse(1000, 100, 0.01);
    auto res = m.ata();
    auto res2 = m.t() * m;
    ASSERT_EQ(res.h, res.w);
    ASSERT_EQ(res.h, m.w);
    ASSERT_NEAR(((Matrix)res - res2).frobeniusNorm(), 0.0, 1e-9);
}

Vector randomVector(int N)
{
    Vector v(N);
    std::mt19937 rng;
    std::uniform_real_distribution<double> runif(-1.0, 1.0);
    for (int i = 0; i < N; ++i)
        v[i] = runif(rng);
    return v;
}

Matrix randomMatrix(int N, int M)
{
    Matrix m(N, M);
    std::mt19937 rng;
    std::uniform_real_distribution<double> runif(-1.0, 1.0);
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            m.a(i, j) = runif(rng);
    return m;
}

TEST(MatrixTest, LinSolve)
{
    auto a = randomMatrix(100, 100);
    auto v = randomVector(100);
    auto vm = a * v;
    corecvs::Vector res;
    CORE_ASSERT_TRUE_S(a.linSolve(vm, res));
    ASSERT_NEAR(!(v - res), 0.0, 1e-6);
}


TEST(MatrixTest, LinSolveSymmetric)
{
    auto a = randomMatrix(10000, 1000).ata();
    auto v = randomVector(1000);
    auto vm = a * v;
    corecvs::Vector res;
    CORE_ASSERT_TRUE_S(a.linSolve(vm, res, true, true));
    ASSERT_NEAR(!(v - res), 0.0, 1e-6);
}

TEST(SparseMatrix, LinSolveSymmetric)
{
    Matrix M(4,4);
    M.a(0, 0) = 1.0; M.a(0, 1) = 0.0; M.a(0, 2) = 0.0; M.a(0, 3) = 0.0;
    M.a(1, 0) = 0.0; M.a(1, 1) = 1.0; M.a(1, 2) = 0.0; M.a(1, 3) = 0.0;
    M.a(2, 0) = 0.0; M.a(2, 1) = 0.0; M.a(2, 2) = 1.0; M.a(2, 3) = 0.0;
    M.a(3, 0) = 0.0; M.a(3, 1) = 0.0; M.a(3, 2) = 0.0; M.a(3, 3) = 1.0;
    auto v = randomVector(4);
    auto a = SparseMatrix(M);
    auto vm = a * v;
    corecvs::Vector res;
    CORE_ASSERT_TRUE_S(a.linSolve(vm, res, true));
    ASSERT_NEAR(!(v - res), 0.0, 1e-6);
}

TEST(SparseMatrix, LinSolveSymmetricNonPos)
{
    Matrix M(4,4);
    M.a(0, 0) =-1.0; M.a(0, 1) = 0.0; M.a(0, 2) = 0.0; M.a(0, 3) = 0.0;
    M.a(1, 0) = 0.0; M.a(1, 1) = 1.0; M.a(1, 2) = 2.0; M.a(1, 3) = 0.0;
    M.a(2, 0) = 0.0; M.a(2, 1) = 2.0; M.a(2, 2) = 1.0; M.a(2, 3) = 0.0;
    M.a(3, 0) = 0.0; M.a(3, 1) = 0.0; M.a(3, 2) = 0.0; M.a(3, 3) = 1.0;
    auto a = (SparseMatrix)M;//
//    auto a = randomSparse(100000, 1000, 0.005).ata();
    std::cout << "ATA" << std::endl;
    auto v = randomVector(4);
    auto vm = a * v;
    std::cout << a << std::endl;
    std::cout << vm << std::endl;
    corecvs::Vector res;
    CORE_ASSERT_TRUE_S(a.linSolve(vm, res, true));
    ASSERT_NEAR(!(v - res), 0.0, 1e-6);

}

TEST(SparseMatrix, ReferenceA)
{
    std::vector<int> zeros(11);
    SparseMatrix sm(10, 10, {}, {}, zeros);
    ASSERT_EQ(sm.a(5, 5), 0.0);
    sm.a(5, 5) = 10.0;
    ASSERT_EQ(sm.a(5, 5), 10.0);
}

TEST(Matrix, SchurComplement)
{
    double foo[] =
    {
        4.0, 5.0, 6.0, 7.0, 0.0,
        8.0, 9.0, 9.0, 1.0, 2.0,
        3.0, 7.0, 0.0, 1.0, 0.0,
        2.0, 5.0, 1.0, 0.0, 0.0,
        9.0, 3.0, 0.0, 0.0, 5.0
    };
    double boo[] = { 1.0, 2.0, 3.0, 4.0, 5.0 };
    corecvs::Matrix M(5, 5, foo);
    std::vector<int> blocks = {2, 4, 5};
    corecvs::Vector x(5, boo);

    auto rhs = M * x;
    corecvs::Vector xx(5);
    M.linSolveSchurComplement(rhs, blocks, xx, false, false);


    std::cout << M << std::endl << std::endl << x << std::endl << xx << std::endl << rhs << std::endl << M * xx << std::endl;

    ASSERT_LE(!(x - xx), 1e-6);
}

TEST(SparseMatrix, Submatrix)
{
    double foo[] =
    {
        4.0, 5.0, 6.0, 7.0, 0.0,
        8.0, 9.0, 9.0, 1.0, 2.0,
        3.0, 7.0, 0.0, 1.0, 0.0,
        2.0, 5.0, 1.0, 0.0, 0.0,
        9.0, 3.0, 0.0, 0.0, 5.0
    };
    //double boo[] = { 1.0, 2.0, 3.0, 4.0, 5.0 };
    corecvs::Matrix M(5, 5, foo);
    std::vector<int> blocks = {2, 4, 5};
    corecvs::SparseMatrix sm(M);
    corecvs::SparseMatrix ssm(M, 2, 1, 5, 4);
    std::cout << M << std::endl << std::endl << sm << std::endl << std::endl << ssm << std::endl;
    ASSERT_EQ(ssm.h, 3);
    ASSERT_EQ(ssm.w, 3);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
        {
            ASSERT_EQ(sm.a(1 + i, 2 + j), ssm.a(i, j));
            ASSERT_EQ( M.a(1 + i, 2 + j), ssm.a(i, j));
        }

}

#if !defined(_WIN32) || defined(_WIN64)

TEST(SparseMatrix, SchurComplement)
{
    double foo[] =
    {
        4.0, 5.0, 6.0, 7.0, 0.0,
        8.0, 9.0, 9.0, 1.0, 2.0,
        3.0, 7.0, 0.0, 1.0, 0.0,
        2.0, 5.0, 1.0, 0.0, 0.0,
        9.0, 3.0, 0.0, 0.0, 5.0
    };
    double boo[] = { 1.0, 2.0, 3.0, 4.0, 5.0 };
    corecvs::Matrix MM(5, 5, foo);
    corecvs::SparseMatrix M(MM);
    std::vector<int> blocks = {2, 4, 5};
    corecvs::Vector x(5, boo);

    auto rhs = M * x;
    corecvs::Vector xx(5);
    M.linSolveSchurComplement(rhs, blocks, xx, false, false);

    std::cout << M << std::endl << std::endl << x << std::endl << xx << std::endl << rhs << std::endl << M * xx << std::endl;

    ASSERT_LE(!(x - xx), 1e-6);
}

#endif

TEST(SparseMatrix, denseRows)
{
    double foo[] =
    {
        0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0
    },
           bar[] =
    {
        0.0, 0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 0.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 1.0
    };
    corecvs::Matrix Foo(4, 7, foo), Bar(4, 5, bar);
    corecvs::SparseMatrix Boo(Foo);

    ASSERT_EQ(Boo.nnz(), 6);
    std::vector<int> colIdx;
    auto D = Boo.denseRows(0, 0, 7, 4, colIdx);
    ASSERT_EQ(D.w, 5);
    ASSERT_EQ(D.h, 4);
    ASSERT_EQ((Bar-D).frobeniusNorm(), 0.0);
}

TEST(SparseMatrix, denseCols)
{
    double foo[] =
    {
        0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0
    },
           bar[] =
    {
        0.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        1.0, 0.0, 0.0, 0.0
    };
    corecvs::Matrix Foo(4, 7, foo), Bar(3, 4, bar);
    corecvs::SparseMatrix Boo(Foo);

    ASSERT_EQ(Boo.nnz(), 6);
    std::vector<int> rowIdx;
    auto D = Boo.denseCols(0, 0, 4, 4, rowIdx);
    ASSERT_EQ(D.w, 4);
    ASSERT_EQ(D.h, 3);
    ASSERT_EQ((Bar-D).frobeniusNorm(), 0.0);
}

TEST(SparseMatrix, denseCols2)
{
    double foo[] =
    {
        0.0,
        1.0
    };
    corecvs::SparseMatrix F(corecvs::Matrix(2, 1, foo));
    std::vector<int> rowIdx;
    auto D = F.denseCols(0, 0, 1, 2, rowIdx);
    ASSERT_EQ(D.w, 1);
    ASSERT_EQ(D.h, 1);
    ASSERT_EQ(rowIdx.size(), (size_t)1);
    ASSERT_EQ(rowIdx[0], 1);
}

TEST(SparseMatrix, denseRows2)
{
    double foo[] =
    {
        0.0,
        1.0
    };
    corecvs::SparseMatrix F(corecvs::Matrix(1, 2, foo));
    std::vector<int> rowIdx;
    auto D = F.denseRows(0, 0, 2, 1, rowIdx);
    ASSERT_EQ(D.w, 1);
    ASSERT_EQ(D.h, 1);
    ASSERT_EQ(rowIdx.size(), (size_t)1);
    ASSERT_EQ(rowIdx[0], 1);
}


TEST(SparseMatrix, denseCols3)
{
    double foo[] =
    {
        1.0, 0.0, 3.0,
        2.0, 0.0, 4.0
    };
    corecvs::SparseMatrix F(corecvs::Matrix(2, 3, foo));
    std::vector<int> rowIdx;
    auto D = F.denseCols(1, 0, 2, 2, rowIdx);
    ASSERT_EQ(D.w, 1);
    ASSERT_EQ(D.h, 0);
    ASSERT_EQ(rowIdx.size(), (size_t)0);
}

TEST(SparseMatrix, denseCols4)
{
    double foo[] =
    {
        1.0, 2.0, 0.0,
        3.0, 4.0, 5.0
    };
    corecvs::SparseMatrix F(corecvs::Matrix(2, 3, foo));
    std::vector<int> rowIdx;
    auto D = F.denseCols(2, 0, 3, 2, rowIdx);
    ASSERT_EQ(D.w, 1);
    ASSERT_EQ(D.h, 1);
    ASSERT_EQ(rowIdx.size(), (size_t)1);
    ASSERT_EQ(rowIdx[0], 1);
    ASSERT_EQ(D.a(0, 0), 5.0);
}


#if 0
TEST(EM, EM)
{
    std::mt19937 rng(SEED);
    std::uniform_real_distribution<double> runif(0, 1);
    std::normal_distribution<double> rnorm;
    const int NDIM = 3;

    double CC1[NDIM * NDIM] = {
       -0.4, 0.9, 1.2,
        0.1, 0.2,-0.1,
        0.5,-0.5, 0.5
    };
    double CC2[NDIM * NDIM] = {
        1.2, -0.8, 0.3,
        1.0,  0.0,-1.2,
        0.4, -0.1, 0.1
    };

    corecvs::Matrix M1(NDIM, NDIM, CC1), M2(NDIM, NDIM, CC2);
    auto cov1 = M1 * M1.t(), cov2 = M2 * M2.t();

    corecvs::Vector m1(NDIM), m2(NDIM);
    m1[0] = 10.0, m1[1] = 10.0, m1[2] = 10.0;
    m2[0] =  0.0, m2[1] = -1.0, m2[2] = -2.0;

    //double p1 = 0.25, p2 = 0.75;

    const int NCASES = 10000;
    corecvs::Matrix A(NCASES, NDIM);
    corecvs::Matrix A1(NCASES, NDIM);
    corecvs::Matrix A2(NCASES, NDIM);

    int n1 = 0, n2 = 0;
    corecvs::Vector nm1(NDIM), nm2(NDIM);
    for (int i = 0; i < NDIM; ++i)
        nm1[i] = nm2[i] = 0;

    for (int i = 0; i < NCASES; ++i)
    {
        corecvs::Vector vv(NDIM);
        int dir = 0;
        for (int j = 0; j < NDIM; ++j)
            vv[j] = rnorm(rng);
        if (runif(rng) < 0.25)
            vv = (M1 * vv) + m1;
        else
        {
            vv = (M2 * vv) + m2;
            dir = 1;
        }
        for (int j = 0; j < NDIM; ++j)
        {
            A.a(i, j) = vv[j];
            if (dir == 0)
                A1.a(n1, j) = vv[j];
            else
                A2.a(n2, j) = vv[j];
        }
        (dir == 0 ? n1 : n2)++;
        (dir == 0 ? nm1 : nm2) += vv;
    }
    A1.h = n1;
    A2.h = n2;
    nm1 /= n1;
    nm2 /= n2;

    for (int i = 0; i < n1; ++i)
        for (int j = 0; j < NDIM; ++j)
            A1.a(i, j) -= nm1[j];
    for (int i = 0; i < n2; ++i)
        for (int j = 0; j < NDIM; ++j)
            A2.a(i, j) -= nm2[j];



    EM em(A, 2, true);
    std::cout << "Expected covs: " << cov1 << std::endl << cov2 << std::endl << std::endl;
    std::cout << "EMP1: " << A1.ata() / n1 << std::endl << A2.ata() / n2 << std::endl << double(n1) / (n1 + n2) << ":" << double(n2)/(n1 + n2) << std::endl;
    std::cout << "MNS: " << nm1 << " " << nm2 << std::endl;
    std::cout << em << std::endl;
    double prob1 = double(n1) / (n1 + n2), prob2 = double(n2) / (n1 + n2);
    Matrix cv1 = A1.ata() * (1.0 / n1), cv2 = A2.ata() * (1.0 / n2);
    Vector mean1 = nm1, mean2 = nm2;
    if ((prob1 < prob2) ^ (em.prior[0] < em.prior[1]))
    {
        std::swap(cv1, cv2);
        std::swap(prob1, prob2);
        std::swap(mean1, mean2);
    }
    ASSERT_NEAR(em.prior[0], prob1, 1e-3);
    ASSERT_NEAR(em.prior[1], prob2, 1e-3);
    ASSERT_NEAR((cv1 - em.covariances[0]).frobeniusNorm(), 0.0, 1e-3);
    ASSERT_NEAR((cv2 - em.covariances[1]).frobeniusNorm(), 0.0, 1e-3);
    ASSERT_NEAR(!(mean1 - em.means[0]), 0.0, 1e-3);
    ASSERT_NEAR(!(mean2 - em.means[1]), 0.0, 1e-3);
}
#endif


TEST(Matrix, invPosdefSqrt)
{
    double a[]= {
        9, 0, 0,
        0, 8, 0,
        0, 0, 7
    };
    double b[]= {
        std::cos(0.123), std::sin(0.123), 0.0,
       -std::sin(0.123), std::cos(0.123), 0.0,
                    0.0,             0.0, 1.0
    };
    double c[]= {
       1.0,              0.0,             0.0,
       0.0,  std::cos(0.456), std::sin(0.456),
       0.0, -std::sin(0.456), std::cos(0.456)
    };
    double d[]= {
        std::cos(0.789),  0.0,std::sin(0.789),
                    0.0,  1.0,            0.0,
       -std::sin(0.789),  0.0,std::cos(0.789)
    };
    corecvs::Matrix A(3, 3, a), B(3, 3, b), C(3, 3, c), D(3, 3, d), W;
    auto CC = D * C * B * A * B.t() * C.t() * D.t();
    std::cout << (W = CC.invPosdefSqrt().t()*CC*CC.invPosdefSqrt()) << std::endl << std::endl << B * C * D << std::endl;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            ASSERT_NEAR(W.a(i, j), i == j ? 1.0 : 0.0, 6e-16);
}

TEST(Matrix, DISABLED_hugeMatrix)
{
    int sx = 65536, sy = 32768;

    ASSERT_TRUE((sx * sy) < 0);
#if 0
    Matrix A(sy + 1, sx + 1, 0.);                         // it requires 16GB
#else
    AbstractBuffer<int, int32_t> A(sy + 1, sx + 1, 0);    // it requires 8GB
#endif
    for (int i = 0; i <= sy; i += sy/2)
        for (int j = 0; j <= sx; j += sx/2)
            A.element(i, j) = 1.;

    ASSERT_NEAR(A.element(         0,              0), 1.0, 1e-16);
    ASSERT_NEAR(A.element(        10,             10), 0.0, 1e-16);
    ASSERT_NEAR(A.element(sy / 2    , sx / 2        ), 1.0, 1e-16);
    ASSERT_NEAR(A.element(sy / 2 * 2, sx / 2 * 2    ), 1.0, 1e-16);
    ASSERT_NEAR(A.element(sy / 2 * 2, sx / 2 * 2 - 1), 0.0, 1e-16);
}

TEST(Matrix, svd33Matrix)
{
    Matrix33 input(
        0,  0,  0,
        0,  0,  1,
        0, -1,  0 );

    Vector3dd out1(1.0);
    Matrix33  out2(1.0);

    Matrix::svd(&input, &out1, &out2);

    cout << input << endl;
    cout << out1 << endl;
    cout << out2 << endl;

}
