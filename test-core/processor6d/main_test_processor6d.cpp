/**
 * \file main_test_processor6d.cpp
 * \brief This is the main file for the test processor6d 
 *
 * \date июл 18, 2018
 * \author dina
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "core/stereointerface/processor6D.h"
#include "core/stereointerface/dummyFlowProcessor.h"

#include "core/utils/global.h"


using namespace std;
using namespace corecvs;


TEST(processor6d, testprocessor6d)
{
    RGB24Buffer in1(100,100);
    RGB24Buffer in2(100,100);

    Processor6D *proc = new DummyFlowProcessor;
    proc->requestResultsi(Processor6D::RESULT_FLOW);
    proc->beginFrame();
    proc->setFrameRGB24(Processor6D::FRAME_LEFT_ID, &in1);
    proc->endFrame();
    proc->beginFrame();
    proc->setFrameRGB24(Processor6D::FRAME_LEFT_ID, &in2);
    proc->endFrame();
    FlowBuffer *flow = proc->getFlow();

    delete_safe(proc);


}
