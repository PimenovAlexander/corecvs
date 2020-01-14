#ifndef DUMMY_FLOW_PROCESSOR_H
#define DUMMY_FLOW_PROCESSOR_H

#include "core/stats/calculationStats.h"
#include "core/reflection/dynamicObject.h"
#include "core/stereointerface/processor6D.h"

namespace corecvs {

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
    virtual int clean(int /*mask*/) {
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
    virtual int setFrameG12  (G12Buffer   */*frame*/, int /*frameType*/)
    {
        return 1;
    }

    virtual int setFrameRGB24(RGB24Buffer *frame, int /*frameType*/)
    {
        inCurr = frame;
        return 0;
    }

    virtual int setDisparityBufferS16(FlowBuffer */*frame*/, int /*frameType*/)
    {
        return 1;
    }

    /** sets statistics data. Implementation should support stats == NULL **/
    virtual int setStats(Statistics *stats)
    {
        this->stats = stats;
        return 0;
    }

    virtual int endFrame()
    {

        if (inCurr != NULL && inPrev != NULL)
        {
            delete_safe(opticalFlow);
            opticalFlow = new FlowBuffer(inCurr->h, inCurr->w);
        }
        inPrev = inCurr;
        return 0;
    }


    virtual std::map<std::string, DynamicObject> getParameters();
    virtual bool setParameters(std::string name, const DynamicObject &param);


    /** Oldstyle calls **/
    virtual int setParameteri(int /*parameterName*/, int /*parameterValue*/) {return 0;}

    /**
     * Setting this value to the combination of ResultNames flags
     **/
    virtual int requestResultsi(int /*parameterName*/) {return 0;}

    virtual int getResultCaps() {return RESULT_FLOW | RESULT_FLOAT_FLOW;}

    /**
     * Methods below return the pointers to the internal data structures that are only valid
     * untill next beginFrame() is called. If you want them to persist - make a copy
     **/

    /* This method computes flow form current frame to previous */
    virtual FlowBuffer *getFlow() override
    {
       return opticalFlow;
    }

    virtual FloatFlowBuffer *getFloatFlow() override
    {
       return new FloatFlowBuffer(opticalFlow); /* leaks? */
    }

    virtual FlowBuffer *getStereo()  override
    {
        return NULL;
    }

    virtual CorrespondenceList *getFlowList() override
    {
        return NULL;
    }

    virtual FlowTracks *getFlowTracks() override
    {
        return NULL;
    }


    virtual int getError(std::string */*errorString*/) {return 0;}
};

class DummyFlowImplFactory : public corecvs::Processor6DFactory {
public:

    DummyFlowImplFactory();

    virtual corecvs::Processor6D *getProcessor() override;
    virtual std::string getName() override { return "DummyFlow";}

};


} // namespace corecvs

#endif // DUMMY_FLOW_PROCESSOR_H
