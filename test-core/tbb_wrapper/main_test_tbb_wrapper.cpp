/**
 * \file main_test_tbb_wrapper.cpp
 * \brief This is the main file for the test tbb_wrapper 
 *
 * \date Jun 21, 2011
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/tbbwrapper/tbbWrapper.h"
#ifdef WITH_TBB
#include <tbb/task.h>
#endif

using namespace std;
using namespace corecvs;

class ParallelTestTBBWrapper
{
public:
    uint8_t *data;

    ParallelTestTBBWrapper(uint8_t *_data) : data(_data) {}

    void operator()( const BlockedRange<int>& r ) const
    {
        DOTRACE(("Wrapped: Interval [%d : %d)\n", r.begin(), r.end()));
        for (int i = r.begin(); i < r.end(); i++)
        {
            data[i] = 0xFF;
        }
    }

#ifdef WITH_TBB
    void operator()( const tbb::blocked_range<int>& r ) const
    {
        DOTRACE(("TBB   : Interval [%d : %d)\n", r.begin(), r.end()));
        for (int i = r.begin(); i < r.end(); i++)
        {
            data[i] = 0x7F;
        }
    }
#endif
};

TEST(TBBWrapperTest, testWrapper)
{
    const int SIZE = 9;
    uint8_t *data = new uint8_t[SIZE];
    memset(data, 0, sizeof(uint8_t) * SIZE);

#ifdef WITH_TBB
    tbb::parallel_for(tbb::blocked_range<int>(0, SIZE), ParallelTestTBBWrapper(data));
    for (int i = 0; i < SIZE; i++)
    {
        CORE_ASSERT_TRUE_P(data[i] == 0x7F, ("TBB Wrapper has problems at pos %d found %d", i, data[i]));
    }
#endif

    parallelable_for(0, SIZE, ParallelTestTBBWrapper(data));

    for (int i = 0; i < SIZE; i++)
    {
        CORE_ASSERT_TRUE_P(data[i] == 0xFF, ("TBB Wrapper has problems at pos %d found %d", i, data[i]));
    }
    delete[] data;
}

TEST(TBBWrapperTest, testReduce)
{
    const size_t LIMIT = 10;

    std::vector<uint64_t> in(LIMIT);
    parallelable_for((size_t)0, in.size(), [&](const BlockedRange<size_t>& r)
    {
        for (size_t id = r.begin(); id < r.end(); id++ ) {
            in[id] = id;
        }
    });

    uint64_t sum = 0;
    for (size_t i = 0; i < in.size(); i++)
    {
        sum += in[i];
    }

    double expected1 = LIMIT * (LIMIT - 1) / 2;
    cout << "sum  :" << sum << endl;
    cout << "exp:"   << expected1 << endl;

    CORE_ASSERT_TRUE(sum == expected1, "Wrong fill function");

    /*=================================*/

    uint64_t value1 =
    parallelable_reduce((size_t)0, in.size(), (uint64_t)0,
        [&]( const corecvs::BlockedRange<size_t> &r, const uint64_t init) {
            uint64_t result = init;
            for (size_t id = r.begin(); id < r.end(); id++ ) {
                result += in[id] * in[id];
            }
            return result;
        },
        [&](const uint64_t collected, const uint64_t add)
        {
            return collected + add;
        }, true);

    uint64_t value2 =
    parallelable_reduce((size_t)0, in.size(), (uint64_t)0,
        [&]( const corecvs::BlockedRange<size_t> &r, const uint64_t init) {
            uint64_t result = init;
            for (size_t id = r.begin(); id < r.end(); id++ ) {
                result += in[id] * in[id];
            }
            return result;
        },
        [&](const uint64_t collected, const uint64_t add)
        {
            return collected + add;
        }, false);

    uint64_t expected2 = LIMIT * (LIMIT - 1) * (2 * LIMIT - 1) / 6;
    cout << "sumsq1  :" << value1 << endl;
    cout << "sumsq2  :" << value2 << endl;
    cout << "expected:" << expected2 << endl;

    CORE_ASSERT_TRUE(value1 == expected2, "Wrong reduce");
    CORE_ASSERT_TRUE(value2 == expected2, "Wrong reduce when no parallel");


}

/*================= Task Tester ====================*/

#ifdef WITH_TBB

class TestTask : public tbb::task
{
public:
    int      len;
    uint8_t *data;
    int      value;

    TestTask(int _len, uint8_t *_data, int _value) : len(_len), data(_data), value(_value) {}

    tbb::task* execute()
    {
        for (int i = 0; i < len; i++)
            data[i] = value;
        return NULL;
    }
};

TEST(TBBWrapperTest, testTasks)
{
    const int SIZE = 90;
    uint8_t *data = new uint8_t[SIZE];
    memset(data, 0, sizeof(uint8_t) * SIZE);

    tbb::task_list list;
    for (int i = 0; i < SIZE; i += 10)
    {
        list.push_back(*new(task::allocate_root()) TestTask(10, data + i, 20 + (i / 10)));
    }

    tbb::task::spawn_root_and_wait(list);

    for (int i = 0; i < SIZE; i++)
    {
        printf("%d ", data[i]);
    }
    printf("\n");
    delete[] data;
}

#endif // WITH_TBB
