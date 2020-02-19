#ifndef BLAS_REPLACEMENT_H
#define BLAS_REPLACEMENT_H

#include "utils/global.h"
#include "math/matrix/matrix.h"
#include "math/matrix/matrix33.h"

#include "tbbwrapper/tbbWrapper.h"
#include "math/sse/sseWrapper.h"
#include "math/sse/doublex4.h"

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

//#ifdef WITH_AVX
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
//#endif

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

    //static const int MC = 384;
    //static const int KC = 384;
    //static const int NC = 8192;

    //  Local buffers for storing panels from A, B and local result
    double ALIGN_DATA(32) _A[MC * KC];
    double ALIGN_DATA(32) _B[KC * NC];
};

/**
 * This is made with help of tutorial: http://apfel.mathematik.uni-ulm.de/~lehn/sghpc/gemm/index.html
 *
 * The implementation of the matrix-matrix product is cache optimized.
 * MC and KC serve as cache block sizes used by the higher-level blocked algorithms
 * to partition the matrix down to cache optimized matrix macro-blocks, implemented as a macro-kernel.
 * They, in order to accelerate product, should be packed (copied into local buffers _A, _B, and in _B blocks are transposed).
 * Macro-blocks consist of panels -- micro-blocks: the size of panel for A is BlOCKxKC, for B panel size is BlOCKxMC,
 * BlOCK serves as register block size for the micro-kernel, where we multiply panels and compute the product with AVX.
 */
struct BlockMM8
{
    static const int BLOCK = 8;
    static const int MC = BlockMM8Context::MC;
    static const int KC = BlockMM8Context::KC;
    static const int NC = BlockMM8Context::NC;

    //  Local buffers for storing panels from A, B and local result
    double* _A = NULL;
    double* _B = NULL;
    double ALIGN_DATA(32) _result[BLOCK * BLOCK] = { };

    //  Packing complete panels from A (i.e. without padding)
    static void pack_BLOCKxk(int k, const double *A, int incRowA, double *buffer)
    {
        for (int j = 0; j < k; j++)
        {
            for (int i = 0; i < BLOCK; i++)
            {
                buffer[i] = A[i * incRowA];
            }
            buffer += BLOCK;
	    A += 1;           
        }
    }

    //  Packing panels from A with padding if required
    static void pack_A(int mc, int kc, const double *A, int incRowA, double *buffer)
    {
        int mp = mc / BLOCK;
        int _BLOCK = mc % BLOCK;

        for (int i = 0; i < mp; ++i) {
            pack_BLOCKxk(kc, A, incRowA, buffer);
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
                A += 1;
            }
        }
    }

    //  Packing complete panels from B (i.e. without padding)
    static void pack_kxBLOCK(int k, const double *B, int incRowB, double *buffer)
    {
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < BLOCK; j++)
            {
                buffer[j] = B[j];
            }
            buffer += BLOCK;
            B += incRowB;
        }
    }

    //  Packing panels from B with padding if required
    static void pack_B(int kc, int nc, const double *B, int incRowB, double *buffer)
    {
        int np = nc / BLOCK;
        int _BLOCK = nc % BLOCK;

        for (int j = 0; j < np; j++)
        {
            pack_kxBLOCK(kc, B, incRowB, buffer);
            buffer += kc * BLOCK;
            B += BLOCK;
        }

        if (_BLOCK > 0) {
            for (int i = 0; i < kc; ++i)
            {
                for (int j = 0; j < _BLOCK; j++)
                {
                    buffer[j] = B[j];
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

    /**
     *  Micro kernel for multiplying panels from A and B
     *
     *  A stores a number of the line spans of BLOCK (so far 8) elements
     *
     *    \f[ \pmatrix{ A_{10} & A_{11} & A_{12} & A_{13} & A_{14} & A_{15} & A_{16} & A_{17} & | & A_{21} & \ldots & A_{n5} & A_{n6} & A_{n7} } \f]
     *
     *  B stores a number of the line spans of BLOCK (so far 8) elements
     *
     *    \f[ \pmatrix{ B_{10} & B_{11} & B_{12} & B_{13} & B_{14} & B_{15} & B_{16} & B_{17} & | & B_{21} & \ldots & B_{n5} & B_{n6} & B_{n7} } \f]
     *
     *  This function computes vector A * vector B product
     *
     * \f[
     *  \pmatrix{
     *    \sum^{kc} A_0  B_0 & \sum^{kc}  A_1 B_0 & \ldots & \sum^{kc} A_6 B_0 & \sum^{kc} A_7 B_0 \cr
     *          \vdots                    & & \ldots &                & \vdots \cr
     *          \vdots                    & & \ldots &                & \vdots \cr
     *    \sum^{kc} A_0  B_7 & \sum^{kc}  A_1 B_7 & \ldots & \sum^{kc} A_6 B_7 & \sum^{kc} A_7 B_7
     *  }
     * \f]
     *
     *
     **/
    static void dgemm_micro_kernel_avx(long kc, const double *A, const double *B, double *C, long incRowC)
    {       

        // Compute AB = A*B
        Doublex4 s00 = Doublex4::Zero(); Doublex4 s10 = Doublex4::Zero();
        Doublex4 s01 = Doublex4::Zero(); Doublex4 s11 = Doublex4::Zero();
        Doublex4 s02 = Doublex4::Zero(); Doublex4 s12 = Doublex4::Zero();
        Doublex4 s03 = Doublex4::Zero(); Doublex4 s13 = Doublex4::Zero();
        Doublex4 s04 = Doublex4::Zero(); Doublex4 s14 = Doublex4::Zero();
        Doublex4 s05 = Doublex4::Zero(); Doublex4 s15 = Doublex4::Zero();
        Doublex4 s06 = Doublex4::Zero(); Doublex4 s16 = Doublex4::Zero();
        Doublex4 s07 = Doublex4::Zero(); Doublex4 s17 = Doublex4::Zero();

        Doublex4 b0123, b4567;

        Doublex4 a;

        for (int l = 0; l < kc; l++)
        {
            b0123 = Doublex4(B);
            b4567 = Doublex4(B + 4);

            a = Doublex4(A[0]);
            s00 = multiplyAdd(b0123, a, s00);
            s10 = multiplyAdd(b4567, a, s10);

            a = Doublex4(A[1]);
            s01 = multiplyAdd(b0123, a, s01);
            s11 = multiplyAdd(b4567, a, s11);

            a = Doublex4(A[2]);
            s02 = multiplyAdd(b0123, a, s02);
            s12 = multiplyAdd(b4567, a, s12);

            a = Doublex4(A[3]);
            s03 = multiplyAdd(b0123, a, s03);
            s13 = multiplyAdd(b4567, a, s13);

            a = Doublex4(A[4]);
            s04 = multiplyAdd(b0123, a, s04);
            s14 = multiplyAdd(b4567, a, s14);

            a = Doublex4(A[5]);
            s05 = multiplyAdd(b0123, a, s05);
            s15 = multiplyAdd(b4567, a, s15);

            a = Doublex4(A[6]);
            s06 = multiplyAdd(b0123, a, s06);
            s16 = multiplyAdd(b4567, a, s16);

            a = Doublex4(A[7]);
            s07 = multiplyAdd(b0123, a, s07);
            s17 = multiplyAdd(b4567, a, s17);

            A += BLOCK;
            B += BLOCK;
        }

        Doublex4 c00; c00.load(C                  ); c00 += s00; c00.save(C                  );
        Doublex4 c10; c10.load(C +               4); c10 += s10; c10.save(C +               4);

        Doublex4 c01; c01.load(C + incRowC        ); c01 += s01; c01.save(C + incRowC        );
        Doublex4 c11; c11.load(C + incRowC     + 4); c11 += s11; c11.save(C + incRowC     + 4);

        Doublex4 c02; c02.load(C + incRowC * 2    ); c02 += s02; c02.save(C + incRowC * 2    );
        Doublex4 c12; c12.load(C + incRowC * 2 + 4); c12 += s12; c12.save(C + incRowC * 2 + 4);

        Doublex4 c03; c03.load(C + incRowC * 3    ); c03 += s03; c03.save(C + incRowC * 3    );
        Doublex4 c13; c13.load(C + incRowC * 3 + 4); c13 += s13; c13.save(C + incRowC * 3 + 4);

        // -----------

        Doublex4 c04; c04.load(C + incRowC * 4    ); c04 += s04; c04.save(C + incRowC * 4    );
        Doublex4 c14; c14.load(C + incRowC * 4 + 4); c14 += s14; c14.save(C + incRowC * 4 + 4);

        Doublex4 c05; c05.load(C + incRowC * 5    ); c05 += s05; c05.save(C + incRowC * 5    );
        Doublex4 c15; c15.load(C + incRowC * 5 + 4); c15 += s15; c15.save(C + incRowC * 5 + 4);

        Doublex4 c06; c06.load(C + incRowC * 6    ); c06 += s06; c06.save(C + incRowC * 6    );
        Doublex4 c16; c16.load(C + incRowC * 6 + 4); c16 += s16; c16.save(C + incRowC * 6 + 4);

        Doublex4 c07; c07.load(C + incRowC * 7    ); c07 += s07; c07.save(C + incRowC * 7    );
        Doublex4 c17; c17.load(C + incRowC * 7 + 4); c17 += s17; c17.save(C + incRowC * 7 + 4);
    }

    static void dgemm_micro_kernel(int mc, int nc, long kc, const double *A, const double *B, double *C, long incRowC)
    {
        for (int l = 0; l < kc; l++) {
            for (int i = 0; i < mc; i++) {
                for (int j = 0; j < nc; j++) {
                    C[i * incRowC + j] += A[i] * B[j];
                }
            }

            A += BLOCK;
            B += BLOCK;
        }
    }

    /**
     *   Macro Kernel for the multiplication of blocks of A and B.  We assume that
     *   these blocks were previously packed to buffers _A and _B.
     **/
    void dgemm_macro_kernel(int mc, int nc, int kc, double *C, int incRowC)
    {
        int mp = mc / BLOCK;
        int np = nc / BLOCK;

        int mr = mc % BLOCK;
        int nr = nc % BLOCK;

#if 0
        parallelable_for(0, np, [&](const BlockedRange<int> &r){
        for (int j = r.begin(); j < r.end(); j++)
        {
            for (int i = 0; i < mp; i++)
            {
                    dgemm_micro_kernel_avx(
                            kc,
                            &_A[i * kc * BLOCK],
                            &_B[j * kc * BLOCK],
                            & C[i * BLOCK * incRowC + j * BLOCK], incRowC);
            }
        }}, true);
#endif
        parallelable_for2d(0, np, 0, mp,
                           [&](const BlockedRange2d<int, int> &r){
        for (int j = r.getRows().begin(); j < r.getRows().end(); j++)
        {
            for (int i = r.getCols().begin(); i < r.getCols().end(); i++)
            {
                    dgemm_micro_kernel_avx(
                            kc,
                            &_A[i * kc * BLOCK],
                            &_B[j * kc * BLOCK],
                            & C[i * BLOCK * incRowC + j * BLOCK], incRowC);
            }
        }}, true);

        /* Work with the boundary */
#if 0
        cout << "mc:" << mc << endl;
        cout << "nc:" << nc << endl;
        cout << "kc:" << kc << endl;

        cout << "mp:" << mp << endl;
        cout << "np:" << np << endl;

        cout << "mr:" << mr << endl;
        cout << "nr:" << nr << endl;
#endif
        if (nr != 0) {
            int j = np;
            for (int i = 0; i < mp; i++)
            {
                dgemm_micro_kernel(BLOCK, nr, kc,
                        &_A[i * kc * BLOCK],
                        &_B[j * kc * BLOCK],
                        & C[i * BLOCK * incRowC + j * BLOCK], incRowC);
            }
        }

        if (mr != 0) {
            int i = mp;
            for (int j = 0; j < np; j++)
            {
                dgemm_micro_kernel(mr, BLOCK, kc,
                        &_A[i * kc * BLOCK],
                        &_B[j * kc * BLOCK],
                        & C[i * BLOCK * incRowC + j * BLOCK], incRowC);
            }
        }

        if (mr != 0 && nr != 0) {
            dgemm_micro_kernel(mr, nr, kc,
                    &_A[mp * kc * BLOCK],
                    &_B[np * kc * BLOCK],
                    & C[mp * BLOCK * incRowC + np * BLOCK], incRowC);
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
        const int incRowB = B.stride;
        const int incRowC = result.stride;

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
                pack_B(kc, nc, &B.a(l * KC, j * NC), incRowB, _B);

                for (int i = 0; i < mb; i++)
                {
                    mc = (i != mb - 1 || _mc == 0) ? MC : _mc;
                    pack_A(mc, kc,&A.a(i * MC, l * KC), incRowA, _A);
                    dgemm_macro_kernel(mc, nc, kc,&result.a(i * MC, j * NC), incRowC);
                }
            }
        }
    }

    BlockMM8(const Matrix *pA, const Matrix *pB, Matrix *pResult, BlockMM8Context& context)
            : _A(context._A), _B(context._B), pA(pA), pB(pB), pResult(pResult)
    {
    }

    const Matrix *pA = NULL;
    const Matrix *pB = NULL;
    Matrix *pResult  = NULL;

};

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

} //namespace corecvs

#endif  // BLAS_REPLACEMENT_H
