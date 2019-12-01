#ifndef CORECVS_DISFLOW_H
#define CORECVS_DISFLOW_H
//
// Created by vladimir on 20.11.2019.
//


#include "generated/disFlowParameters.h"
#include "DISFlow/openCVDISParameters.h"
#include "siblings/OF_DIS/oflow.h"
#include <core/stereointerface/processor6D.h>

class DISFlow : public corecvs::ProcessorFlow {
public:
    corecvs::Statistics *stats = NULL;

    corecvs::FlowBuffer *opticalFlow = NULL;

    cv::Mat *previousFlow;

    corecvs::RGB24Buffer *inPrev  = NULL;
    corecvs::RGB24Buffer *inCurr  = NULL;

    DIS_Args *params = nullptr;

    int SELECTCHANNEL;
    int SELECTMODE;

    DISFlow(DIS_Args *par) {
        params = par;
        SELECTCHANNEL = par->getSelChannel();
        SELECTMODE = par->getSelMode();
        inPrev = par->getBuffer();
    }

    /*Oh well, API assumese there would be default constructor. Adding a dummy one*/
    DISFlow() {

    }

    /* No cleanup is needed? Some probably should be*/
    virtual ~DISFlow() {

    }

    virtual int beginFrame() {return 0;}

    virtual int setFrameRGB24(corecvs::RGB24Buffer *frame, int frameType = FRAME_DEFAULT) override
    {
        inCurr = new corecvs::RGB24Buffer(frame);
        return 0;
    }

    virtual int reset()
    {
        inPrev = NULL;
        inCurr = NULL;
        delete_safe(opticalFlow);
        return 0;
    }

    virtual int endFrame();

    virtual corecvs::FlowBuffer *getFlow() override
    {
        return opticalFlow;
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

    virtual int setFrameG12  (corecvs::G12Buffer   */*frame*/, int /*frameType*/) override
    {
        return 1;
    }


    virtual int setStats(corecvs::Statistics *stats) override
    {
        this->stats = stats;
        return 0;
    }

    virtual bool setParameters(std::string /*name*/, const corecvs::DynamicObject & /*param*/)  override;

    virtual std::map<std::string, corecvs::DynamicObject> getParameters() override;

private:
    cv::Mat convertToCVMat(corecvs::RGB24Buffer *buffer);

    void saveFlowBuffer(cv::Mat &img);

    cv::Mat execute(cv::Mat img_ao_fmat, cv::Mat img_bo_fmat);

    void ConstructImgPyramide(const cv::Mat &img_ao_fmat, cv::Mat *img_ao_fmat_pyr, cv::Mat *img_ao_dx_fmat_pyr,
                              cv::Mat *img_ao_dy_fmat_pyr, const float **img_ao_pyr, const float **img_ao_dx_pyr,
                              const float **img_ao_dy_pyr, const int lv_f, const int lv_l, const int rpyrtype,
                              const bool getgrad, const int imgpadding, const int padw, const int padh);
};


#endif //CORECVS_DISFLOW_H
