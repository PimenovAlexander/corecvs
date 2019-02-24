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

#include "core/utils/global.h"


using namespace std;
using namespace corecvs;

class DummyFlowProcessor : public Processor6D
{
    Statistics *stats = NULL;

    FlowBuffer *opticalFlow = NULL;

    RGB24Buffer *inPrev  = NULL;
    RGB24Buffer *inCurr  = NULL;




    virtual int beginFrame() {return 0;}

    /** Completly reset internal data structures. parameters are left intact **/
    virtual int reset()
    {
        inPrev = NULL;
        inCurr = NULL;
        delete_safe(opticalFlow);
        return 0;
    }


    /** Implemetation may allow you to reset some internal structures at will **/
    virtual int clean(int mask) {
        return 0;
    }

    /**
     * You are responisble to delete the frame sometime after endFrame() is called
     * Implemetation may (and will) store additinal views (pointers) to actual image data
     * for as long as implementation need them.
     *
     * see AbstractBuffer::createViewPtr<> for details on this mechanism
     *
     * To get consistent result don't modify frame data after the call to setFrame*
     **/
    virtual int setFrameG12  (FrameNames frameType, G12Buffer   *frame)
    {
        return 1;
    }

    virtual int setFrameRGB24(FrameNames frameType, RGB24Buffer *frame)
    {
        inCurr = frame;
    }

    virtual int setDisparityBufferS16(FrameNames frameType, FlowBuffer *frame)
    {
        return 1;

    }

    /** sets statistics data. Implementation should support stats == NULL **/
    virtual int setStats(Statistics *stats)
    {
        this->stats = stats;
    }



    virtual int endFrame()
    {

        if (inCurr != NULL && inPrev != NULL)
        {
            delete_safe(opticalFlow);
            opticalFlow = new FlowBuffer(inCurr->h, inCurr->w);
        }
        inPrev = inCurr;
    }


    virtual std::map<std::string, DynamicObject> getParameters() { return std::map<std::string, DynamicObject>();}
    virtual bool setParameters(std::string name, const DynamicObject &param) {return true;}


    /** Oldstyle calls **/
    virtual int setParameteri  (int /*parameterName*/, int /*parameterValue*/) {return 0;}
    virtual int requestResultsi(int /*parameterName*/) {return 0;}

    /**
     * Methods below return the pointers to the internal data structures that are only valid
     * untill next beginFrame() is called. If you want them to persist - make a copy
     **/

    /* This method computes flow form current frame to previous */
    virtual FlowBuffer *getFlow(){
       return opticalFlow;
    }


    virtual FlowBuffer *getStereo()
    {
        return NULL;
    }

    virtual CorrespondenceList *getFlowList()
    {
        return NULL;
    }


    virtual int getError(std::string * /*errorString*/) {return 0;}
};


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
