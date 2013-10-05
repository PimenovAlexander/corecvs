/**
 * \file main_test_filter_blocks.cpp
 * \brief This is the main file for the test filter_blocks 
 *
 * \date Nov 11, 2012
 * \author alexander
 *
 * \ingroup autotest  
 */

#ifndef ASSERTS
#define ASSERTS
#endif

#include <iostream>

#include "global.h"

#include "operationFilter.h"
#include "sobelFilter.h"
#include "txtFilter.h"
#include "bmpLoader.h"
#include "filterGraph.h"

using namespace std;
using corecvs::FilterGraph;
using corecvs::FilterBlock;
using corecvs::SobelFilter;
using corecvs::GeneratedFilterBlock;

/**
 *  Creating test graph
 *
 *   A       B
 *  / \     /
 * C   D   E     I
 *  \ / \ / \
 *   F   G   H
 *
 **/
void testFilterBlocks()
{
    FilterGraph graph;

    FilterBlock* H = graph.addBlock(new FilterBlock("H"));
    FilterBlock* A = graph.addBlock(new FilterBlock("A"));
    FilterBlock* G = graph.addBlock(new FilterBlock("G"));
    FilterBlock* C = graph.addBlock(new FilterBlock("C"));
    FilterBlock* B = graph.addBlock(new FilterBlock("B"));
    FilterBlock* F = graph.addBlock(new FilterBlock("F"));
    FilterBlock* D = graph.addBlock(new FilterBlock("D"));
    FilterBlock* E = graph.addBlock(new FilterBlock("E"));
    FilterBlock* I = graph.addBlock(new FilterBlock("I"));

    ASSERT_TRUE(graph.addConnection(D, "0", G, "0"), "Failed connection D to G");
    ASSERT_TRUE(graph.addConnection(E, "0", G, "1"), "Failed connection E to G");
    ASSERT_TRUE(graph.addConnection(E, "1", H, "0"), "Failed connection E to H");
    ASSERT_TRUE(graph.addConnection(A, "0", D, "0"), "Failed connection A to D");
    ASSERT_TRUE(graph.addConnection(B, "0", E, "0"), "Failed connection B to E");
    ASSERT_TRUE(graph.addConnection(C, "0", F, "0"), "Failed connection C to F");

    ASSERT_FALSE(graph.addConnection(D, "0", F, "0"), "Connected D to busy pin of F");

    ASSERT_TRUE(graph.addConnection(D, "0", F, "1"), "Failed connection D to F");
    ASSERT_TRUE(graph.addConnection(A, "1", C, "0"), "Failed connection A to C");

    bool passed = true;
    graph.print();
    passed = passed && graph.topologicSort();
    if (passed) {
        graph.print();
    } else {
        printf("Sorting Failed\n");
        return;
    }

    graph.execute();
}

/**
 *  Creating test graph
 *
 *   A       B
 *  / \     /
 * C   D   E     I
 *  \ / \ / \
 *   F   G   H
 *
 **/
void testFilterBlocksGenerated()
{
    FilterGraph graph;

    FilterBlock* H = graph.addBlock(new OperationFilter("H"));
    FilterBlock* A = graph.addBlock(new OperationFilter("A"));
    FilterBlock* G = graph.addBlock(new OperationFilter("G"));
    FilterBlock* C = graph.addBlock(new OperationFilter("C"));
    FilterBlock* B = graph.addBlock(new TxtFilter("B"));
    FilterBlock* F = graph.addBlock(new OperationFilter("F"));
    FilterBlock* D = graph.addBlock(new OperationFilter("D"));
    FilterBlock* E = graph.addBlock(new OperationFilter("E"));
    FilterBlock* I = graph.addBlock(new OperationFilter("I"));

    ASSERT_TRUE(graph.connect("D", "0", "G", "0"), "Failed connection D to G");
    ASSERT_TRUE(graph.connect("E", "0", "G", "1"), "Failed connection E to G");
    ASSERT_TRUE(graph.connect("E", "0", "H", "0"), "Failed connection E to H");
    ASSERT_TRUE(graph.connect("A", "0", "D", "0"), "Failed connection A to D");
    ASSERT_TRUE(graph.connect("B", "0", "E", "0"), "Failed connection B to E");

    ASSERT_FALSE(E->inPinByName("0")->setPin(E->inPinByName("0")->takeFrom), "Failed connection B to E");

    ASSERT_TRUE(graph.connect("C", "0", "F", "0"), "Failed connection C to F");

    ASSERT_FALSE(graph.connect("D", "0", "F", "0"), "Connected D to busy pin of F");

    ASSERT_TRUE(graph.connect("D", "0", "F", "1"), "Failed connection D to F");
    ASSERT_TRUE(graph.connect("A", "0", "C", "0"), "Failed connection A to C");

    bool passed = true;
    printf("Before topologic sort\n");
    graph.print();
    passed = passed && graph.topologicSort();
    if (passed) {
        printf("After topologic sort\n");
        graph.print();
    } else {
        printf("Sorting Failed\n");
        return;
    }
    graph.execute();
}


//void testOperationBlock (void)
//{
//    G12Buffer *A = new G12Buffer(100, 100);
//    A->element(10,10) = G12Buffer::BUFFER_MAX_VALUE;

//    G12Buffer *B = new G12Buffer(100, 100);
//    B->element(20,20) = G12Buffer::BUFFER_MAX_VALUE;

//    G12Buffer *C = new G12Buffer(100, 100);
//    C->element(30,30) = G12Buffer::BUFFER_MAX_VALUE;

//    G12Buffer *D = new G12Buffer(100, 100);
//    D->element(40,40) = G12Buffer::BUFFER_MAX_VALUE;

//    FilterGraph graph;

//    OperationParameters params(Operation::ADD);

//    FilterBlock *ab = new OperationFilter("Add A+B");
//    ab->setParameters((const void *)&params);
//    ab->setPin(0, A);

//    ab->setPin(1, B);
//    graph.addBlock(ab);

//    FilterBlock *cd = new OperationFilter("Add C+D");
//    cd->setParameters((const void *)&params);
//    cd->setPin(0, C);
//    cd->setPin(1, D);
//    graph.addBlock(cd);

//    FilterBlock *abcd = new OperationFilter("Add (A+B)+(C+D)");
//    abcd->setParameters((const void *)&params);
//    graph.addBlock(abcd);
//    graph.connect(ab, 0, abcd, 0);
//    graph.connect(cd, 0, abcd, 1);

//    graph.topologicSort();
//    graph.execute();
//    BMPLoader().save("graph.bmp", (G12Buffer *)abcd->getPin(0));
//}

int main (int /*argC*/, char ** /*argV*/)
{
    //testFilterBlocks();
    testFilterBlocksGenerated();
    //testOperationBlock();
    cout << "PASSED" << endl;
    return 0;
}
