/**
 * \file main_test_assignment.cpp
 * \brief This is the main file for the test assignment
 *
 * \date Oct 17, 2010
 * \author alexander
 *
 * \ingroup autotest
 */

#include <iostream>
#include "gtest/gtest.h"

#include "global.h"

#include "g12Buffer.h"
#include "assignmentOptimal.h"


using namespace corecvs;

void printAssignment(AbstractBuffer<int> *assignment, unsigned h, unsigned w)
{
    unsigned i,j;
    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            printf(" %s ", (((unsigned)assignment->element(i, 0)) == j)? "1" : "0");
        }
        printf("\n");
    }
}

template<typename Type>
Type computeCost(AbstractBuffer<int> *mapping, AbstractBuffer<Type> *matrix)
{
    Type sum = Type(0);
    for (int i = 0; i < mapping->h; i++)
    {
        sum += matrix->element(i, mapping->element(i,0));
    }
    return sum;
}


/*
void doTestLimits (void)
{
    G12Buffer *M1 = CreateMatrix(1,1);
    G12Buffer *M = CreateMatrix(0,0);
    if (assignmentoptimal(NULL, M1, NULL))
    {
        printf("Fail");
    }
    if (assignmentoptimal(M1, NULL, NULL))
    {
        printf("Fail");
    }
    if (assignmentoptimal(M, M, NULL))
    {
        printf("Fail");
    }
    if (assignmentoptimal(M, M1, NULL))
    {
        printf("Fail");
    }
    DeleteMatrix(&M1);
    DeleteMatrix(&M);
}
*/

TEST(Assignment, testSizes)
{
    printf("Starting test...\n");

    float matData[] =  {
        1.0,  2.0,  3.0, 5.0,
        2.0,  4.0,  6.0, 2.0,
        3.0,  6.0,  9.0, 1.0,
    };
    float resultCost = 5.0;
    AbstractBuffer<float> *M       = new AbstractBuffer<float>(3,4, matData);
    AbstractBuffer<int>   *mapping = new AbstractBuffer<int>(M->h, 1);
    assignOptimal(M, mapping);
    printAssignment(mapping, M->h, M->w);
    float cost = computeCost<float>(mapping, M);
    printf("Total cost: %lf expexted:%lf\n", cost, resultCost);
    CORE_ASSERT_DOUBLE_EQUAL(cost, resultCost, "Assignment was not optimal");
    delete M;
    delete mapping;
 /*   {
        static const int testSizeRows    = 4;
        static const int testSizeColumns = 3;

        printf("Starting test...\n");

        double matData[] =  {
            1.0,  2.0,  3.0,
            2.0,  4.0,  6.0,
            3.0,  6.0,  9.0,
            5.0,  2.0,  1.0
        };

        MATRIX M1 = {matData, testSizeRows, testSizeColumns };

        MATRIX *mapping = CreateMatrix(testSizeRows, 1);
        double result1Cost  = 8.0;
        double cost;
        assignmentoptimal(&M1, mapping, &cost);
        printAssignment(mapping->values, testSizeRows, testSizeColumns);
        printf("Total cost: %lf expexted:%lf\n", cost, result1Cost);
        DeleteMatrix(&mapping);
    } */
}

TEST(Assignment, testGeneric0)
{
    printf("Starting test...\n");

    float matData[] =  {
            1000.0,    8.0,  6.0,   12.0,    1.0,
              15.0,   12.0,  7.0, 1000.0,   10.0,
              10.0, 1000.0,  5.0,   14.0, 1000.0,
              12.0, 1000.0, 12.0,   16.0,   15.0,
              18.0,   17.0, 14.0, 1000.0,   13.0
        };

    float resultCost = 51.0;

    AbstractBuffer<float> *M       = new AbstractBuffer<float>(5,5, matData);
    AbstractBuffer<int>   *mapping = new AbstractBuffer<int>(M->h, 1);
    assignOptimal(M, mapping);
    printAssignment(mapping, M->h, M->w);
    float cost = computeCost<float>(mapping, M);
    printf("Total cost: %lf expexted:%lf\n", cost, resultCost);
    CORE_ASSERT_DOUBLE_EQUAL(cost, resultCost, "Assignment was not optimal");
    delete M;
    delete mapping;
}

TEST(Assignment, testGeneric1)
{
    printf("Starting test...\n");

    float matData[] =  {
         1.0,  2.0,  3.0,
         2.0,  4.0,  6.0,
         3.0,  6.0,  9.0
     };

    float resultCost = 10.0;
    AbstractBuffer<float> *M       = new AbstractBuffer<float>(3, 3, matData);
    AbstractBuffer<int>   *mapping = new AbstractBuffer<int>(M->h, 1);
    assignOptimal(M, mapping);
    printAssignment(mapping,  M->h, M->w);
    float cost = computeCost<float>(mapping, M);
    printf("Total cost: %lf expexted:%lf\n", cost, resultCost);
    CORE_ASSERT_DOUBLE_EQUAL(cost, resultCost, "Assignment was not optimal");
    delete M;
    delete mapping;
}

TEST(Assignment, testGeneric2)
{
    printf("Starting test...\n");

    float matData[] =  {
           20.0, 23.0, 36.0, 80.0, 26.0,
           28.0, 29.0, 25.0, 44.0, 73.0,
           21.0, 34.0, 37.0, 45.0, 38.0,
           33.0, 27.0, 40.0, 56.0, 31.0,
           39.0, 35.0, 60.0, 50.0, 48.0,

       };
    float resultCost = 149.0;

    AbstractBuffer<float> *M       = new AbstractBuffer<float>(5, 5, matData);
    AbstractBuffer<int>   *mapping = new AbstractBuffer<int>(M->h, 1);
    assignOptimal(M, mapping);
    printAssignment(mapping,  M->h, M->w);
    float cost = computeCost<float>(mapping, M);
    printf("Total cost: %lf expexted:%lf\n", cost, resultCost);
    CORE_ASSERT_DOUBLE_EQUAL(cost, resultCost, "Assignment was not optimal");
    delete M;
    delete mapping;
}

TEST(Assignment, testGeneric3)
{
    printf("Starting test...\n");

    float matData[] =  {
         62.0, 75.0, 80.0, 93.0, 95.0, 97.0,
         75.0, 80.0, 82.0, 85.0, 71.0, 97.0,
         80.0, 75.0, 81.0, 98.0, 90.0, 97.0,
         78.0, 82.0, 84.0, 80.0, 50.0, 98.0,
         90.0, 85.0, 85.0, 80.0, 85.0, 99.0,
         65.0, 75.0, 80.0, 75.0, 68.0, 96.0

     };

    float resultCost = 443.0;
    AbstractBuffer<float> *M       = new AbstractBuffer<float>(6, 6, matData);
    AbstractBuffer<int>   *mapping = new AbstractBuffer<int>(M->h, 1);
    assignOptimal(M, mapping);
    printAssignment(mapping, M->h, M->w);
    float cost = computeCost<float>(mapping, M);
    printf("Total cost: %lf expected:%lf\n", cost, resultCost);
    CORE_ASSERT_DOUBLE_EQUAL(cost, resultCost, "Assignment was not optimal");
    delete M;
    delete mapping;
}

#if 0
TEST(Assignment, doProfileGeneric)
{
    static const int testSizeRows    = 50;
    static const int testSizeColumns = 50;
    unsigned i, count;

    printf("Starting test...\n");

    for (count = 0; count < 640 * 480 / 50; count ++)
    {
        MATRIX *M1 = CreateMatrix(testSizeRows, testSizeColumns);
        for (i = 0; i < testSizeColumns * testSizeRows; i++)
        {
            M1->values[i] = rand() % 6000;
        }

        MATRIX *mapping = CreateMatrix(testSizeRows, 1);
        double result1Cost  = 443.0;
        double cost;
        assignmentoptimal(M1, mapping, &cost);
        //printAssignment(mapping->values, testSizeRows, testSizeColumns);
//      printf("Total cost: %lf expexted:%lf\n", cost, result1Cost);
        //CPPUNIT_ASSERT_MESSAGE("Wrong cost", IsEqualFloats(cost, result1Cost));*/
        DeleteMatdrix(&mapping);
    }
    printf("Finished")
}


#endif
