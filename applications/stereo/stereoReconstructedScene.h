#ifndef STEREORECONSTRUCTEDSCENE_H
#define STEREORECONSTRUCTEDSCENE_H

#include "mesh3DScene.h"

class StereoReconstructedScene : public Mesh3DScene
{
public:
	StereoReconstructedScene(DepthBuffer* buffer
			, const double& hScale = 1
			, const double& wScale = 1
			, const double& dScale = 1);

	StereoReconstructedScene(QImage image
			, const double& hScale = 1
			, const double& wScale = 1
			, const double& dScale = 1);

	corecvs::Vector3dd radius;
	corecvs::Vector3dd center;
};

#endif // STEREORECONSTRUCTEDSCENE_H
