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

#include "core/utils/global.h"

#include "core/math/vector/vectorOperations.h"
#include "core/utils/preciseTimer.h"
#include "core/math/vector/fixedVector.h"
#include "core/math/vector/fixedArray.h"

#include "core/math/vector/vector3d.h"

using namespace corecvs;
using namespace std;

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
    std::cout << sum << std::endl;

    printf("Profiling Vector Operation Implementation\n");
    start = PreciseTimer::currentTime();
    sum = 0;
    for (int i = 0; i < CYCLES; i++)
    {
        sum += (vec1[i] + 2 * vec2[i]).sumAllElementsSq();
    }
    delay = start.usecsToNow();
    printf("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us\n", delay, delay / 1000, delay / CYCLES); fflush(stdout);
    std::cout << sum << std::endl;

    printf("Profiling Fixed Vector Implementation\n");
    start = PreciseTimer::currentTime();
    sum = 0;
    for (int i = 0; i < CYCLES; i++)
    {
        sum += (vec1f[i] + 2 * vec2f[i]).sumAllElementsSq();
    }
    delay = start.usecsToNow();
    printf("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us\n", delay, delay / 1000, delay / CYCLES); fflush(stdout);
    std::cout << sum << std::endl;

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

TEST(Vector, prod)
{
    const int LENGTH = 8;
    FixedArray<int> arr(LENGTH);
    for (int i = 0; i < LENGTH; i++)
    {
        arr[i] = i + 1;
    }
    ASSERT_EQ(arr.prod(), 1*2*3*4*5*6*7*8);
}

TEST(Vector, cwise)
{
    FixedArray<int> r({  1,   2,  3,  4,  5,  6,  7,  8 });
    FixedArray<int> a({  1,  -2,  3, -4,  5, -6,  7, -8 });
    FixedArray<int> b({ -1,   2, -3,  4, -5,  6, -7,  8 });

    ASSERT_EQ(a.cwiseAbs(), r);
    ASSERT_EQ(a.cwiseMin(b), -r);
    ASSERT_EQ(a.cwiseMax(b), r);

    ASSERT_EQ(FixedArray<double>({ 1, 4, 9, 16, 25 }).cwiseSqrt(), FixedArray<double>({ 1, 2, 3, 4, 5 }));
}

TEST(Vector, testSpherical)
{
    cout << "Case 1:" << endl;
    {
        double longitude = degToRad(45.0);
        double latitude  = degToRad(45.0);
        double length    = 45.0;

        Vector3dd vec = Vector3dd::FromSpherical(latitude, longitude, length);
        cout << "Vector:" << vec << std::endl;

        cout << "Length:" << vec.l2Metric() << std::endl;


        Vector3dd back = Vector3dd::toSpherical(vec);
        cout << "Back:" << back << std::endl;

        ASSERT_DOUBLE_EQ( length, back.z());

        ASSERT_DOUBLE_EQ(latitude , back.x());
        ASSERT_DOUBLE_EQ(longitude, back.y());
    }

    cout << "Case 2:" << endl;
    {
        double longitude = degToRad(50.0);
        double latitude  = degToRad(20.0);
        double length    = 45.0;

        Vector3dd vec = Vector3dd::FromSpherical(latitude, longitude, length);
        cout << "Vector:" << vec << std::endl;

        cout << "Length:" << vec.l2Metric() << std::endl;


        Vector3dd back = Vector3dd::toSpherical(vec);
        cout << "Back:" << back << std::endl;

        ASSERT_DOUBLE_EQ( length, back.z());

        ASSERT_DOUBLE_EQ(latitude , back.x());
        ASSERT_DOUBLE_EQ(longitude, back.y());
    }


    {
        Vector3dd vec(0.0, 0.0, 50.0);
        Vector3dd back = Vector3dd::toSpherical(vec);
        CORE_UNUSED(back);
    }
}


TEST(Vector, streamVectors)
{
    vector<double> v;
    v.resize(6);
    double *buffer = v.data();

    Vector3dd a1(1.0, 2.0, 3.0);
    Vector3dd a2(4.0, 5.0, 6.0);

    a1.storeToStream(buffer);
    a2.storeToStream(buffer);

    Vector3dd b1(0.0);
    Vector3dd b2(0.0);

    const double *input = v.data();

    b1.loadFromStream(input);
    b2.loadFromStream(input);

    cout << a1 << " = " << b1 << endl;
    cout << a2 << " = " << b2 << endl;

    CORE_ASSERT_TRUE(a1 == b1, "Load Error");
    CORE_ASSERT_TRUE(a2 == b2, "Load Error2");
}

//int main (int /*argC*/, char ** /*argV*/)
//{
//    profileVectorOperations ();
//    testFixedArray ();
//        cout << "PASSED" << endl;
//        return 0;
//}
