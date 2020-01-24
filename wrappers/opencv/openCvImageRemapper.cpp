#include "opencv2/imgproc/imgproc.hpp"
#include "openCvImageRemapper.h"

#ifdef WITH_OPENCV_GPU
#include <opencv2/core/cuda.hpp>
#endif

namespace corecvs
{

static void convertRGB(const corecvs::RGB24Buffer& buffer, cv::Mat& mat)
{
	const int height = buffer.getH();
	const int width = buffer.getW();

	mat = cv::Mat(height, width, CV_8UC4);
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
			*mat.ptr<uint>(j, i) = buffer.element(j, i).color();		
	}
}

static void convertRGB(const cv::Mat& mat, corecvs::RGB24Buffer& buffer)
{
	const int height = mat.rows;
	const int width = mat.cols;

	buffer = corecvs::RGB24Buffer(height, width, false);
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
			buffer.element(j, i).color() = *mat.ptr<uint>(j, i);
	}
}

static void convertDisplacementBuffer( const corecvs::DisplacementBuffer &transform, cv::Mat& map )
{
    const int height = transform.getH();
    const int width = transform.getW();

    map = cv::Mat( height, width, CV_32FC2 );
    for ( int j = 0; j < height; j++ )
    {
        for ( int i = 0; i < width; i++ )
        {
            corecvs::Vector2dd elementd = transform.map( j, i );
            corecvs::Vector2df elementf( elementd.x(), elementd.y() );
            *map.ptr<corecvs::Vector2df>( j, i ) = elementf;
        }
    }
}

static void convertDisplacementBuffer(const corecvs::DisplacementBuffer &transform, cv::Mat& mapX, cv::Mat& mapY)
{
	const int height = transform.getH();
	const int width = transform.getW();

	mapX = cv::Mat(height, width, CV_32FC1);
	mapY = cv::Mat(height, width, CV_32FC1);
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			corecvs::Vector2dd elementd = transform.map(j, i);
			*mapX.ptr<float>(j, i) = (float)elementd.x();
			*mapY.ptr<float>(j, i) = (float)elementd.y();
		}
	}
}

void convert( const corecvs::DisplacementBuffer &transform, cv::Mat &map0, cv::Mat &map1 )
{
    cv::Mat map;
	convertDisplacementBuffer(transform, map);
	cv::convertMaps(map, cv::Mat(), map0, map1, CV_16SC2, true);
}

void remap( corecvs::RGB24Buffer &src, corecvs::RGB24Buffer &dst, const corecvs::DisplacementBuffer &transform )
{
	cv::Mat input, output;
    cv::Mat map, map0, map1;

	convertRGB(src, input);
    convertDisplacementBuffer( transform, map );
    cv::convertMaps( map, cv::Mat(), map0, map1, CV_16SC2, true );
    cv::remap( input, output, map0, map1, cv::INTER_NEAREST );
	convertRGB(output, dst);
}

template< typename T >
void convertT( const corecvs::DisplacementBuffer &transform, T &map0, T &map1 )
{
	cv::Mat _map0, _map1;
	convertDisplacementBuffer(transform, _map0, _map1);
	map0 = T(_map0);
	map1 = T(_map1);
}

#ifdef WITH_OPENCV_GPU
// cuda version
void convert( const corecvs::DisplacementBuffer &transform, cv::cuda::GpuMat &map0, cv::cuda::GpuMat &map1 )
{
    convertT( transform, map0, map1 );
}

// openCL version
void convert( const corecvs::DisplacementBuffer &transform, cv::UMat &map0, cv::UMat &map1 )
{
    convertT( transform, map0, map1 );
}
#endif

}
