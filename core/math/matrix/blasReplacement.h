#ifndef BLASREPLACEMENT_H
#define BLASREPLACEMENT_H

#include "core/utils/global.h"
#include "core/math/matrix/matrix.h"
#include "core/math/matrix/matrix33.h"

#include "core/tbbwrapper/tbbWrapper.h"
#include "core/math/sse/sseWrapper.h"

namespace corecvs {

template<int vectorize = true>
struct ParallelMM
{
    void operator() (const corecvs::BlockedRange<int> &r) const
    {
        const Matrix& A = *pA;
        const Matrix& B = *pB;
        Matrix& result = *pResult;

        for (int row = r.begin(); row < r.end(); row++)
        {
            int column = 0;
#if WITH_SSE
            if (vectorize)
            {
                const int STEP = DoublexN::SIZE;

                for (; column + STEP - 1 < result.w; column += STEP)
                {
                    const double *ALine = &A.a(row, 0);
                    const double *BCol  = &B.a(0, column);

                    DoublexN sum = DoublexN::Zero();
                    for (int runner = 0; runner < A.w; runner++)
                    {
                        DoublexN bc = DoublexN::Broadcast(ALine);
                        DoublexN rw(BCol);
                        sum = multiplyAdd(bc, rw, sum);

                        ALine++;
                        BCol += B.stride;
                    }

                    sum.save(&(result.a(row, column)));

                }
            }
#endif
            for (; column < result.w; ++column)
            {
                double sum = 0;
                for (int runner = 0; runner < A.w; runner++)
                {
                    sum += A.a(row, runner) * B.a(runner, column);
                }
                result.a(row, column) = sum;
            }
        }
    }
    ParallelMM(const Matrix *pA, const Matrix *pB, Matrix *pResult) : pA(pA), pB(pB), pResult(pResult)
    {
    }
    const Matrix *pA;
    const Matrix *pB;
    Matrix *pResult;
};

template<int vectorize = true>
struct ParallelMM4
{
    static const int BLOCK = 4;

    void operator() (const corecvs::BlockedRange<int> &r) const
    {
        const Matrix& A = *pA;
        const Matrix& B = *pB;
        Matrix& result = *pResult;

        size_t Ad = A.stride;
        size_t Bd = B.stride;

        int row = r.begin();

        for (; (row + BLOCK <= r.end()) && vectorize; row += BLOCK)
        {
            int column = 0;
            for (; column + BLOCK <= result.w; column += BLOCK)
            {
                /*Ok. Here we have a 4x4 block to update*/
#ifndef WITH_AVX
                for (int dr = 0; dr < BLOCK; dr++)
                {
                    for (int dc = 0; dc < BLOCK; dc++)
                    {
                        result.a(row + dr, column + dc) = 0;
                    }
                }

                for (int runner = 0; runner < A.w; runner++)
                {
                    for (int dr = 0; dr < BLOCK; dr++)
                    {
                        for (int dc = 0; dc < BLOCK; dc++)
                        {
                            result.a(row + dr, column + dc) += A.a(row + dr, runner) * B.a(runner, column + dc);
                        }
                    }
                }
#else
                Doublex4 s0 = Doublex4::Zero();
                Doublex4 s1 = Doublex4::Zero();
                Doublex4 s2 = Doublex4::Zero();
                Doublex4 s3 = Doublex4::Zero();

                Doublex4 a0, a1, a2, a3, b;

                const double *As = &A.a(row, 0);
                const double *Bs = &B.a(0, column);

                for (int runner = 0; runner < A.w; runner++)
                {

                    b  = Doublex4(Bs);
                    a0 = Doublex4::Broadcast(As);
                    a1 = Doublex4::Broadcast(As + Ad);

                    s0 = multiplyAdd(a0, b, s0);
                    s1 = multiplyAdd(a1, b, s1);

                    a2 = Doublex4::Broadcast(As + Ad * 2);
                    a3 = Doublex4::Broadcast(As + Ad * 3);

                    s2 = multiplyAdd(a2, b, s2);
                    s3 = multiplyAdd(a3, b, s3);

                    As++;
                    Bs+=Bd;
                }

                s0.save(&result.a(row + 0, column));
                s1.save(&result.a(row + 1, column));
                s2.save(&result.a(row + 2, column));
                s3.save(&result.a(row + 3, column));
#endif
            }

            for (; column < result.w; column++)
            {
                for (int dr = 0; dr < BLOCK; dr++)
                {
                    double sum = 0;
                    for (int runner = 0; runner < A.w; runner++)
                    {
                        sum += A.a(row + dr, runner) * B.a(runner, column);
                    }
                    result.a(row + dr, column) = sum;
                }
            }
        }

        for (; row < r.end(); row++)
        {
            int column = 0;
            for (; column < result.w; column++)
            {
                double sum = 0;
                for (int runner = 0; runner < A.w; runner++)
                {
                    sum += A.a(row, runner) * B.a(runner, column);
                }
                result.a(row, column) = sum;
            }
        }
    }
    ParallelMM4(const Matrix *pA, const Matrix *pB, Matrix *pResult) : pA(pA), pB(pB), pResult(pResult)
    {
    }
    const Matrix *pA;
    const Matrix *pB;
    Matrix *pResult;
};


template<int vectorize = true>
struct ParallelMM8
{
    static constexpr int BLOCK = 8;

    void operator() (const corecvs::BlockedRange<int> &r) const
    {
        const Matrix& A = *pA;
        const Matrix& B = *pB;
        Matrix& result = *pResult;

        size_t Ad = A.stride;
        size_t Bd = B.stride;

        int row = r.begin();
        int rowCycleEnd = r.end() - BLOCK;
        int colCycleEnd = result.w - BLOCK;
#ifdef WITH_AVX
        if (vectorize)
        {
            for (; (row <= rowCycleEnd); row += BLOCK)
            {
                int column = 0;
                for (; column <= colCycleEnd; column += BLOCK)
                {
                    /*Ok. Here we have a 8x8 block to update*/
                    /*Doublex4() init data with zeros*/
                    Doublex4 s00 = Doublex4(); Doublex4 s01 = Doublex4();
                    Doublex4 s10 = Doublex4(); Doublex4 s11 = Doublex4();
                    Doublex4 s20 = Doublex4(); Doublex4 s21 = Doublex4();
                    Doublex4 s30 = Doublex4(); Doublex4 s31 = Doublex4();

                    Doublex4 a0, a1, b0, b1;

                    const double *As = &A.a(row, 0);
                    const double *Bs = &B.a(0, column);

                    for (int runner = result.w; runner != 0; runner--)
                    {

                        b0 = Doublex4(Bs);
                        b1 = Doublex4(Bs + 4);
	
                        Bs+=Bd;

                        const double *Of = As;

                        a0 = Doublex4::Broadcast(Of);  Of += Ad;
                        a1 = Doublex4::Broadcast(Of);  Of += Ad;

                        s00 = multiplyAdd(a0, b0, s00); s01 = multiplyAdd(a0, b1, s01);
                        s10 = multiplyAdd(a1, b0, s10); s11 = multiplyAdd(a1, b1, s11);

                        a0 = Doublex4::Broadcast(Of); Of += Ad;
                        a1 = Doublex4::Broadcast(Of); Of += Ad;

                        s20 = multiplyAdd(a0, b0, s20); s21 = multiplyAdd(a0, b1, s21);
                        s30 = multiplyAdd(a1, b0, s30); s31 = multiplyAdd(a1, b1, s31);

                        Of += 4 * Ad;
                        As++;

                    }

                    s00.save(&result.a(row + 0, column)); s01.save(&result.a(row + 0, column + 4));
                    s10.save(&result.a(row + 1, column)); s11.save(&result.a(row + 1, column + 4));
                    s20.save(&result.a(row + 2, column)); s21.save(&result.a(row + 2, column + 4));
                    s30.save(&result.a(row + 3, column)); s31.save(&result.a(row + 3, column + 4));

                    s00 = Doublex4::Zero(); s01 = Doublex4::Zero();
                    s10 = Doublex4::Zero(); s11 = Doublex4::Zero();
                    s20 = Doublex4::Zero(); s21 = Doublex4::Zero();
                    s30 = Doublex4::Zero(); s31 = Doublex4::Zero();

                    As = &A.a(row, 0);
                    Bs = &B.a(0, column);

                    for (int runner = result.w; runner != 0; runner--)
                    {

                        b0 = Doublex4(Bs);
                        b1 = Doublex4(Bs + 4);
	
                        Bs+=Bd;

                        const double *Of = As;

                        Of += 4 * Ad;

                        a0 = Doublex4::Broadcast(Of);  Of += Ad;
                        a1 = Doublex4::Broadcast(Of);  Of += Ad;

                        s00 = multiplyAdd(a0, b0, s00); s01 = multiplyAdd(a0, b1, s01);
                        s10 = multiplyAdd(a1, b0, s10); s11 = multiplyAdd(a1, b1, s11);

                        a0 = Doublex4::Broadcast(Of); Of += Ad;
                        a1 = Doublex4::Broadcast(Of); Of += Ad;

                        s20 = multiplyAdd(a0, b0, s20); s21 = multiplyAdd(a0, b1, s21);
                        s30 = multiplyAdd(a1, b0, s30); s31 = multiplyAdd(a1, b1, s31);

                        As++;

                    }

                    s00.save(&result.a(row + 4, column)); s01.save(&result.a(row + 4, column + 4));
                    s10.save(&result.a(row + 5, column)); s11.save(&result.a(row + 5, column + 4));
                    s20.save(&result.a(row + 6, column)); s21.save(&result.a(row + 6, column + 4));
                    s30.save(&result.a(row + 7, column)); s31.save(&result.a(row + 7, column + 4));
                }

                for (; column < result.w; column++)
                {
                    for (int dr = 0; dr < BLOCK; dr++)
                    {
                        double sum = 0;
                        for (int runner = A.w; runner != 0; runner--)
                        {
                            sum += A.a(row + dr, runner) * B.a(runner, column);
                        }
                        result.a(row + dr, column) = sum;
                    }
                }
            }
        }
#endif

        for (; row < r.end(); row++)
        {
            int column = 0;
            for (; column < result.w; column++)
            {
                double sum = 0;
                for (int runner = 0; runner < A.w; runner++)
                {
                    sum += A.a(row, runner) * B.a(runner, column);
                }
                result.a(row, column) = sum;
            }
        }
    }
    ParallelMM8(const Matrix *pA, const Matrix *pB, Matrix *pResult) : pA(pA), pB(pB), pResult(pResult)
    {
    }
    const Matrix *pA;
    const Matrix *pB;
    Matrix *pResult;
};


#if 0 // unfinished stuff
template<int vectorize = true>
struct ParallelMMT
{
    void operator() (const corecvs::BlockedRange<int> &r) const
    {
        const Matrix& A = *pA;
        const Matrix& B = *pB;
        Matrix& result = *pResult;

        for (int row = r.begin(); row < r.end(); row++)
        {
            int column = 0;
#if WITH_SSE
            if (vectorize)
            {
                const int STEP = DoublexN::SIZE;
                ALIGN_DATA(16) double scratch[STEP];

                for (; column + STEP - 1 < result.w; column += STEP)
                {
                    const double *ALine = &A.a(row, 0);
                    const double *BCol  = &B.a(0, column);

                    DoublexN sum = DoublexN::Zero();
                    for (int runner = 0; runner < A.w; runner++)
                    {
                        DoublexN bc = DoublexN::Broadcast(ALine);
                        DoublexN rw(BCol);
                        sum = multiplyAdd(bc, rw, sum);

                        ALine++;
                        BCol += B.stride;
                    }

                    sum.saveAligned(scratch);

                    for (int jj = 0; jj < STEP; ++jj)
                    {
                       result.a(row, column + jj) = scratch[jj];
                    }
                }
            }
#endif
            for (; column < result.w; column++)
            {
                double sum = 0;
                for (int runner = 0; runner < A.w; runner++)
                {
                    sum += A.a(row, runner) * B.a(column, runner);
                }
                result.a(row, column) = sum;
            }
        }
    }
    ParallelMMT(const Matrix *pA, const Matrix *pB, Matrix *pResult) : pA(pA), pB(pB), pResult(pResult)
    {
    }
    const Matrix *pA;
    const Matrix *pB;
    Matrix *pResult;
};
#endif

struct ParallelMV
{
    void operator() (const corecvs::BlockedRange<int> &r) const
    {
        auto& A = *pA;
        auto& B = *pB;
        auto& result = *pResult;
        int row;
        for (row = r.begin(); row < r.end(); ++row)
        {
            int column = 0;
            double sum = 0;
#ifdef WITH_SSE
            const int STEP = DoublexN::SIZE;
            ALIGN_DATA(16) double scratch[STEP];

            DoublexN sumV = DoublexN::Zero();

            for (column = 0; column + STEP - 1 < A.w; column += STEP)
            {
                DoublexN bc = DoublexN(&A.a(row, column));
                DoublexN rw = DoublexN(&B.at(column));
                sumV = multiplyAdd(bc, rw, sumV);
            }

            sumV.saveAligned(scratch);
            for (int i = 0; i < STEP; i++) {
                sum += scratch[i];
            }
#endif

            for (; column < A.w; column++)
            {
                sum += B.at(column) * A.a(row, column);
            }
            result.at(row) = sum;
        }
    }
    ParallelMV(const Matrix *pA, const Vector *pB, Vector *pResult) : pA(pA), pB(pB), pResult(pResult)
    {
    }
    const Matrix *pA;
    const Vector *pB;
    Vector *pResult;
};

struct ParallelMD
{
    void operator() (const corecvs::BlockedRange<int> &r) const
    {
        const Matrix& A = *pA;
        const DiagonalMatrix& B = *pB;
        Matrix& result = *pResult;
        int row;
        for (row = r.begin(); row < r.end(); ++row)
        {
            int column = 0;

#ifdef WITH_AVX
            for (column = 0; column + 3 < result.w; column += 4)
            {
                __m256d diag = _mm256_loadu_pd(&B.at(column));
                __m256d mat  = _mm256_loadu_pd(&A.a(row, column));
                __m256d res  = _mm256_mul_pd(diag, mat);
                _mm256_storeu_pd(&result.a(row, column), res);
            }
#endif
            for (; column < result.w; column++)
            {
                result.a(row, column) = A.a(row, column) * B.at(column);
            }
        }
    }

    ParallelMD(const Matrix *pA, const DiagonalMatrix *pB, Matrix *pResult) : pA(pA), pB(pB), pResult(pResult)
    {
    }
    const Matrix *pA;
    const DiagonalMatrix *pB;
    Matrix *pResult;
};


// TODO: it has a bug, see testMatrixOperations result when change the operator *(Matrix&, Matrix&) to use this!!!
Matrix Matrix::multiplyHomebrew(const Matrix &A, const Matrix &B, bool parallel, bool vectorize)
{
    CORE_ASSERT_TRUE(A.w == B.h, "Matrices have wrong sizes");
    Matrix result(A.h, B.w, false);
    if (vectorize) {
        parallelable_for (0, result.h, 8, ParallelMM8<true> (&A, &B, &result), parallel);
    } else {
        parallelable_for (0, result.h, 8, ParallelMM8<false>(&A, &B, &result), parallel);
    }
    return result;
}

Matrix Matrix::multiplyHomebrewMD(const Matrix &M, const DiagonalMatrix &D)
{
    CORE_ASSERT_TRUE(M.w == D.size(), "Matrix and DiagonalMatrix have wrong sizes");
    Matrix result(M.h, M.w);

    parallelable_for (0, result.h, 8, ParallelMD(&M, &D, &result), true);
    return result;
}


Vector Matrix::multiplyHomebrewMV(const Matrix &M, const Vector &V)
{
    CORE_ASSERT_TRUE(M.w == V.size(), "Matrix and Vector have wrong sizes");
    Vector result(M.h);
    parallelable_for (0, M.h, 8, ParallelMV(&M, &V, &result));
    return result;
}


} //namespace corecvs

#endif  //BLASREPLACEMENT_H
