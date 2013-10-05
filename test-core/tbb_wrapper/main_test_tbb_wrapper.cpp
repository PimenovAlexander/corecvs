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

#include "global.h"

#include "tbbWrapper.h"
#ifdef WITH_TBB
#include <tbb/task.h>
#endif



using namespace std;
using namespace corecvs;


class ParallelTestTBBWrapper
{
public:
    uint8_t *data;

    ParallelTestTBBWrapper(uint8_t *_data) : data(_data) {};

    void operator()( const BlockedRange<int>& r ) const
    {
        DOTRACE(("Wrapped: Interval [%d : %d)\n", r.begin(), r.end()));
        for( int i = r.begin(); i < r.end(); i ++ )
        {
            data[i] = 0xFF;
        }
    }

#ifdef WITH_TBB
    void operator()( const tbb::blocked_range<int>& r ) const
    {
        DOTRACE(("TBB   : Interval [%d : %d)\n", r.begin(), r.end()));
        for( int i = r.begin(); i < r.end(); i ++ )
        {
            data[i] = 0x7F;
        }
    }
#endif
};

void testTBBWrapper(void)
{
    const int SIZE = 9;
    uint8_t *data = new uint8_t[SIZE];
    memset(data, 0, sizeof(uint8_t) * SIZE);

#ifdef WITH_TBB
    tbb::parallel_for(tbb::blocked_range<int>(0, SIZE), ParallelTestTBBWrapper(data));
    for (int i = 0; i < SIZE; i++)
    {
        ASSERT_TRUE_P(data[i] == 0x7F,("TBB Wrapper has problems at pos %d found %d", i, data[i]));
    }
#endif

    parallelable_for(0, SIZE, ParallelTestTBBWrapper(data));

    for (int i = 0; i < SIZE; i++)
    {
        ASSERT_TRUE_P(data[i] == 0xFF,("TBB Wrapper has problems at pos %d found %d", i, data[i]));
    }
    delete[] data;
}

/*================= Task Tester ====================*/

#ifdef WITH_TBB
class TestTask : public tbb::task
{
public:
    int len;
    uint8_t *data;
    int value;

    TestTask(int _len, uint8_t *_data, int _value) :
        len(_len),
        data(_data),
        value(_value)
    {}

    tbb::task* execute()
    {
        for (int i = 0; i < len; i++)
            data[i] = value;
        return NULL;
    }
};


void testTasks( void )
{
    const int SIZE = 90;
    uint8_t *data = new uint8_t[SIZE];
    memset(data, 0, sizeof(uint8_t) * SIZE);


    tbb::task_list list;
    for (int i = 0; i < SIZE; i+=10)
    {
        list.push_back( *new(task::allocate_root()) TestTask(10, data+i, 20 + (i / 10)));
    }

    tbb::task::spawn_root_and_wait(list);


    for (int i = 0; i < SIZE; i++)
    {
        printf("%d ", data[i]);
    }
}
#endif


int main (int /*argC*/, char ** /*argV*/)
{
    testTBBWrapper();
#ifdef WITH_TBB
    testTasks();
#endif
        cout << "PASSED" << endl;
        return 0;
}
