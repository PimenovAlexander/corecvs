/**
 * \file main_test_vector.cpp
 * \brief This is the main file for the test vector 
 *
 * \date Jun 14, 2011
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <stdlib.h>
#include <iostream>
#include "gtest/gtest.h"

#include "global.h"

#include "vectorOperations.h"
#include "preciseTimer.h"
#include "fixedVector.h"
#include "fixedArray.h"

using namespace std;
using namespace corecvs;

template<int length>
class TestVector : public VectorOperationsBase< TestVector<length>, int>
{
public:
    TestVector(){}
    TestVector(int /*i*/){}

    typedef int InnerElementType;
    int elements[length];

    inline int &at(int i) {
        return elements[i];
    }

    inline const int &at(int i) const {
        return elements[i];
    }

    inline int size() const {
        return length;
    }

    inline TestVector createVector(int /*length*/) const {
          return TestVector();
    }

};

TEST(Vector, profileVectorOperations)
{
    PreciseTimer start;
    uint64_t delay;
    const int LENGTH = 8;
    const int CYCLES = 10000000;


    TestVector<LENGTH> *vec1 = new TestVector<LENGTH> [CYCLES];
    TestVector<LENGTH> *vec2 = new TestVector<LENGTH> [CYCLES];

    FixedVector<int,LENGTH> *vec1f = new FixedVector<int,LENGTH> [CYCLES];
    FixedVector<int,LENGTH> *vec2f = new FixedVector<int,LENGTH> [CYCLES];

    for (int i = 0; i < CYCLES; i++)
    {
        for (int j = 0; j < vec1[i].size(); j++)
        {
            int a = rand() % 1000;
            int b = rand() % 1000;
            vec1[i].at(j) = a;
            vec2[i].at(j) = b;
            vec1f[i].at(j) = a;
            vec2f[i].at(j) = b;
        }
    }


    printf("Profiling Naive Implementation\n");
    start = PreciseTimer::currentTime();
    int sum = 0;
    for (int i = 0; i < CYCLES; i++)
    {
        TestVector<LENGTH> tmp;
        for (int j = 0; j < vec1[i].size(); j++)
        {
            tmp.at(j) = (vec1[i].at(j) + 2 * vec2[i].at(j));
        }

        for (int j = 0; j < vec1[i].size(); j++)
        {
            sum += tmp.at(j) * tmp.at(j);
        }
    }
    delay = start.usecsToNow();
    printf("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us\n", delay, delay / 1000, delay / CYCLES); fflush(stdout);
    cout << sum << endl;

    printf("Profiling Vector Operation Implementation\n");
    start = PreciseTimer::currentTime();
    sum = 0;
    for (int i = 0; i < CYCLES; i++)
    {
        sum += (vec1[i] + 2 * vec2[i]).sumAllElementsSq();
    }
    delay = start.usecsToNow();
    printf("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us\n", delay, delay / 1000, delay / CYCLES); fflush(stdout);
    cout << sum << endl;

    printf("Profiling Fixed Vector Implementation\n");
    start = PreciseTimer::currentTime();
    sum = 0;
    for (int i = 0; i < CYCLES; i++)
    {
        sum += (vec1f[i] + 2 * vec2f[i]).sumAllElementsSq();
    }
    delay = start.usecsToNow();
    printf("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us\n", delay, delay / 1000, delay / CYCLES); fflush(stdout);
    cout << sum << endl;



    delete[] vec1;
    delete[] vec2;

    delete[] vec1f;
    delete[] vec2f;
}

TEST(Vector, testFixedArray)
{
    const int LENGTH = 8;
    FixedArray<int> arr(LENGTH);
    ASSERT_EQ(arr.size(), LENGTH);
}

TEST(Vector, MulAllElements)
{
    const int LENGTH = 8;
    FixedArray<int> arr(LENGTH);
    for (int i = 0; i < LENGTH; i++)
    {
        arr[i] = i + 1;
    }
    cout << arr.mulAllElements() << endl;
    ASSERT_EQ(arr.mulAllElements(), 40320);
}

//int main (int /*argC*/, char ** /*argV*/)
//{
//    profileVectorOperations ();
//    testFixedArray ();
//        cout << "PASSED" << endl;
//        return 0;
//}
