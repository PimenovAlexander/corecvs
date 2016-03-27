#include "openCvKeyPointsWrapper.h"

cv::KeyPoint convert(const KeyPoint &kp)
{
	return cv::KeyPoint(kp.x, kp.y, kp.size, kp.angle, kp.response, kp.octave);
}

KeyPoint convert(const cv::KeyPoint &kp)
{
	return KeyPoint(kp.pt.x, kp.pt.y, kp.size, kp.angle, kp.response, kp.octave);
}

cv::DMatch convert(const RawMatch &rm)
{
	return cv::DMatch(rm.featureQ, rm.featureT, rm.distance);
}

RawMatch convert(const cv::DMatch &dm)
{
	return RawMatch(dm.queryIdx, dm.trainIdx, dm.distance);
}

cv::Mat convert(const RuntimeTypeBuffer &buffer)
{
	return cv::Mat((int)buffer.getRows()
                 , (int)buffer.getCols()
                 , (int)buffer.getCvType()
                 , buffer.row<void>((size_t)0));
}

RuntimeTypeBuffer convert(const cv::Mat &mat)
{
	return RuntimeTypeBuffer(mat.rows, mat.cols
        , mat.data
        , (int)RuntimeTypeBuffer::getTypeFromCvType(mat.type()));
}
