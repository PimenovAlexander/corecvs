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

cv::Mat DISFlow::convertToCVMat(corecvs::RGB24Buffer *buffer) {
    cv::Mat cv_rgb_image (buffer->h,buffer->w,CV_8UC3);
    int step = sizeof(uint8_t) * buffer->w;

    for(int i = 0,k=0;i < cv_rgb_image.rows;i++)
    {
        for(int j = 0;j < cv_rgb_image.step; j+=3,k++)
        {
            cv_rgb_image.data[step * i + j] = buffer->data[k].b();
            cv_rgb_image.data[step * i + j + 1] = buffer->data[k].g();
            cv_rgb_image.data[step * i + j + 2] = buffer->data[k].r();
        }
    }
    cv::Mat newImg;
    newImg.Mat::convertTo(cv_rgb_image, CV_32FC3, 1/255.0);
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
            // FIXME:: Is need to implement?
            if (nc==1) // depth
                tmp[0] = img.at<float>(y,x);
            else if (nc==2) // Optical Flow
            {
                tmp[0] = img.at<cv::Vec2f>(y,x)[0];
                tmp[1] = img.at<cv::Vec2f>(y,x)[1];
                // TODO: here create new flow buffer
                opticalFlow->element(tmp[0], tmp[1]);
            }
            else if (nc==4) // Scene Flow
            {
                // FIXME:: Is need to implement?
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
if (SELECTCHANNEL == 1 | SELECTCHANNEL == 3)  // use RGB or intensity image directly
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
    if (SELECTCHANNEL==1 | SELECTCHANNEL==2) // use Intensity or Gradient image
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
    cv::Mat img_ao_fmat, img_bo_fmat;
    cv::Size sz = img_ao_mat.size();
    int width_org = sz.width;   // unpadded original image size
    int height_org = sz.height;  // unpadded original image size

    // *** Pad image such that width and height are restless divisible on all scales (except last)
    int padw=0, padh=0;
    int scfct = pow(2,params->getParams().sc_f); // enforce restless division by this number on coarsest scale
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
    if (params->getParams().verbosity > 1)
    {
        gettimeofday(&tv_end_all, NULL);
        double tt = (tv_end_all.tv_sec-tv_start_all.tv_sec)*1000.0f + (tv_end_all.tv_usec-tv_start_all.tv_usec)/1000.0f;
        printf("TIME (Image loading     ) (ms): %3g\n", tt);
        gettimeofday(&tv_start_all, NULL);
    }


    //  *** Generate scale pyramides
    img_ao_mat.convertTo(img_ao_fmat, CV_32F); // convert to float
    img_bo_mat.convertTo(img_bo_fmat, CV_32F);

    const float* img_ao_pyr[params->getParams().sc_f+1];
    const float* img_bo_pyr[params->getParams().sc_f+1];
    const float* img_ao_dx_pyr[params->getParams().sc_f+1];
    const float* img_ao_dy_pyr[params->getParams().sc_f+1];
    const float* img_bo_dx_pyr[params->getParams().sc_f+1];
    const float* img_bo_dy_pyr[params->getParams().sc_f+1];

    cv::Mat img_ao_fmat_pyr[params->getParams().sc_f+1];
    cv::Mat img_bo_fmat_pyr[params->getParams().sc_f+1];
    cv::Mat img_ao_dx_fmat_pyr[params->getParams().sc_f+1];
    cv::Mat img_ao_dy_fmat_pyr[params->getParams().sc_f+1];
    cv::Mat img_bo_dx_fmat_pyr[params->getParams().sc_f+1];
    cv::Mat img_bo_dy_fmat_pyr[params->getParams().sc_f+1];

    ConstructImgPyramide(img_ao_fmat, img_ao_fmat_pyr, img_ao_dx_fmat_pyr, img_ao_dy_fmat_pyr, img_ao_pyr, img_ao_dx_pyr, img_ao_dy_pyr, params->getParams().sc_f, params->getParams().sc_l, rpyrtype, 1, params->getParams().p_samp_s, padw, padh);
    ConstructImgPyramide(img_bo_fmat, img_bo_fmat_pyr, img_bo_dx_fmat_pyr, img_bo_dy_fmat_pyr, img_bo_pyr, img_bo_dx_pyr, img_bo_dy_pyr, params->getParams().sc_f, params->getParams().sc_l, rpyrtype, 1, params->getParams().p_samp_s, padw, padh);

    // Timing, image gradients and pyramid
    if (params->getParams().verbosity > 1)
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
         initptr = (float*)flowinit.data;
     }

    //  *** Run main optical flow / depth algorithm
    float sc_fct = pow(2,params->getParams().sc_l);
    cv::Mat flowout;

    if (SELECTMODE==1)
        flowout = cv::Mat(sz.height / sc_fct , sz.width / sc_fct, CV_32FC2); // Optical Flow
    else
        flowout = cv::Mat(sz.height / sc_fct , sz.width / sc_fct, CV_32FC1); // Depth

    OFC::OFClass ofc(img_ao_pyr, img_ao_dx_pyr, img_ao_dy_pyr,
                     img_bo_pyr, img_bo_dx_pyr, img_bo_dy_pyr,
                     params->getParams().p_samp_s,  // extra image padding to avoid border violation check
                     (float*)flowout.data,   // pointer to n-band output float array
                     initptr,  // pointer to n-band input float array of size of first (coarsest) scale, pass as nullptr to disable
                     sz.width, sz.height,
                     params->getParams().sc_f, params->getParams().sc_l,
                     params->getParams().max_iter, params->getParams().min_iter, params->getParams().dp_thresh,
                     params->getParams().dr_thresh, params->getParams().res_thresh,
                     params->getParams().p_samp_s, params->getParams().p_overlap,
                     params->getParams().usefbcon, params->getParams().costfct, nochannels, params->getParams().patnorm,
                     params->getParams().usetvref, params->getParams().tv_alpha,
                     params->getParams().tv_gamma, params->getParams().tv_delta,
                     params->getParams().tv_innerit, params->getParams().tv_solverit, params->getParams().tv_sor,
                     params->getParams().verbosity);

    if (params->getParams().verbosity > 1) gettimeofday(&tv_start_all, NULL);



    // *** Resize to original scale, if not run to finest level
    if (params->getParams().sc_l != 0)
    {
        flowout *= sc_fct;
        cv::resize(flowout, flowout, cv::Size(), sc_fct, sc_fct , cv::INTER_LINEAR);
    }

    // If image was padded, remove padding before saving to file
    flowout = flowout(cv::Rect((int)floor((float)padw/2.0f),(int)floor((float)padh/2.0f),width_org,height_org));

    return flowout;
}