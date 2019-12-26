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
    static const int BLOCK = 8;

    void operator() (const corecvs::BlockedRange<int> &r) const
    {
        const Matrix& A = *pA;
        const Matrix& B = *pB;
        Matrix& result = *pResult;

        size_t Ad = A.stride;
        size_t Bd = B.stride;

        int row = r.begin();

#ifdef WITH_AVX
        for (; (row + BLOCK <= r.end()) && vectorize; row += BLOCK)
        {
            int column = 0;
            for (; column + BLOCK <= result.w; column += BLOCK)
            {
                /*Ok. Here we have a 8x8 block to update*/
                Doublex4 s00 = Doublex4::Zero(); Doublex4 s01 = Doublex4::Zero();
                Doublex4 s10 = Doublex4::Zero(); Doublex4 s11 = Doublex4::Zero();
                Doublex4 s20 = Doublex4::Zero(); Doublex4 s21 = Doublex4::Zero();
                Doublex4 s30 = Doublex4::Zero(); Doublex4 s31 = Doublex4::Zero();

                Doublex4 s40 = Doublex4::Zero(); Doublex4 s41 = Doublex4::Zero();
                Doublex4 s50 = Doublex4::Zero(); Doublex4 s51 = Doublex4::Zero();
                Doublex4 s60 = Doublex4::Zero(); Doublex4 s61 = Doublex4::Zero();
                Doublex4 s70 = Doublex4::Zero(); Doublex4 s71 = Doublex4::Zero();

                Doublex4 a0, a1, a2, a3, b0, b1;

                const double *As = &A.a(row, 0);
                const double *Bs = &B.a(0, column);

                for (int runner = 0; runner < A.w; runner++)
                {

                    b0 = Doublex4(Bs);
                    b1 = Doublex4(Bs + 4);

                    Bs+=Bd;

                    const double *Of = As;

                    a0 = Doublex4::Broadcast(Of);  Of += Ad;
                    a1 = Doublex4::Broadcast(Of);  Of += Ad;

                    s00 = multiplyAdd(a0, b0, s00); s01 = multiplyAdd(a0, b1, s01);
                    s10 = multiplyAdd(a1, b0, s10); s11 = multiplyAdd(a1, b1, s11);

                    a2 = Doublex4::Broadcast(Of); Of += Ad;
                    a3 = Doublex4::Broadcast(Of); Of += Ad;

                    s20 = multiplyAdd(a2, b0, s20); s21 = multiplyAdd(a2, b1, s21);
                    s30 = multiplyAdd(a3, b0, s30); s31 = multiplyAdd(a3, b1, s31);

                    a0 = Doublex4::Broadcast(Of); Of += Ad;
                    a1 = Doublex4::Broadcast(Of); Of += Ad;

                    s40 = multiplyAdd(a0, b0, s40); s41 = multiplyAdd(a0, b1, s41);
                    s50 = multiplyAdd(a1, b0, s50); s51 = multiplyAdd(a1, b1, s51);

                    a2 = Doublex4::Broadcast(Of); Of += Ad;
                    a3 = Doublex4::Broadcast(Of); Of += Ad;

                    s60 = multiplyAdd(a2, b0, s60); s61 = multiplyAdd(a2, b1, s61);
                    s70 = multiplyAdd(a3, b0, s70); s71 = multiplyAdd(a3, b1, s71);

                    As++;

                }

                s00.save(&result.a(row + 0, column)); s01.save(&result.a(row + 0, column + 4));
                s10.save(&result.a(row + 1, column)); s11.save(&result.a(row + 1, column + 4));
                s20.save(&result.a(row + 2, column)); s21.save(&result.a(row + 2, column + 4));
                s30.save(&result.a(row + 3, column)); s31.save(&result.a(row + 3, column + 4));

                s40.save(&result.a(row + 4, column)); s41.save(&result.a(row + 4, column + 4));
                s50.save(&result.a(row + 5, column)); s51.save(&result.a(row + 5, column + 4));
                s60.save(&result.a(row + 6, column)); s61.save(&result.a(row + 6, column + 4));
                s70.save(&result.a(row + 7, column)); s71.save(&result.a(row + 7, column + 4));


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

/**
 * BlockMM8 memory buffers to pack matrices A and B for better cache performance.
 * @warning This context must be thread-local (each separate thread must have its own context)
 */
struct BlockMM8Context
{
    static const int MC = 384;
    static const int KC = 384;
    static const int NC = 4096;

    //  Local buffers for storing panels from A, B and local result
    double _A[MC * KC] __attribute__ ((aligned (32)));
    double _B[KC * NC] __attribute__ ((aligned (32)));
};

// This is made with help of tutorial: http://apfel.mathematik.uni-ulm.de/~lehn/sghpc/gemm/index.html
struct BlockMM8
{
    static const int BLOCK = 8;
    static const int MC = BlockMM8Context::MC;
    static const int KC = BlockMM8Context::KC;
    static const int NC = BlockMM8Context::NC;

    //  Local buffers for storing panels from A, B and local result
    double* _A;
    double* _B;
    double _result[BLOCK * BLOCK] __attribute__ ((aligned (32))) = { };

    //  Packing complete panels from A (i.e. without padding)
    static void pack_BLOCKxk(int k, const double *A, int incRowA, int incColA, double *buffer)
    {
        for (int j = 0; j < k; j++)
        {
            for (int i = 0; i < BLOCK; i++)
            {
                buffer[i] = A[i * incRowA];
            }
            buffer += BLOCK;
            A += incColA;
        }
    }

    //  Packing panels from A with padding if required
    static void pack_A(int mc, int kc, const double *A, int incRowA, int incColA, double *buffer)
    {
        int mp = mc / BLOCK;
        int _BLOCK = mc % BLOCK;

        for (int i = 0; i < mp; ++i) {
            pack_BLOCKxk(kc, A, incRowA, incColA, buffer);
            buffer += kc * BLOCK;
            A += BLOCK * incRowA;
        }
        if (_BLOCK > 0) {
            for (int j = 0; j < kc; j++)
            {
                for (int i = 0; i < _BLOCK; i++)
                {
                    buffer[i] = A[i * incRowA];
                }
                for (int i = _BLOCK; i < BLOCK; i++)
                {
                    buffer[i] = 0.0;
                }
                buffer += BLOCK;
                A += incColA;
            }
        }
    }

    //  Packing complete panels from B (i.e. without padding)
    static void pack_kxBLOCK(int k, const double *B, int incRowB, int incColB, double *buffer)
    {
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < BLOCK; j++)
            {
                buffer[j] = B[j * incColB];
            }
            buffer += BLOCK;
            B += incRowB;
        }
    }

    //  Packing panels from B with padding if required
    static void pack_B(int kc, int nc, const double *B, int incRowB, int incColB, double *buffer)
    {
        int np = nc / BLOCK;
        int _BLOCK = nc % BLOCK;

        for (int j = 0; j < np; j++)
        {
            pack_kxBLOCK(kc, B, incRowB, incColB, buffer);
            buffer += kc * BLOCK;
            B += BLOCK * incColB;
        }

        if (_BLOCK > 0) {
            for (int i = 0; i < kc; ++i)
            {
                for (int j = 0; j < _BLOCK; j++)
                {
                    buffer[j] = B[j * incColB];
                }
                for (int j = _BLOCK; j < BLOCK; j++)
                {
                    buffer[j] = 0.0;
                }
                buffer += BLOCK;
                B += incRowB;
            }
        }
    }

    //  Micro kernel for multiplying panels from A and B
    static void dgemm_micro_kernel(long kc, const double *A, const double *B, double *C, long incRowC, long incColC)
    {
        double AB[BLOCK * BLOCK] __attribute__ ((aligned (32)));

        Doublex4 s00 = Doublex4::Zero(); Doublex4 s10 = Doublex4::Zero();
        Doublex4 s01 = Doublex4::Zero(); Doublex4 s11 = Doublex4::Zero();
        Doublex4 s02 = Doublex4::Zero(); Doublex4 s12 = Doublex4::Zero();
        Doublex4 s03 = Doublex4::Zero(); Doublex4 s13 = Doublex4::Zero();
        Doublex4 s04 = Doublex4::Zero(); Doublex4 s14 = Doublex4::Zero();
        Doublex4 s05 = Doublex4::Zero(); Doublex4 s15 = Doublex4::Zero();
        Doublex4 s06 = Doublex4::Zero(); Doublex4 s16 = Doublex4::Zero();
        Doublex4 s07 = Doublex4::Zero(); Doublex4 s17 = Doublex4::Zero();

        Doublex4 a0123, a4567;

        Doublex4 b;

        for (int l = 0; l < kc; l++)
        {
            a0123 = Doublex4(A);
            a4567 = Doublex4(A + 4);

            b = Doublex4(B[0]);
            s00 = multiplyAdd(a0123, b, s00);
            s10 = multiplyAdd(a4567, b, s10);

            b = Doublex4(B[1]);
            s01 = multiplyAdd(a0123, b, s01);
            s11 = multiplyAdd(a4567, b, s11);

            b = Doublex4(B[2]);
            s02 = multiplyAdd(a0123, b, s02);
            s12 = multiplyAdd(a4567, b, s12);

            b = Doublex4(B[3]);
            s03 = multiplyAdd(a0123, b, s03);
            s13 = multiplyAdd(a4567, b, s13);

            b = Doublex4(B[4]);
            s04 = multiplyAdd(a0123, b, s04);
            s14 = multiplyAdd(a4567, b, s14);

            b = Doublex4(B[5]);
            s05 = multiplyAdd(a0123, b, s05);
            s15 = multiplyAdd(a4567, b, s15);

            b = Doublex4(B[6]);
            s06 = multiplyAdd(a0123, b, s06);
            s16 = multiplyAdd(a4567, b, s16);

            b = Doublex4(B[7]);
            s07 = multiplyAdd(a0123, b, s07);
            s17 = multiplyAdd(a4567, b, s17);

            A += 8;
            B += 8;
        }

        s00.save(&AB[0 + 0 * 8]); s10.save(&AB[4 + 0 * 8]);
        s01.save(&AB[0 + 1 * 8]); s11.save(&AB[4 + 1 * 8]);
        s02.save(&AB[0 + 2 * 8]); s12.save(&AB[4 + 2 * 8]);
        s03.save(&AB[0 + 3 * 8]); s13.save(&AB[4 + 3 * 8]);

        s04.save(&AB[0 + 4 * 8]); s14.save(&AB[4 + 4 * 8]);
        s05.save(&AB[0 + 5 * 8]); s15.save(&AB[4 + 5 * 8]);
        s06.save(&AB[0 + 6 * 8]); s16.save(&AB[4 + 6 * 8]);
        s07.save(&AB[0 + 7 * 8]); s17.save(&AB[4 + 7 * 8]);

        //  Update C <- C + AB
        for (int j = 0; j < BLOCK; j++)
        {
            for (int i = 0; i < BLOCK; i++)
            {
                C[i * incRowC + j * incColC] += AB[i + j * BLOCK];
            }
        }
    }

    //  Macro Kernel for the multiplication of blocks of A and B.  We assume that
    //  these blocks were previously packed to buffers _A and _B.
    void dgemm_macro_kernel(int mc, int nc, int kc, double *C, int incRowC, int incColC)
                       {
        int mp = (mc + BLOCK - 1) / BLOCK;
        int np = (nc + BLOCK - 1) / BLOCK;

        int _mr = mc % BLOCK;
        int _nr = nc % BLOCK;

        int mr, nr;

        for (int j = 0; j < np; j++)
        {
            nr = (j != np - 1 || _nr == 0) ? BLOCK : _nr;

            for (int i = 0; i < mp; i++)
            {
                mr = (i != mp - 1 || _mr == 0) ? BLOCK : _mr;

                if (mr == BLOCK && nr == BLOCK)
                {
                    dgemm_micro_kernel(kc, &_A[i * kc * BLOCK], &_B[j * kc * BLOCK],
                                       &C[i * BLOCK * incRowC + j * BLOCK * incColC], incRowC, incColC);
                } else {
                    dgemm_micro_kernel(kc, &_A[i * kc * BLOCK], &_B[j * kc * BLOCK],
                                       _result, BLOCK, 1);
                }
            }
        }
    }

    void operator()()
    {
        const Matrix &A = *pA;
        const Matrix &B = *pB;
        Matrix &result = *pResult;

        // mxk * kxn = mxn
        const int m = A.h;
        const int k = A.w;
        const int n = B.w;

        // How to access next value in the row / column
        const int incRowA = A.stride;
        const int incColA = 1;
        const int incRowB = B.stride;
        const int incColB = 1;
        const int incRowC = result.stride;
        const int incColC = 1;

        const long mb = (m + MC - 1) / MC;
        const long nb = (n + NC - 1) / NC;
        const long kb = (k + KC - 1) / KC;

        const int _mc = m % MC;
        const int _nc = n % NC;
        const int _kc = k % KC;

        int mc, nc, kc;

        for (int j = 0; j < nb; j++)
        {
            nc = (j != nb - 1 || _nc == 0) ? NC : _nc;

            for (int l = 0; l < kb; l++)
            {
                kc = (l != kb - 1 || _kc == 0) ? KC : _kc;
                pack_B(kc, nc, &B.a(l * KC, j * NC), incRowB, incColB, _B);

                for (int i = 0; i < mb; i++)
                {
                    mc = (i != mb - 1 || _mc == 0) ? MC : _mc;
                    pack_A(mc, kc,&A.a(i * MC, l * KC), incRowA, incColA, _A);
                    dgemm_macro_kernel(mc, nc, kc,&result.a(i * MC, j * NC), incRowC, incColC);
                }
            }
        }
    }

    BlockMM8(const Matrix *pA, const Matrix *pB, Matrix *pResult, BlockMM8Context& context)
            : pA(pA), pB(pB), pResult(pResult), _A(context._A), _B(context._B)
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