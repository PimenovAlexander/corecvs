/**
 * \file triangulator.h
 * \brief Add Comment Here
 *
 * \date Nov 17, 2011
 * \author alexander
 */

#ifndef TRIANGULATOR_H_
#define TRIANGULATOR_H_

#include <vector>

#include "cameraParameters.h"
#include "matrix33.h"
#include "essentialMatrix.h"
#include "sixDBuffer.h"
#include "vector3d.h"
#include "rgbColor.h"
#include "depthBuffer.h"
#include "cloud.h"
#include "tbbWrapper.h"

namespace corecvs {
using std::vector;

/**
 *  This class contains all the output information about the results of the
 *  rectification process
 *
 **/
class RectificationResult
{
public:
    /** Intrinsic parameters of the cameras*/
    CameraIntrinsics rightCamera;
    CameraIntrinsics leftCamera;

    /**
     * Essential matrix
     **/
    Matrix33  F;
    /**
     * Decomposed version of the matrix above
     *
     * Address this  class for ray based triangulation.
     **/
    EssentialDecomposition decomposition;

    /**
     * Transformations that should be applied to right input image to get rectified image
     **/
    Matrix33 rightTransform;

    /**
     * Transformations that should be applied to left input image to get rectified image
     **/
    Matrix33 leftTransform;

    /** Distance between cameras in units (generally millimeters) */
    double baseline;

    RectificationResult() :
        F(1.0),
        rightTransform(1.0),
        leftTransform (1.0),
        baseline(1.0)
    {}

    /**
     *   This method makes current RectificationResult that was created for a perticular input image corresponded
     *  to the transformed image. Say if you have rectified the full resolution, full frame image, and now want to
     *  tringulate a scaled subregion.
     **/
    RectificationResult addPretransform(const Matrix33 &transform) const
    {
        RectificationResult toReturn = *this;
        toReturn.leftTransform  = transform * this->leftTransform;
        toReturn.rightTransform = transform * this->rightTransform;
        return toReturn;
    }

    Vector2dd getRectifiedProjectionRight(const Vector3dd &point) const
    {
        return (rightTransform * rightCamera.getKMatrix33() * point).project();
    }

    Vector2dd getRectifiedProjectionLeft(const Vector3dd &point) const
    {
        Matrix44 toLeftFrame =
                Matrix44(decomposition.rotation) *
                Matrix44::Shift(decomposition.direction * baseline);

        Vector3dd pointInLeftFrame = toLeftFrame.inverted() * point;

        return (leftTransform * leftCamera.getKMatrix33() * pointInLeftFrame).project();
    }

    /* Gets a camera shift as a vector */
    Vector3dd getCameraShift() const
    {
        return baseline * decomposition.direction;
    }


template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(rightCamera, CameraIntrinsics(), "RightCameraIntrinsics");
        visitor.visit(leftCamera, CameraIntrinsics(), "LeftCameraIntrinsics");

        /* Fix this ASAP*/
        EssentialMatrix defaultEMatrix;
        Matrix33 defaultMatrix = defaultEMatrix;
        visitor.visit(F, defaultMatrix, "EssentialMatrix");
        visitor.visit(decomposition, EssentialDecomposition(), "EssentialDecomposition");

        visitor.visit(rightTransform, Matrix33(1.0), "RightTransform");
        visitor.visit(leftTransform, Matrix33(1.0), "LeftTransform");

        visitor.visit(baseline, 1.0, "Baseline");
    }

}; // RectificationResult

//-----------------------------------------------------------------------------------------

/**
 *  Swarm point is a class that holds the information about single point in space
 *  Associated with color and etc.
 *
 **/

class Triangulator
{
public:
    RectificationResult rectifierData;
    Matrix33 rectifiedToCameraFrameLeft;
    Matrix33 rectifiedToCameraFrameRight;

    Triangulator(const RectificationResult &_rectifierData);

    /* Getting single 3D poision from a point and disparity */
    Vector3dd triangulate(const Vector2dd &start, const Vector2dd &shift) const;

    Vector3dd triangulate(const Vector2dd &start, double shift) const;

    /* Get depth */
    double getDepth(const Vector2dd &start, const Vector2dd &shift) const;

    double getDepth(const Vector2dd &start, double shift) const;

    /* Get disparity*/
    double getDisparity(const Vector3dd &point) const;

    /**
     *  Input is right to left
     *
     **/
    Cloud *triangulate (DisparityBuffer *input, int density, bool enforceRectify = false) const;
    Cloud *triangulate (FloatFlowBuffer *input, int density, bool enforceRectify = false) const;

    Cloud *triangulate (SixDBuffer *input, int density) const;

    DepthBuffer *triangulateToDB (DisparityBuffer *input, bool enforceRectify = false) const;


    /*
    vector<SwarmPoint> *triangulate (DisparityBuffer *input, int density, bool enforceRectify = false) const;
    vector<SwarmPoint> *triangulate (FloatFlowBuffer *input, int density, bool enforceRectify = false) const;
    vector<SwarmPoint> *triangulate (SixDBuffer *input, int density) const;
    */

private:
    template<class InputType>
    Cloud *triangulateHelper (InputType *input,  int density, bool enforceRectify) const;
    template<class InputType>
    Cloud *triangulateHelperParallel (InputType *input,  int density, bool enforceRectify) const;

};


} //namespace corecvs
#endif /* TRIANGULATOR_H_ */

