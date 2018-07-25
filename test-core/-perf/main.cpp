#include "gtest/gtest.h"
#include "core/buffers/g12Buffer.h"

#include "core/utils/preciseTimer.h"
#include "core/tbbwrapper/tbbWrapper.h"
#include "core/math/sse/sseWrapper.h"

using namespace corecvs;

static const int LIMIT  = 150000000;
static const int REPEAT = 10;


class FMAAdder {
public:
    int16_t *A = NULL;
    int16_t *B = NULL;

    FMAAdder(int16_t *A, int16_t *B) :
        A(A), B(B)
    {}

    void operator()(BlockedRange<int> range) const
    {
        for (int i = range.begin(); i < range.end(); i++)
        {
            int c = 5;
            A[i] = sqrt(A[i] + c * B[i]);
        }
    }
};

void trivialRun(int16_t *A, int16_t *B)
{
    {
        SYNC_PRINT(("Trival run\n"));

        PreciseTimer timer = PreciseTimer::currentTime();

        for (int rep = 0; rep < REPEAT; rep++)
        {
            for (int i = 0; i < LIMIT; i++)
            {
                int c = 5;
                A[i] = sqrt(A[i] + c * B[i]);
            }
        }

        uint64_t us = timer.usecsToNow();
        SYNC_PRINT(("%.3lf msecs elapsed\n", us / 1000.0));
    }

}

void optim(int16_t *A, int16_t *B)
{
    SYNC_PRINT(("TBB run lambda intr\n"));

    PreciseTimer timer = PreciseTimer::currentTime();

    for (int rep = 0; rep < REPEAT; rep++)
    {
        parallelable_for(0, LIMIT,
        [&](BlockedRange<int> range){
             for (int i = range.begin(); i < range.end(); i+= 16)
             {
                 int c = 5;
                 /*
                    Int16x16 AV(a[i]);
                    Int16x16 BV(b[i]);
                    AV += BV * 5;


                */
                 __m256i ai, bi, ci;
                 ai = _mm256_loadu_si256((__m256i *)&A[i]);
                 bi = _mm256_loadu_si256((__m256i *)&B[i]);
                 ci = _mm256_set1_epi16(c);
                 bi = _mm256_mullo_epi16(bi, ci);

                 ai = _mm256_adds_epi16(ai, bi);

                 __m256 in = _mm256_castsi256_ps(ai);
                 __m256 root = _mm256_sqrt_ps (in);
                 ai = _mm256_castps_si256(root);

                 _mm256_storeu_si256((__m256i *)&A[i],ai);

                 //A[i] = sqrt(A[i] + c * B[i]);

             }
         });
    }

    uint64_t us = timer.usecsToNow();
    SYNC_PRINT(("%.3lf msecs elapsed\n", us / 1000.0));
}


TEST(TestFMA, fma)
{
    cout << "TBB" << tbbInfo() << endl;

    //A += B * const;
    int16_t *A = new int16_t[LIMIT];
    int16_t *B = new int16_t[LIMIT];

    trivialRun(A,B);
    {
        SYNC_PRINT(("TBB run class\n"));

        PreciseTimer timer = PreciseTimer::currentTime();

        for (int rep = 0; rep < REPEAT; rep++)
        {
            FMAAdder adder(A ,B);
            parallelable_for(0, LIMIT, adder);
        }

        uint64_t us = timer.usecsToNow();
        SYNC_PRINT(("%.3lf msecs elapsed\n", us / 1000.0));
    }

    {
        SYNC_PRINT(("TBB run lambda\n"));

        PreciseTimer timer = PreciseTimer::currentTime();

        for (int rep = 0; rep < REPEAT; rep++)
        {
            parallelable_for(0, LIMIT,
            [&](BlockedRange<int> range){
                 for (int i = range.begin(); i < range.end(); i++)
                 {
                     int c = 5;
                     A[i] = sqrt(A[i] + c * B[i]);
                 }
             });
        }

        uint64_t us = timer.usecsToNow();
        SYNC_PRINT(("%.3lf msecs elapsed\n", us / 1000.0));
    }
    optim(A,B);

}

int main(int argc, char **argv)
{

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
