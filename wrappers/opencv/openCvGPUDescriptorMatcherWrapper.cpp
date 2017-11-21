#include "openCvGPUDescriptorMatcherWrapper.h"
#include "openCvKeyPointsWrapper.h"

#include "core/utils/global.h"

#ifndef WITH_OPENCV_3x

#include <opencv2/features2d/features2d.hpp>    // cv::DescriptorMatcher
#include <opencv2/gpu/gpu.hpp>           // cv::gpu::BruteForceMatcher_GPU_base
#include <opencv2/ocl/ocl.hpp>        // cv::ocl::BruteForceMatcher_OCL_base

using namespace cv::gpu;
using namespace cv::ocl;

OpenCvGPUDescriptorMatcherWrapper::OpenCvGPUDescriptorMatcherWrapper( cv::gpu::BruteForceMatcher_GPU_base *matcher ) : matcherBF_CUDA( matcher ), matcherBF_OCL( 0 )
{}

OpenCvGPUDescriptorMatcherWrapper::OpenCvGPUDescriptorMatcherWrapper( cv::ocl::BruteForceMatcher_OCL_base *matcher ) : matcherBF_OCL( matcher ), matcherBF_CUDA( 0 )
{}

OpenCvGPUDescriptorMatcherWrapper::~OpenCvGPUDescriptorMatcherWrapper()
{
    delete matcherBF_CUDA;
    delete matcherBF_OCL;
}

void OpenCvGPUDescriptorMatcherWrapper::knnMatchImpl(corecvs::RuntimeTypeBuffer &query
	, corecvs::RuntimeTypeBuffer &train
    , std::vector<std::vector<RawMatch> > &matches
    , size_t K)
{
    cv::Mat qd = convert( query ), td = convert( train );
    std::vector< std::vector<cv::DMatch> > matches_cv;
    if ( matcherBF_CUDA )
    {
        GpuMat gpuQuery( qd );
        GpuMat gpuTrain( td );

        matcherBF_CUDA->knnMatch( gpuQuery, gpuTrain, matches_cv, ( int )K );
    }
    else if ( matcherBF_OCL )
    {
        oclMat gpuQuery( qd );
        oclMat gpuTrain( td );

        matcherBF_OCL->knnMatch( gpuQuery, gpuTrain, matches_cv, ( int )K );
    }

    matches.clear();
    matches.resize(matches_cv.size());

    for (size_t idx = 0; idx < matches.size(); ++idx)
    {
        for (std::vector<cv::DMatch>::iterator m = matches_cv[idx].begin(); m != matches_cv[idx].end(); ++m)
        {
            matches[idx].push_back(convert(*m));
        }
    }
}

void OpenCvGPUDescriptorMatcherWrapper::setProperty( const std::string &name, const double &value )
{
    CORE_UNUSED( name );
    CORE_UNUSED( value );
}

double OpenCvGPUDescriptorMatcherWrapper::getProperty( const std::string &name ) const
{
    CORE_UNUSED( name );
    return 0.0;
}

extern bool FindGPUDevice( bool& cudaApi );

void init_opencv_gpu_matchers_provider()
{
    bool cudaApi = false;
    if ( FindGPUDevice( cudaApi ) )
        DescriptorMatcherProvider::getInstance().add( new OpenCvGPUDescriptorMatcherProvider( cudaApi ) );
}

#define SWITCH_TYPE(str, expr) \
    if(type == #str) \
    { \
        expr; \
    }

#define SWITCH_MATCHER_TYPE(str, expr) \
    if(matcher == #str) \
    { \
        expr; \
    }

OpenCvGPUDescriptorMatcherProvider::OpenCvGPUDescriptorMatcherProvider( bool cudaApi ) : cudaApi( cudaApi ) {}

DescriptorMatcher* OpenCvGPUDescriptorMatcherProvider::getDescriptorMatcher( const DescriptorType &type, const MatcherType &matcher, const std::string &params )
{
	CORE_UNUSED(params);
    if ( cudaApi )
    {
        SWITCH_MATCHER_TYPE(BF_GPU,
            SWITCH_TYPE(SURF_GPU, return new OpenCvGPUDescriptorMatcherWrapper( new cv::gpu::BruteForceMatcher_GPU_base( cv::gpu::BruteForceMatcher_GPU_base::L2Dist ) ); );
            SWITCH_TYPE(ORB_GPU,  return new OpenCvGPUDescriptorMatcherWrapper( new cv::gpu::BruteForceMatcher_GPU_base( cv::gpu::BruteForceMatcher_GPU_base::HammingDist ) ); ); );

		SWITCH_MATCHER_TYPE(BF_GPU,
			SWITCH_TYPE(SIFT, return new OpenCvGPUDescriptorMatcherWrapper(new cv::gpu::BruteForceMatcher_GPU_base(cv::gpu::BruteForceMatcher_GPU_base::L2Dist)););
		    SWITCH_TYPE(BRISK, return new OpenCvGPUDescriptorMatcherWrapper(new cv::gpu::BruteForceMatcher_GPU_base(cv::gpu::BruteForceMatcher_GPU_base::HammingDist)););
			SWITCH_TYPE(SURF, return new OpenCvGPUDescriptorMatcherWrapper(new cv::gpu::BruteForceMatcher_GPU_base(cv::gpu::BruteForceMatcher_GPU_base::L2Dist)););
		    SWITCH_TYPE(ORB, return new OpenCvGPUDescriptorMatcherWrapper(new cv::gpu::BruteForceMatcher_GPU_base(cv::gpu::BruteForceMatcher_GPU_base::HammingDist));););
    }
    else
    {
        SWITCH_MATCHER_TYPE( BF_GPU,
            SWITCH_TYPE(SURF_GPU, return new OpenCvGPUDescriptorMatcherWrapper(new cv::ocl::BruteForceMatcher_OCL_base( cv::ocl::BruteForceMatcher_OCL_base::L2Dist ) ); );
            SWITCH_TYPE(ORB_GPU,  return new OpenCvGPUDescriptorMatcherWrapper(new cv::ocl::BruteForceMatcher_OCL_base( cv::ocl::BruteForceMatcher_OCL_base::HammingDist ) ); ); );

		SWITCH_MATCHER_TYPE(BF_GPU,
			SWITCH_TYPE(SIFT, return new OpenCvGPUDescriptorMatcherWrapper(new cv::ocl::BruteForceMatcher_OCL_base(cv::ocl::BruteForceMatcher_OCL_base::L2Dist)););
		    SWITCH_TYPE(BRISK, return new OpenCvGPUDescriptorMatcherWrapper(new cv::ocl::BruteForceMatcher_OCL_base(cv::ocl::BruteForceMatcher_OCL_base::HammingDist)););
			SWITCH_TYPE(SURF, return new OpenCvGPUDescriptorMatcherWrapper(new cv::ocl::BruteForceMatcher_OCL_base(cv::ocl::BruteForceMatcher_OCL_base::L2Dist)););
		    SWITCH_TYPE(ORB, return new OpenCvGPUDescriptorMatcherWrapper(new cv::ocl::BruteForceMatcher_OCL_base(cv::ocl::BruteForceMatcher_OCL_base::HammingDist));););
    }

    CORE_ASSERT_FAIL_P(("OpenCvGPUDescriptorMatcherProvider::getDescriptorMatcher(%s, %s): no matcherWrapper", type.c_str(), matcher.c_str()));
    return 0;
}

bool OpenCvGPUDescriptorMatcherProvider::provides( const DescriptorType &type, const MatcherType &matcher )
{
    SWITCH_MATCHER_TYPE(BF_GPU,
        SWITCH_TYPE(SURF_GPU, return true;);
        SWITCH_TYPE(ORB_GPU, return true;););

	SWITCH_MATCHER_TYPE(BF_GPU,
		SWITCH_TYPE(SURF, return true;);
		SWITCH_TYPE(ORB, return true;););
    return false;
}

#undef SWITCH_TYPE

#else
void init_opencv_gpu_matchers_provider() {}

#endif