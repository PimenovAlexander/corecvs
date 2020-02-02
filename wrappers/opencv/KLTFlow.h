/**
 * \file KLTFlow.h
 * \brief Add Comment Here
 *
 * \date Feb 23, 2011
 * \author alexander
 */

#ifndef KLTFLOW_H_
#define KLTFLOW_H_

#include <vector>

#include <core/stereointerface/processor6D.h>

#include "core/buffers/flow/flowBuffer.h"
#include "core/buffers/g12Buffer.h"
#include "core/buffers/flow/flowVector.h"
#include "generated/openCVKLTParameters.h"

using corecvs::FloatFlowVector;
using corecvs::G12Buffer;
using std::vector;

class KLTFlow
{
public:
    KLTFlow();
    virtual ~KLTFlow();

    static vector<FloatFlowVector> *getOpenCVKLT(G12Buffer *first,
            G12Buffer *second,
            double selectorQuality,
            double selectorDistance,
            int selectorSize,
            int useHarris,
            double harrisK,
            int kltSize,
            int mMaxCorners);

    static vector<FloatFlowVector> *getOpenCVKLT(
            G12Buffer *first,
            G12Buffer *second,
            const OpenCVKLTParameters &params);

};

class OpenCVFlowProcessor : public corecvs::Processor6D
{
    corecvs::Statistics *stats = NULL;

    corecvs::FlowBuffer *opticalFlow = NULL;

    corecvs::RGB24Buffer *inPrev  = NULL;
    corecvs::RGB24Buffer *inCurr  = NULL;


    OpenCVKLTParameters params;

    virtual int beginFrame() override;

    /** Completly reset internal data structures. parameters are left intact **/
    virtual int reset() override;


    /** Implemetation may allow you to reset some internal structures at will **/
    virtual int clean(int /*mask*/) override;

    /**
     * You are responisble to delete the frame sometime after endFrame() is called
     * Implemetation may (and will) store additinal views (pointers) to actual image data
     * for as long as implementation need them.
     *
     * see AbstractBuffer::createViewPtr<> for details on this mechanism
     *
     * To get consistent result don't modify frame data after the call to setFrame*
     **/
    virtual int setFrameG12  (corecvs::G12Buffer */*frame*/, int /*frameType*/) override
    {
        return 1;
    }

    virtual int setFrameRGB24(corecvs::RGB24Buffer *frame, int /*frameType*/) override
    {
        inCurr = new corecvs::RGB24Buffer(frame);
        return 0;
    }

    virtual int setDisparityBufferS16(corecvs::FlowBuffer   */*frame*/, int /*frameType*/) override
    {
        return 1;

    }

    /** sets statistics data. Implementation should support stats == NULL **/
    virtual int setStats(corecvs::Statistics *stats) override
    {
        this->stats = stats;
        return 0;
    }



    virtual int endFrame() override;


    virtual std::map<std::string, corecvs::DynamicObject> getParameters() override;
    virtual bool setParameters(std::string /*name*/, const corecvs::DynamicObject & /*param*/)  override {return true;}


    /** Oldstyle calls **/
    virtual int setParameteri  (int /*parameterName*/, int /*parameterValue*/) override {return 0;}

    /**
     * Setting this value to the combination of ResultNames flags requests output buffers.
     * This is a request only, if other combination parameters doesn't allow to produce particular outputs,
     * they may be returned as NULL
     **/
    virtual int requestResultsi(int /*parameterName*/) override {return 0;}
    virtual int getResultCaps() override  {return RESULT_FLOW | RESULT_FLOAT_FLOW;}

    /**
     * Methods below return the pointers to the internal data structures that are only valid
     * untill next beginFrame() is called. If you want them to persist - make a copy
     **/

    /* This method computes flow form current frame to previous */
    virtual corecvs::FlowBuffer *getFlow() override
    {
       return opticalFlow;
    }


    virtual corecvs::FlowBuffer *getStereo() override
    {
        return NULL;
    }

    virtual corecvs::CorrespondenceList *getFlowList() override
    {
        return NULL;
    }

    virtual corecvs::FloatFlowBuffer *getFloatFlow() override
    {
        return NULL;
    }

    virtual corecvs::FlowTracks *getFlowTracks()
    {
        return NULL;
    }


    virtual int getError(std::string * /*errorString*/) override {return 0;}
};


#if 0
class OpenCVProcessor6DFactory : public corecvs::Processor6DFactory
{
public:
   virtual corecvs::Processor6D *getProcessor() { return new OpenCVFlowProcessor(); }
   virtual std::string getName() {return "OpenCVFlowProcessor"; }
   virtual ~OpenCVProcessor6DFactory() {}
};
#endif

#endif /* KLTFLOW_H_ */
