#ifndef STEREORECONSTRUCTEDSCENE_H
#define STEREORECONSTRUCTEDSCENE_H

#include "mesh3DScene.h"

class StereoReconstructedScene : public Mesh3DScene
{
public:
	StereoReconstructedScene(DepthBuffer* buffer
			, const int& hScale = 1
			, const int& wScale = 1
			, const int& dScale = 1);

	StereoReconstructedScene(QImage image
			, const int& hScale = 1
			, const int& wScale = 1
			, const int& dScale = 1);
};

#endif // STEREORECONSTRUCTEDSCENE_H
