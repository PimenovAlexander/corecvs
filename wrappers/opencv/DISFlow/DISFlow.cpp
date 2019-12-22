//
// Created by vladimir on 21.11.2019.
//

#include "DISFlow.h"

int DISFlow::endFrame() {

    if (inCurr != NULL && inPrev != NULL)
    {
        delete_safe(opticalFlow);
        cv::Mat img = execute(convertToCVMat(inPrev), convertToCVMat(inCurr));
        saveFlowBuffer(img);
    }
    delete_safe (inPrev);
    inPrev = inCurr;
    return 0;
}

bool DISFlow::setParameters(std::string name, const corecvs::DynamicObject &newParams)
{
    if (name == "params" && inPrev)
    {
        newParams.copyTo(&params);
        if (params.sc_f() != 0 && params.sc_l() == 0) {
            defaultParams(false);
        }
        isSelectedParams = true;
    } else {
        return false;
    }
    return true;
}

std::map<std::string, corecvs::DynamicObject> DISFlow::getParameters() {

    cout << "DISFlow::getParameters(): called\n" << endl;
    std::map<std::string, corecvs::DynamicObject> toReturn;
    if (!isSelectedParams)
    {
        cout << "DISFlow::getParameters(): returning default parameters\n" << endl;
    } else {
        cout << "DISFlow::getParameters(): returning real parameters\n" << endl;
    }
    toReturn.emplace("params", corecvs::DynamicObject(&params));
    return toReturn;
}

cv::Mat DISFlow::convertToCVMat(corecvs::RGB24Buffer *buffer) {
    cv::Mat cv_rgb_image (buffer->h,buffer->w,CV_8UC3);
    int step = 3*buffer->w;

    for(int i = 0;i < cv_rgb_image.rows;i++)
    {
        int k = 0;
        for(int j = 0;j < cv_rgb_image.cols; k+=3,j++)
        {
            cv_rgb_image.data[step * i + k] = buffer->element(i, j).b();
            cv_rgb_image.data[step * i + k + 1] = buffer->element(i, j).g();
            cv_rgb_image.data[step * i + k + 2] = buffer->element(i, j).r();
        }
    }
    return  cv_rgb_image;
}

void DISFlow::saveFlowBuffer(cv::Mat &img) {
    previousFlow = &img;

    cv::Size szt = img.size();
    int width = szt.width, height = szt.height;
    int nc = img.channels();
    float tmp[nc];

    opticalFlow = new corecvs::FlowBuffer(inCurr->h, inCurr->w);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (nc==1) // depth
                tmp[0] = img.at<float>(y,x);
            else if (nc==2) // Optical Flow
            {
                tmp[0] = img.at<cv::Vec2f>(y,x)[0];
                tmp[1] = img.at<cv::Vec2f>(y,x)[1];
                corecvs::FlowElement elem(tmp[0], tmp[1]);
                opticalFlow->setElement(y, x, elem);
            }
            else if (nc==4) // Scene Flow
            {
                tmp[0] = img.at<cv::Vec4f>(y,x)[0];
                tmp[1] = img.at<cv::Vec4f>(y,x)[1];
                tmp[2] = img.at<cv::Vec4f>(y,x)[2];
                tmp[3] = img.at<cv::Vec4f>(y,x)[3];
            }
        }
    }

}

void DISFlow::ConstructImgPyramide(const cv::Mat & img_ao_fmat, cv::Mat * img_ao_fmat_pyr, cv::Mat * img_ao_dx_fmat_pyr, cv::Mat * img_ao_dy_fmat_pyr, const float ** img_ao_pyr, const float ** img_ao_dx_pyr, const float ** img_ao_dy_pyr, const int lv_f, const int lv_l, const int rpyrtype, const bool getgrad, const int imgpadding, const int padw, const int padh)
{
    for (int i=0; i<=lv_f; ++i)  // Construct image and gradient pyramides
    {
        if (i==0) // At finest scale: copy directly, for all other: downscale previous scale by .5
        {
            if (SELECTCHANNEL == 1 || SELECTCHANNEL == 3)  // use RGB or intensity image directly
                img_ao_fmat_pyr[i] = img_ao_fmat.clone();
            else if (SELECTCHANNEL == 2) { // use gradient magnitude image as input
                cv::Mat dx, dy, dx2, dy2, dmag;
                cv::Sobel(img_ao_fmat, dx, CV_32F, 1, 0, 3, 1 / 8.0, 0, cv::BORDER_DEFAULT);
                cv::Sobel(img_ao_fmat, dy, CV_32F, 0, 1, 3, 1 / 8.0, 0, cv::BORDER_DEFAULT);
                dx2 = dx.mul(dx);
                dy2 = dy.mul(dy);
                dmag = dx2 + dy2;
                cv::sqrt(dmag, dmag);
                img_ao_fmat_pyr[i] = dmag.clone();
            }
        }
        else
            cv::resize(img_ao_fmat_pyr[i-1], img_ao_fmat_pyr[i], cv::Size(), .5, .5, cv::INTER_LINEAR);

        img_ao_fmat_pyr[i].convertTo(img_ao_fmat_pyr[i], rpyrtype);

        if ( getgrad )
        {
            cv::Sobel( img_ao_fmat_pyr[i], img_ao_dx_fmat_pyr[i], CV_32F, 1, 0, 3, 1/8.0, 0, cv::BORDER_DEFAULT );
            cv::Sobel( img_ao_fmat_pyr[i], img_ao_dy_fmat_pyr[i], CV_32F, 0, 1, 3, 1/8.0, 0, cv::BORDER_DEFAULT );
            img_ao_dx_fmat_pyr[i].convertTo(img_ao_dx_fmat_pyr[i], CV_32F);
            img_ao_dy_fmat_pyr[i].convertTo(img_ao_dy_fmat_pyr[i], CV_32F);
        }
    }

    // pad images
    for (int i=0; i<=lv_f; ++i)  // Construct image and gradient pyramides
    {
        copyMakeBorder(img_ao_fmat_pyr[i],img_ao_fmat_pyr[i],imgpadding,imgpadding,imgpadding,imgpadding,cv::BORDER_REPLICATE);  // Replicate border for image padding
        img_ao_pyr[i] = (float*)img_ao_fmat_pyr[i].data;

        if ( getgrad )
        {
            copyMakeBorder(img_ao_dx_fmat_pyr[i],img_ao_dx_fmat_pyr[i],imgpadding,imgpadding,imgpadding,imgpadding,cv::BORDER_CONSTANT , 0); // Zero padding for gradients
            copyMakeBorder(img_ao_dy_fmat_pyr[i],img_ao_dy_fmat_pyr[i],imgpadding,imgpadding,imgpadding,imgpadding,cv::BORDER_CONSTANT , 0);

            img_ao_dx_pyr[i] = (float*)img_ao_dx_fmat_pyr[i].data;
            img_ao_dy_pyr[i] = (float*)img_ao_dy_fmat_pyr[i].data;
        }
    }
}

cv::Mat DISFlow::execute(cv::Mat img_ao_mat, cv::Mat img_bo_mat) {

    struct timeval tv_start_all, tv_end_all;
    gettimeofday(&tv_start_all, NULL);

    int rpyrtype, nochannels, incoltype;
    if (SELECTCHANNEL==1 || SELECTCHANNEL==2) // use Intensity or Gradient image
    {
        incoltype = CV_LOAD_IMAGE_GRAYSCALE;
        rpyrtype = CV_32FC1;
        nochannels = 1;
    }
    else {
        incoltype = CV_LOAD_IMAGE_COLOR;
        rpyrtype = CV_32FC3;
        nochannels = 3;
    }

    //cv::Mat img_ao_mat = cv::imread("/home/vladimir/Рабочий стол/clear/1.png", incoltype);   // Read the file
    //cv::Mat img_bo_mat = cv::imread("/home/vladimir/Рабочий стол/clear/2.png", incoltype);   // Read the file

    cv::Mat img_ao_fmat, img_bo_fmat;
    cv::Size sz = img_ao_mat.size();
    int width_org = sz.width;   // unpadded original image size
    int height_org = sz.height;  // unpadded original image size

    // *** Pad image such that width and height are restless divisible on all scales (except last)
    int padw=0, padh=0;
    int scfct = pow(2,params.sc_f()); // enforce restless division by this number on coarsest scale
    //if (hasinfile) scfct = pow(2,lv_f+1); // if initialization file is given, make sure that size is restless divisible by 2^(lv_f+1) !
    int div = sz.width % scfct;
    if (div>0) padw = scfct - div;
    div = sz.height % scfct;
    if (div>0) padh = scfct - div;
    if (padh>0 || padw>0)
    {
        copyMakeBorder(img_ao_mat,img_ao_mat,floor((float)padh/2.0f),ceil((float)padh/2.0f),floor((float)padw/2.0f),ceil((float)padw/2.0f),cv::BORDER_REPLICATE);
        copyMakeBorder(img_bo_mat,img_bo_mat,floor((float)padh/2.0f),ceil((float)padh/2.0f),floor((float)padw/2.0f),ceil((float)padw/2.0f),cv::BORDER_REPLICATE);
    }
    sz = img_ao_mat.size();  // padded image size, ensures divisibility by 2 on all scales (except last)

    // Timing, image loading
    if (params.verbosity() > 1)
    {
        gettimeofday(&tv_end_all, NULL);
        double tt = (tv_end_all.tv_sec-tv_start_all.tv_sec)*1000.0f + (tv_end_all.tv_usec-tv_start_all.tv_usec)/1000.0f;
        printf("TIME (Image loading     ) (ms): %3g\n", tt);
        gettimeofday(&tv_start_all, NULL);
    }


    //  *** Generate scale pyramides
    img_ao_mat.convertTo(img_ao_fmat, CV_32F); // convert to float
    img_bo_mat.convertTo(img_bo_fmat, CV_32F);

    const float* img_ao_pyr   [params.sc_f() + 1];
    const float* img_bo_pyr   [params.sc_f() + 1];
    const float* img_ao_dx_pyr[params.sc_f() + 1];
    const float* img_ao_dy_pyr[params.sc_f() + 1];
    const float* img_bo_dx_pyr[params.sc_f() + 1];
    const float* img_bo_dy_pyr[params.sc_f() + 1];

    cv::Mat img_ao_fmat_pyr   [params.sc_f() + 1];
    cv::Mat img_bo_fmat_pyr   [params.sc_f() + 1];
    cv::Mat img_ao_dx_fmat_pyr[params.sc_f() + 1];
    cv::Mat img_ao_dy_fmat_pyr[params.sc_f() + 1];
    cv::Mat img_bo_dx_fmat_pyr[params.sc_f() + 1];
    cv::Mat img_bo_dy_fmat_pyr[params.sc_f() + 1];

    ConstructImgPyramide(img_ao_fmat, img_ao_fmat_pyr, img_ao_dx_fmat_pyr, img_ao_dy_fmat_pyr, img_ao_pyr, img_ao_dx_pyr, img_ao_dy_pyr, params.sc_f(), params.sc_l(), rpyrtype, 1, params.patchsz(), padw, padh);
    ConstructImgPyramide(img_bo_fmat, img_bo_fmat_pyr, img_bo_dx_fmat_pyr, img_bo_dy_fmat_pyr, img_bo_pyr, img_bo_dx_pyr, img_bo_dy_pyr, params.sc_f(), params.sc_l(), rpyrtype, 1, params.patchsz(), padw, padh);

    // Timing, image gradients and pyramid
    if (params.verbosity() > 1)
    {
        gettimeofday(&tv_end_all, NULL);
        double tt = (tv_end_all.tv_sec-tv_start_all.tv_sec)*1000.0f + (tv_end_all.tv_usec-tv_start_all.tv_usec)/1000.0f;
        printf("TIME (Pyramide+Gradients) (ms): %3g\n", tt);
    }


    // Read Initial Truth flow (if available)
     float * initptr = nullptr;
     cv::Mat flowinit;
     if (previousFlow)
     {
         flowinit = *previousFlow;

         if (padh>0 || padw>0)
            copyMakeBorder(flowinit,flowinit,floor((float)padh/2.0f),ceil((float)padh/2.0f),floor((float)padw/2.0f),ceil((float)padw/2.0f),cv::BORDER_REPLICATE);

         // resizing to coarsest scale - 1, since the array is upsampled at .5 in the code
         float sc_fct = pow(2,-params.sc_f()-1);
         flowinit *= sc_fct;
         cv::resize(flowinit, flowinit, cv::Size(), sc_fct, sc_fct , cv::INTER_AREA);

         initptr = (float*)flowinit.data;
     }

    //  *** Run main optical flow / depth algorithm
    float sc_fct = pow(2,params.sc_l());
    cv::Mat flowout;

    if (SELECTMODE==1)
        flowout = cv::Mat(sz.height / sc_fct , sz.width / sc_fct, CV_32FC2); // Optical Flow
    else
        flowout = cv::Mat(sz.height / sc_fct , sz.width / sc_fct, CV_32FC1); // Depth

    OFC::OFClass ofc(img_ao_pyr, img_ao_dx_pyr, img_ao_dy_pyr,
                     img_bo_pyr, img_bo_dx_pyr, img_bo_dy_pyr,
                     params.patchsz(),  // extra image padding to avoid border violation check
                     (float*)flowout.data,   // pointer to n-band output float array
                     initptr,  // pointer to n-band input float array of size of first (coarsest) scale, pass as nullptr to disable
                     sz.width, sz.height,
                     params.sc_f(), params.sc_l(),
                     params.maxiter(), params.miniter(), params.mindprate(),
                     params.mindrrate(), params.minimgerr(),
                     params.patchsz() , params.poverl(),
                     params.usefbcon(), params.costfct(), nochannels, params.patnorm(),
                     params.usetvref(), params.tv_alpha(),
                     params.tv_gamma(), params.tv_delta(),
                     params.tv_innerit(), params.tv_solverit(), params.tv_sor(),
                     params.verbosity());

    if (params.verbosity() > 1)
        gettimeofday(&tv_start_all, NULL);



    // *** Resize to original scale, if not run to finest level
    if (params.sc_l() != 0)
    {
        flowout *= sc_fct;
        cv::resize(flowout, flowout, cv::Size(), sc_fct, sc_fct , cv::INTER_LINEAR);
    }

    // If image was padded, remove padding before saving to file
    flowout = flowout(cv::Rect((int)floor((float)padw/2.0f),(int)floor((float)padh/2.0f),width_org,height_org));

    return flowout;
}


int AutoFirstScaleSelect(int imgwidth, int fratio, int patchsize)
{
    return std::max(0,(int)std::floor(log2((2.0f*(float)imgwidth) / ((float)fratio * (float)patchsize))));
}

void DISFlow::defaultParams(bool isNeedDefineAll) {
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
        sel_oppoint = params.sc_f();


    int width_org = inPrev->w;   // unpadded original image size
    int height_org = inPrev->h;  // unpadded original image size

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

    params.setSc_f(lv_f);
    params.setSc_l(lv_l);
    params.setMaxiter(maxiter);
    params.setMiniter(miniter);
    params.setMindprate(mindprate);
    params.setMindrrate(mindrrate);
    params.setMinimgerr(minimgerr);
    params.setPatchsz(patchsz);
    params.setPoverl(poverl);
    params.setUsefbcon(usefbcon);
    params.setPatnorm(patnorm);
    params.setCostfct(costfct);
    params.setUsetvref(usetvref);
    params.setTv_alpha(tv_alpha);
    params.setTv_gamma(tv_gamma);
    params.setTv_delta(tv_delta);
    params.setTv_innerit(tv_innerit);
    params.setTv_solverit(tv_solverit);
    params.setTv_sor(tv_sor);
    params.setVerbosity(verbosity);

}
