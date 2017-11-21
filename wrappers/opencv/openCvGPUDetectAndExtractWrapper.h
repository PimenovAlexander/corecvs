#pragma once

#ifdef WITH_OPENCV

#include "core/features2d/detectAndExtractProvider.h"

namespace cv {
	namespace gpu {
		class SURF_GPU;
		class ORB_GPU;
	}

	namespace ocl {
		class SURF_OCL;
	}

	class SURF;
	class ORB;
}

class OpenCvGPUDetectAndExtractWrapper : public DetectAndExtract
{
public:
	OpenCvGPUDetectAndExtractWrapper(cv::ORB *detectorORB = 0, cv::SURF *detectorSURF = 0, cv::gpu::SURF_GPU *detectorSURF_CUDA = 0, cv::gpu::ORB_GPU *detectorORB_CUDA = 0, cv::ocl::SURF_OCL* detectorSURF_OCL = 0);
   ~OpenCvGPUDetectAndExtractWrapper();

	double getProperty(const std::string &name) const;
	void   setProperty(const std::string &name, const double &value);

	bool  isParallelable() { return false; }

protected:
	void detectAndExtractImpl(corecvs::RuntimeTypeBuffer &image, std::vector<::KeyPoint> &keyPoints, corecvs::RuntimeTypeBuffer &descriptors, int nMaxKeypoints, void* pRemapCache);

private:
    OpenCvGPUDetectAndExtractWrapper(const OpenCvGPUDetectAndExtractWrapper&);
  //OpenCvGPUDetectAndExtractWrapper& operator=( const OpenCvGPUDetectAndExtractWrapper& );

	cv::gpu::SURF_GPU* detectorSURF_CUDA;
	cv::gpu::ORB_GPU*  detectorORB_CUDA;
	cv::ocl::SURF_OCL* detectorSURF_OCL;

	cv::SURF*	detectorSURF;
	cv::ORB*	detectorORB;
};

extern "C"
{
    bool init_opencv_gpu_detect_and_extract_provider( bool& cudaApi );
}

class OpenCvGPUDetectAndExtractProvider : public DetectAndExtractProviderImpl
{
public:
    OpenCvGPUDetectAndExtractProvider( bool cudaApi );

    ~OpenCvGPUDetectAndExtractProvider() {}

    DetectAndExtract* getDetector( const DetectorType &detectorType, const DescriptorType &descriptorType, const std::string &params = "" );
	bool provides(const DetectorType &detectorType, const DescriptorType &descriptorType);
    virtual std::string name()  override { return "OpenCv GPU"; }

private:
	bool  cudaApi; // opencv module to use : cuda or opencl
};


#endif // WITH_OPENCV
