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

using corecvs::FloatFlowVector;
using corecvs::G12Buffer;
using std::vector;

class KLTFlow
{
public:
    KLTFlow();
    virtual ~KLTFlow();

    static vector<FloatFlowVector> *getOpenCVKLT(
            G12Buffer *first,
            G12Buffer *second,
            double selectorQuality,
            double selectorDistance,
            int selectorSize,
            int useHarris,
            double harrisK,
            int kltSize);
};

class OpenCVFlowProcessor : public corecvs::Processor6D
{
    corecvs::Statistics *stats = NULL;

    corecvs::FlowBuffer *opticalFlow = NULL;

    corecvs::RGB24Buffer *inPrev  = NULL;
    corecvs::RGB24Buffer *inCurr  = NULL;




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
    virtual int setFrameG12  (FrameNames frameType, corecvs::G12Buffer   *frame)
    {
        return 1;
    }

    virtual int setFrameRGB24(FrameNames frameType, corecvs::RGB24Buffer *frame)
    {
        inCurr = frame;
    }

    virtual int setDisparityBufferS16(FrameNames frameType, corecvs::FlowBuffer *frame)
    {
        return 1;

    }

    /** sets statistics data. Implementation should support stats == NULL **/
    virtual int setStats(corecvs::Statistics *stats)
    {
        this->stats = stats;
    }



    virtual int endFrame();


    virtual std::map<std::string, corecvs::DynamicObject> getParameters() { return std::map<std::string, corecvs::DynamicObject>();}
    virtual bool setParameters(std::string name, const corecvs::DynamicObject &param) {return true;}


    /** Oldstyle calls **/
    virtual int setParameteri(int parameterName, int parameterValue) {return 0;}
    virtual int requestResultsi(int parameterName) {return 0;}

    /**
     * Methods below return the pointers to the internal data structures that are only valid
     * untill next beginFrame() is called. If you want them to persist - make a copy
     **/

    /* This method computes flow form current frame to previous */
    virtual corecvs::FlowBuffer *getFlow(){
       return opticalFlow;
    }


    virtual corecvs::FlowBuffer *getStereo()
    {
        return NULL;
    }

    virtual corecvs::CorrespondenceList *getFlowList()
    {
        return NULL;
    }


    virtual int getError(std::string *errorString) {return 0;}
};


class OpenCVProcessor6DFactory : public corecvs::Processor6DFactory
{
public:
   virtual corecvs::Processor6D *getProcessor() { return new OpenCVFlowProcessor(); }
   virtual std::string getName() {return "OpenCVFlowProcessor"; }
   virtual ~OpenCVProcessor6DFactory() {}
};

#endif /* KLTFLOW_H_ */
