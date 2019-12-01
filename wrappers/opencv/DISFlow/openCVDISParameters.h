#ifndef CORECVS_OPENCVDISPARAMETERS_H
#define CORECVS_OPENCVDISPARAMETERS_H
//
// Created by vladimir on 21.11.2019.
//

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sys/time.h>

#include <string>
#include <core/buffers/rgb24/rgb24Buffer.h>
#include <generated/disFlowParameters.h>

typedef struct {
    int sc_f;             // first (coarsest) scale
} optScaleOnly;

enum algorithm {OF_INT, OF_RGB};

class DIS_Args {

private:
    algorithm name;

public: /* Well, why is it private, how should I access it? Why is it a pointer at all? */
    DisFlowParameters *params;
private:
    corecvs::RGB24Buffer *buffer1;
    int SELECTMODE;
    int SELECTCHANNEL;

public:
    DIS_Args(algorithm name, corecvs::RGB24Buffer *frame, DisFlowParameters *params1) {
        setNameParams(name);
        params = params1;
        buffer1 = frame;
    }

    DIS_Args(algorithm name, corecvs::RGB24Buffer *frame, optScaleOnly *params1) {
        setNameParams(name);
        params->setSc_f(params1->sc_f);
        defaultParams(false);
        buffer1 = frame;
    }

    DIS_Args(algorithm name, corecvs::RGB24Buffer *frame) {
        setNameParams(name);
        defaultParams(true);
        buffer1 = frame;
    }

public:
    int getSelMode() const
    {
        return SELECTMODE;
    }

    int getSelChannel() const
    {
        return SELECTCHANNEL;
    }

    DisFlowParameters getParams() const
    {
        return *params;
    }

    corecvs::RGB24Buffer *getBuffer() const {
        return buffer1;
    }

private:

    void setNameParams(algorithm name) {
        switch (name) {
            case OF_INT:
                SELECTMODE = 1;
                SELECTCHANNEL = 1;
                break;
            case OF_RGB:
                SELECTMODE = 1;
                SELECTCHANNEL = 3;
                break;
        }
    }

    int AutoFirstScaleSelect(int imgwidth, int fratio, int patchsize)
    {
        return std::max(0,(int)std::floor(log2((2.0f*(float)imgwidth) / ((float)fratio * (float)patchsize))));
    }

    void defaultParams(bool isNeedDefineAll) {
        int lv_f, lv_l, maxiter, miniter, patchsz, patnorm, costfct, tv_innerit, tv_solverit, verbosity;
        float mindprate, mindrrate, minimgerr, poverl, tv_alpha, tv_gamma, tv_delta, tv_sor;
        bool usefbcon, usetvref;

        mindprate = 0.05; mindrrate = 0.95; minimgerr = 0.0;
        usefbcon = 0; patnorm = 1; costfct = 0;
        tv_alpha = 10.0; tv_gamma = 10.0; tv_delta = 5.0;
        tv_innerit = 1; tv_solverit = 3; tv_sor = 1.6;
        verbosity = 2; // Default: Plot detailed timings

        int fratio = 5; // For automatic selection of coarsest scale: 1/fratio * width = maximum expected motion magnitude in image. Set lower to restrict search space.

        int sel_oppoint = 2; // Default operating point
        if (!isNeedDefineAll)         // Use provided operating point
            sel_oppoint = params->sc_f();


        int width_org = buffer1->w;   // unpadded original image size
        int height_org = buffer1->h;  // unpadded original image size

        switch (sel_oppoint)
        {
            case 1:
                patchsz = 8; poverl = 0.3;
                lv_f = AutoFirstScaleSelect(width_org, fratio, patchsz);
                lv_l = std::max(lv_f-2,0); maxiter = 16; miniter = 16;
                usetvref = 0;
                break;
            case 3:
                patchsz = 12; poverl = 0.75;
                lv_f = AutoFirstScaleSelect(width_org, fratio, patchsz);
                lv_l = std::max(lv_f-4,0); maxiter = 16; miniter = 16;
                usetvref = 1;
                break;
            case 4:
                patchsz = 12; poverl = 0.75;
                lv_f = AutoFirstScaleSelect(width_org, fratio, patchsz);
                lv_l = std::max(lv_f-5,0); maxiter = 128; miniter = 128;
                usetvref = 1;
                break;
            case 2:
            default:
                patchsz = 8; poverl = 0.4;
                lv_f = AutoFirstScaleSelect(width_org, fratio, patchsz);
                lv_l = std::max(lv_f-2,0); maxiter = 12; miniter = 12;
                usetvref = 1;
                break;

        }

        params->setSc_f(lv_f);
        params->setSc_l(lv_l);
        params->setMaxiter(maxiter);
        params->setMiniter(miniter);
        params->setMindprate(mindprate);
        params->setMindrrate(mindrrate);
        params->setMinimgerr(minimgerr);
        params->setPatchsz(patchsz);
        params->setPoverl(poverl);
        params->setUsefbcon(usefbcon);
        params->setPatnorm(patnorm);
        params->setCostfct(costfct);
        params->setUsetvref(usetvref);
        params->setTv_alpha(tv_alpha);
        params->setTv_gamma(tv_gamma);
        params->setTv_delta(tv_delta);
        params->setTv_innerit(tv_innerit);
        params->setTv_solverit(tv_solverit);
        params->setTv_sor(tv_sor);
        params->setVerbosity(verbosity);

    }
};

#endif //CORECVS_OPENCVDISPARAMETERS_H
