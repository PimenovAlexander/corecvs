#ifndef CORECVS_PCAFLOWPROCESSOR_H
#define CORECVS_PCAFLOWPROCESSOR_H

//
// Created by elijah on 24.12.2019.
//


#include <opencv2/optflow/pcaflow.hpp>
#include "core/stereointerface/processor6D.h"
using namespace corecvs;
using namespace cv;

class PCAFlowProcessor : public ProcessorFlow {
    G8Buffer *prev  = NULL;
    G8Buffer *curr  = NULL;

public:
    FlowBuffer *opticalFlow = NULL;
    PCAFlowProcessor() ;

    int beginFrame() override;
    int endFrame() override;

    FlowBuffer *getFlow() override;
    int setFrameRGB24(RGB24Buffer *frame, int /*frameType*/) override;
    int setFrameG12(G12Buffer *frame, int frameType) override;


    /* TODO: To be implemented */
    /** Completly reset internal data structures. parameters are left intact **/
    virtual int reset() {return 0;}

    /** sets statistics data. Implementation should support stats == NULL **/
    virtual int setStats(Statistics *stats) override {return 1;}

    virtual std::map<std::string, DynamicObject> getParameters() override { return std::map<std::string, DynamicObject>(); }
    virtual bool setParameters(std::string name, const DynamicObject &param) {return false; }


    virtual CorrespondenceList *getFlowList() override {return  NULL;}
    virtual FloatFlowBuffer    *getFloatFlow() override {return  NULL;}
    virtual FlowTracks         *getFlowTracks() override {return  NULL;}


    virtual int getError(std::string *errorString) override {return  0;}


};

#endif //CORECVS_PCAFLOWPROCESSOR_H
