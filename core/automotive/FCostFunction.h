/**
 * \file FCostFunction.h
 * \brief This file holds the data primitives for compensated flow cost function
 *
 * \date Feb 6, 2011
 * \author alexander
 */

#ifndef FCOSTFUNCTION_H_
#define FCOSTFUNCTION_H_
#include "matrix44.h"
#include "cameraParameters.h"
#include "eulerAngles.h"
#include "floatFlowBuffer.h"
namespace corecvs {

class FCostFunction
{
public:

enum CameraId {
    FIRST_CAMERA_NUMBER = 0,
    FRONT_VIEW_CAMERA = FIRST_CAMERA_NUMBER,
    REAR_VIEW_CAMERA,
    LEFT_SIDE_VIEW_CAMERA,
    RIGHT_SIDE_VIEW_CAMERA,
    CAMERA_NUMBER
};

enum CostFunctionType {
    FU = 0,
    FV,
    AUTO
};



public:



    FCostFunction();
    virtual ~FCostFunction();

    static Matrix44 RotationMatrix (
           double i_yaw,
           double i_pitch,
           double i_roll
       );

    static Matrix44 NominalMatrix ( CameraId cameraId );
    static Matrix44 InvNominalMatrix ( CameraId cameraId );

    static Matrix44 getMMatrix(
        CameraId cameraId,
        const ShiftRotateTransformation &cameraExtrinsics
    );

    static Matrix44 getDMatrix(
          CameraId cameraId,
          const CameraIntrinsicsLegacy          &cameraIntrinsics,
          const ShiftRotateTransformation &cameraExtrinsics
     );

    static Matrix44 getInvDMatrix(
         CameraId cameraId,
         const CameraIntrinsicsLegacy          &cameraIntrinsics,
         const ShiftRotateTransformation &cameraExtrinsics
     );

    static Matrix44 getBMatrix(
        CameraId cameraId,
        const CameraIntrinsicsLegacy &cameraIntrinsics,
        const ShiftRotateTransformation &cameraExtrinsics,
        const ShiftRotateTransformation &carEvolution
    );

   /*TODO: See how this should be interpreted in ISO and other standards*/
   static Matrix44 GetMatrixForTransform ( const ShiftRotateTransformation &transform );


   static double function(
       const FloatFlowVector &flow,
       const Matrix44 &B
   );

   static double functionPro(
         const FloatFlowVector &flow,
         const Matrix44 &B,
         double *distancePtr, /*<[out]*/
         CostFunctionType *typePtr /*<[out]*/
   );

   static double function(
       const FloatFlowVector &flow,
       CameraId cameraId,
       const CameraIntrinsicsLegacy &cameraIntrinsics,
       const ShiftRotateTransformation &cameraExtrinsics,
       const ShiftRotateTransformation &carEvolution);

   static double functionPro(
       const FloatFlowVector &flow,
       CameraId cameraId,
       const CameraIntrinsicsLegacy &cameraIntrinsics,
       const ShiftRotateTransformation &cameraExtrinsics,
       const ShiftRotateTransformation &carEvolution,
       double *distancePtr, /*<[out]*/
       CostFunctionType *typePtr /*<[out]*/);


};


} //namespace corecvs
#endif /* FCOSTFUNCTION_H_ */

