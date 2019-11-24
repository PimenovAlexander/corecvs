//
// Created by vladimir on 20.11.2019.
//

#ifndef CORECVS_DISFLOW_H
#define CORECVS_DISFLOW_H

#include "generated/openCVDISParameters.h"
#include <core/stereointerface/processor6D.h>

class DISFlow : corecvs::ProcessorFlow {
    corecvs::Statistics *stats = NULL;

    corecvs::FlowBuffer *opticalFlow = NULL;

    cv::Mat *previousFlow = nullptr;

    corecvs::RGB24Buffer *inPrev  = NULL;
    corecvs::RGB24Buffer *inCurr  = NULL;

    DIS_Args *params;

    int SELECTCHANNEL;
    int SELECTMODE;

    DISFlow(DIS_Args *par) {
        params = par;
        SELECTCHANNEL = par->getSelChannel();
        SELECTMODE = par->getSelMode();
    }

    virtual int beginFrame() {return 0;}

    virtual int setFrameRGB24(corecvs::RGB24Buffer *frame, int /*frameType*/) override
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