/**
 * \file FCostFunction.cpp
 * \brief This file holds implementation of functions for compensated flow cost function
 *
 * \date Feb 6, 2011
 * \author alexander
 *
 *
  *   <h2> Fu function structure </h2>
 *
 *
 *   We have a 3D feature
 *   \f[ m = (x, y, z, 1)^T \f]
 *   World to camera matrix M
 *   \f[ M_{4,4} = R_a R_b R_c\f]
 *
 *   There are 3 coordinate systems types with
 *
 *   1. Car system - connected to the center of the mass of the car
 *   <ul>
 *   <li>\f$OX\f$ -reverse of car forward movement </li>
 *   <li>\f$OY\f$ -goes right from car </li>
 *   <li>\f$OZ\f$ -orthogonal, goes up into the air</li>
 *   </ul>
 *
 *   2. Camera system with car axis orientation but it's origin is connected with camera optical center
 *
 *   3. And the third one is the camera system (in fact 4 of them) that is connected with each cameras top left point of the image
 *   <ul>
 *   <li>\f$OX\f$ -to the right of the image (from top left to the top right corner)</li>
 *   <li>\f$OY\f$ -downwards (from top left to the bottom left corner)</li>
 *   <li>\f$OZ\f$ -orthogonal, goes from the camera into the scene</li>
 *   </ul>
 *
 *   The transformation from the 2nd to the 3rd system is denoted by the matrix \f$R_{camera}\f$
 *
 *
 *   So rotation matrixes are \f$Rc=R_{yaw}, Rb = R_{pitch}, Ra = R_{roll}\f$
 *
 *   Translation matrix is
 *
 *   \f[ T_{camera} =  \pmatrix{
 *                      1 & 0 & 0 & tx_{camera} \cr
 *                      0 & 1 & 0 & ty_{camera} \cr
 *                      0 & 0 & 1 & tz_{camera} \cr
 *                      0 & 0 & 0 & 1      }\f]
 *
 *   Then M matrix will be
 *
 *   \f[ M_{4,4} = R_c(\alpha_{yaw}) R_b(\alpha_{pitch}) R_a(\alpha_{roll}) R_{camera} T_{camera}\f]
 *
 *   As car moves - the m moves relative to car system of coordinates. This movement is denoted by the
 *   Matrix:
 *   \f[ A_{4,4}(t) \f]
 *
 *   A can be constructed as the product of the car translation matrix -
 *
  *   \f[ T_{car} =  \pmatrix{
 *                      1 & 0 & 0 & tx_{car} \cr
 *                      0 & 1 & 0 & ty_{car} \cr
 *                      0 & 0 & 1 & tz_{car} \cr
 *                      0 & 0 & 0 & 1      }\f]
*
 *   and car rotation matrix
 *
 *   \f[ R_{car} = R_c(\alpha_{frame yaw}) R_b(\alpha_{frame pitch}) R_a(\alpha_{frame roll}) \f]
 *
 *   So A will be
 *
 *   \f[ A(1) = R_{car} T_{car} \f]
 *
 *   Camera intrinsics scale and shift the image. Denoting camera scaler as
 *   \f$f = \frac {F_{Focal Length}} {K_{Pixel Size}}\f$ and camera image center as \f$(I_x, I_y)\f$ we can
 *   write the connection between the image coordinates and the camera coordinates.
 *   We will denote it by matrix \f$K\f$
 *
 *   \f[K =  \pmatrix{
 *        f \over k &       0    & I_x & 0 \cr
 *            0     &  f \over k & I_y & 0 \cr
 *            0     &       0    & 1   & 0 \cr
 *            0     &       0    & 0   & 1  }\f]
 *
 *   We now have the position of the \f$m\f$ how it is visible from the camera coordinate system
 *   in the moment 0 and 1
 *   \f[ q_0 = K M m\f]
 *   \f[ q_1 = K M A(1) m\f]
 *
 *   For simplification we will denote
 *
 *   \f[ D = K M \f]
 *
 *   So \f$q_1, q_0\f$ are connected with the following equation
 *   \f[ q_1 =  D A(1) D^{-1} q_0 \f]
 *
 *   As \f$K\f$ and \f$M\f$ both have very simple structure the analytical inversion is trivial.
 *   Now let's denote vector components the following way
 *
 *     \f[ q_0 = (u_0 d_0, v_0 d_0, d_0, 1) \f]
 *     \f[ q_1 = (u_1 d_1, v_1 d_1, d_1, 1) \f]
 *
 *   Time give up \f$m\f$ and study
 *
 *    the \f$q_i\f$ evolution
 *
 *    \f[ B_{4,4} = D A(1) D^{-1} \f]
 *    \f[ q_1 = B q_0 \f]
 *    \f[ (u_1 d_1, v_1 d_1, d_1, 1) = B (u_0 d_0, v_0 d_0, d_0, 1)^T \f]
 *
 *   Let's now introduce the notation for elements of B
 *
 *   \f[ B =  \pmatrix{ b_{0,0} & b_{0,1} & b_{0,2} & b_{0,3}\cr
 *                      b_{1,0} & b_{1,1} & b_{1,2} & b_{1,3}\cr
 *                      b_{2,0} & b_{2,1} & b_{2,2} & b_{2,3}\cr
 *                      0       & 0       & 0       & 1      } =
 *    \left( \begin{array}{cccc}
 *        \multicolumn{4}{c}{B_u} \\
 *        \multicolumn{4}{c}{B_v} \\
 *        \multicolumn{4}{c}{B_w} \\
 *        0 & 0 & 0 & 1
 *    \end{array} \right)
 *  \f]
 *
 * rewriting the matrix form into element one
 *
 * \f{eqnarray*}
 *  u_1 d_1 &=& b_{00} u_0 d_0 + b_{01} v_0 d_0 + b_{02} d_0 + b_{03} \\
 *  v_1 d_1 &=& b_{10} u_0 d_0 + b_{11} v_0 d_0 + b_{12} d_0 + b_{13} \\
 *      d_1 &=& b_{20} u_0 d_0 + b_{21} v_0 d_0 + b_{22} d_0 + b_{23}
 * \f}
 *
 * Introducing the substitutions independent from \f$d_0\f$ and \f$d_1\f$
 *
 * \f{eqnarray*}
 * T_u &=& b_{00} u_0 + b_{01} v_0 + b_{02} \\
 * T_v &=& b_{10} u_0 + b_{11} v_0 + b_{12} \\
 * T_w &=& b_{20} u_0 + b_{21} v_0 + b_{22}
 * \f}
 *
 * With this we can rewrite the equations to :
 *
 * \f{eqnarray}
 * u_1 d_1 &=& T_u d_0 + b_{03} \\
 * v_1 d_1 &=& T_v d_0 + b_{13} \\
 * d_1 &=& T_w d_0 + b_{23}
 * \f}
 *
 * <h3> Forward and Backward camera </h3>
 *
 * We now want to get rid of \f$d_0\f$ and \f$d_1\f$ completely, for this we substitute
 * \f$(3)\f$ into \f$(2)\f$
 *
 * \f{eqnarray*}
 *  v_1 (T_w d_0 + b_{23}) &=& T_v d_0 + b_{13} \\
 *     (v_1 T_w - T_v) d_0 &=& b_{13} - v_1 b_{23} \\
 *                      d0 &=& - \frac {b_{23} v_1 - b_{13}} {T_w v_1 - T_v} \label{D0v}
 * \f}
 *
 * <h3> Left and Right side camera </h3>
 *
 * Equation differs for side view cameras
 *
 * \f{eqnarray*}
 *  d0 &=& - \frac {b_{23} v_1 - b_{03}} {T_w u_1 - T_u} \label{D1v}
 * \f}
 *
 * In case of a still standing car, it could happen that d0 will be undefined because the
 * denominator of the equation will be equal to zero. In this case take any positive
 * value as d0.
 *
 * Introducing former into the equation \f$(3)\f$ leads to the distance d1
 *
 *  \f[ d_1 = d_0 T_w + b_{23} \f]
 *
 * Putting \f$d_0\f$ and \f$d_1\f$ into equations \f$(1)\f$ into \f$(2)\f$ results into the functions
 *
 * \f{eqnarray*}
 *  b_{00} u_0 d_0 + b_{01} v_0 d_0 + b_{02} d_0 + b_{03} - u_1 d_1 = 0 \\
 *  F_u() = u_1 - {\frac {1} {d_1}} (T_u d_0 + b_{14}) = 0
 * \f}
 *
 * \f{eqnarray*}
 *  b_{10} u_0 d_0 + b_{11} v_0 d_0 + b_{12} d_0 + b_{23} - v_1 d_1 = 0 \\
 *  F_v() = v_1 - {\frac {1} {d_1}} (T_v d_0 + b_{13}) = 0
 * \f}
 *
 * In the previous equation, in case \f$(u_1,v_1)\f$ is really the image of \f$(u_0,v_0)\f$ and camera parameters
 * are precise. \f$F_u\f$ and \f$F_v\f$ should both be zero. In reality if we will be able to find model parameters that makes all
 * \f$F_u\f$ and \f$F_v\f$ be zero - we have found a correct model.
 *
 *  Also we are able to build Jacobian for Kalman filter
 *
 *  \f[
 *  C = \pmatrix{
 *      \frac {\delta F} {\delta \alpha_{yaw}}   \cr
 *      \frac {\delta F} {\delta \alpha_{pitch}} \cr
 *      \frac {\delta F} {\delta C_{Road Curvature}}
 *      }
 *  \f]
 *
 *  <h2>Some additional Math </h2>
 *
 *  Formulas above give us the information about the actual depth of the object if we know car speed
 *  and visa versa.
 *  Knowing speed is a common situation, and thus we can guess depth.
 *
 *  Using \f$ d_1 \f$ derived form known matrices we can compute
 *
 *     \f[ q_1 = (u_1 d_1, v_1 d_1, d_1, 1) \f]
 *
 *  And then \f$ q_1 = K M m_1 = D m_1\f$ , where m_1 = A(1) m
 *
 *  So current position \f$ m_1\f$  could be computed as
 *
 *     \f[ m_1 = D^{-1} q_1 \f]
 *
 *
 **/


#include "core/cameracalibration/cameraModel.h"
#include "core/automotive/FCostFunction.h"
#include "core/math/matrix/matrix44.h"
#include "core/math/eulerAngles.h"

namespace corecvs {

FCostFunction::FCostFunction()
{
}

 /**   Rotation matrix in details:
  *
  *
  *   The axis goes as follows:
  *   <ul>
  *   <li>\f$OX\f$ -car forward movement </li>
  *   <li>\f$OY\f$ -goes right from car </li>
  *   <li>\f$OZ\f$ -orthogonal, goes into the ground </li>
  *   </ul>
  *
  *   Example of the rotation matrix:
  *   \f[
  *   Ra(\alpha_{roll}) = \pmatrix{
  *     1 & 0                  & 0                   & 0\cr
  *     0 & cos(\alpha_{roll}) & -sin(\alpha_{roll}) & 0\cr
  *     0 & sin(\alpha_{roll}) & cos(\alpha_{roll})  & 0\cr
  *     0 &  0                 & 0                   & 1
  *   }
  *   \f]
  *
  *   See more in the file describtion
  */
 Matrix44 FCostFunction::RotationMatrix (
       double i_yaw,
       double i_pitch,
       double i_roll
   )
 {

     /*TODO: Check and explain the order */
     /* Ra matrix initialization */
     Matrix33 Ra = Matrix33::RotationZ(i_yaw);
     Matrix33 Rb = Matrix33::RotationY(i_pitch);
     Matrix33 Rc = Matrix33::RotationX(i_roll);
     return Matrix44(Ra*Rb*Rc);
 }

 /**
  *  Transformation form camera system to car system depending on camera
  **/
 Matrix44 FCostFunction::NominalMatrix ( CameraId cameraId )
 {
    switch (cameraId)
    {
    /*Front*/
      case FRONT_VIEW_CAMERA:
         return Matrix44(
              0.0, 0.0,-1.0, 0.0,
              1.0, 0.0, 0.0, 0.0,
              0.0,-1.0, 0.0, 0.0,
              0.0, 0.0, 0.0, 1.0
         );
         break;
    /*Rear*/
      case REAR_VIEW_CAMERA:
          return Matrix44(
             0.0, 0.0, 1.0, 0.0,
            -1.0, 0.0, 0.0, 0.0,
             0.0,-1.0, 0.0, 0.0,
             0.0, 0.0, 0.0, 1.0
         );
         break;
     /*Left*/
      case LEFT_SIDE_VIEW_CAMERA:
         return Matrix44(
            -1.0, 0.0, 0.0, 0.0,
             0.0, 0.0,-1.0, 0.0,
             0.0,-1.0, 0.0, 0.0,
             0.0, 0.0, 0.0, 1.0
         );
         break;
     /*Right*/
      case RIGHT_SIDE_VIEW_CAMERA:
         return Matrix44(
             1.0, 0.0, 0.0, 0.0,
             0.0, 0.0, 1.0, 0.0,
             0.0,-1.0, 0.0, 0.0,
             0.0, 0.0, 0.0, 1.0
         );
         break;
      default:
         break;
    }
    return Matrix44(1.0);
 }

/**
 *  Helper function that transforms Euler angels into Matrix presentation
 *  Sould be merged into ShiftRotateTransformation class
 *
 */
Matrix44 FCostFunction::GetMatrixForTransform (
        const ShiftRotateTransformation &transform
        )
{
    Matrix44 REulerCar = RotationMatrix(
            transform.rotation.gamma,
            transform.rotation.beta,
            transform.rotation.alpha);

    Matrix44 TCar = Matrix44( Matrix33(1.0), transform.translation);
    return REulerCar * TCar;
}



 Matrix44 FCostFunction::getMMatrix(
     CameraId cameraId,
     const ShiftRotateTransformation &cameraExtrinsics
 )
 {
   Matrix44 invREulerCamera = RotationMatrix(
         cameraExtrinsics.rotation.gamma,
         cameraExtrinsics.rotation.beta,
         cameraExtrinsics.rotation.alpha).transposed();

   Matrix44 invNominalCamera = NominalMatrix(cameraId).transposed();
   Matrix44 invT = Matrix44( Matrix33(1.0), -cameraExtrinsics.translation);

   return invREulerCamera * invNominalCamera * invT;
 }

 Matrix44 FCostFunction::getInvDMatrix(
     CameraId cameraId,
     const PinholeCameraIntrinsics   &cameraIntrinsics,
     const ShiftRotateTransformation &cameraExtrinsics
 )
 {
   Matrix44 RCamera = RotationMatrix(
         cameraExtrinsics.rotation.gamma,
         cameraExtrinsics.rotation.beta,
         cameraExtrinsics.rotation.alpha);

   Matrix44 NominalCamera = NominalMatrix(cameraId);
   Matrix44 TCamera = Matrix44( Matrix33(1.0), cameraExtrinsics.translation);
   Matrix44 InvK = cameraIntrinsics.getInvKMatrix();

   return TCamera * NominalCamera * RCamera * InvK;
 }


 Matrix44 FCostFunction::getDMatrix(
       CameraId cameraId,
       const PinholeCameraIntrinsics   &cameraIntrinsics,
       const ShiftRotateTransformation &cameraExtrinsics
  )
 {
     Matrix44 M = FCostFunction::getMMatrix(cameraId, cameraExtrinsics);
     Matrix44 K = cameraIntrinsics.getKMatrix();
     return K * M;
 }

 Matrix44 FCostFunction::getBMatrix(
     CameraId cameraId,
     const PinholeCameraIntrinsics   &cameraIntrinsics,
     const ShiftRotateTransformation &cameraExtrinsics,
     const ShiftRotateTransformation &carEvolution
 )
 {

     Matrix44 REulerCamera = RotationMatrix(
           cameraExtrinsics.rotation.gamma,
           cameraExtrinsics.rotation.beta,
           cameraExtrinsics.rotation.alpha);

     Matrix44 NominalCamera = NominalMatrix(cameraId);
     Matrix44 invTCamera = Matrix44( Matrix33(1.0), -cameraExtrinsics.translation);
     Matrix44 TCamera    = Matrix44( Matrix33(1.0),  cameraExtrinsics.translation);


    /*Car Evolution*/
    Matrix44  A = GetMatrixForTransform(carEvolution);

    Matrix44  K    = cameraIntrinsics.getKMatrix();
    Matrix44  invK = cameraIntrinsics.getInvKMatrix();

    return K * REulerCamera.transposed() * NominalCamera.transposed() * invTCamera  *
           A *
           TCamera * NominalCamera * REulerCamera * invK;

 }

 double FCostFunction::function(
     const FloatFlowVector &flow,
     const Matrix44 &B
 )
 {

   double Tu, Tv, Tw;
   double d0, d1;

   /* Prepare Flow */
   double u0 = flow.start.x();
   double v0 = flow.start.y();

   double u1 = flow.end.x();
   double v1 = flow.end.y();

   Tu = B.a(0,0) * u0 + B.a(0,1) * v0 + B.a(0,2);
   Tv = B.a(1,0) * u0 + B.a(1,1) * v0 + B.a(1,2);
   Tw = B.a(2,0) * u0 + B.a(2,1) * v0 + B.a(2,2);

#ifdef OLDSTYLE
     if (fabs(v1 - v0) > fabs(u1 - u0)) // Flow is more vertical then horizontal
     {
       double denum = (Tw * v1 - Tv);
       if (denum == 0.0)
           return 0.0;
       d0 = - (B.a(2,3) * v1 - B.a(1,3)) / denum;

       if (d0 == 0.0)
           d0 = 1.0;

       d1 = d0 * Tw + B.a(2,3);

       if (d1 == 0.0)
           d1 = 1.0;

       return  u1 - (1.0 / d1) * (Tu * d0 + B.a(0,3));

     } else
     {
       double denum = (Tw * u1 - Tu);
       if (denum == 0.0)
           return 0.0;
       d0 = -(B.a(2,3) * u1 - B.a(0,3)) / denum;

       if (d0 == 0.0)
          d0 = 1.0;

       d1 = d0 * Tw + B.a(2,3);

       if (d1 == 0.0)
          d1 = 1.0;

       return  v1 - (1.0 / d1) * (Tv * d0 + B.a(1,3));
     }
#else
    double ru1,  rv1;
    double rTu,  rTv;
    double rb13, rb03;

    if (fabs(v1 - v0) < fabs(u1 - u0)) // Flow is more vertical then horizontal
    {
        rv1 = v1; rTv = Tv; rb13 = B.a(1,3);
        ru1 = u1; rTu = Tu; rb03 = B.a(0,3);
    } else {
        rv1 = u1; rTv = Tu; rb13 = B.a(0,3);
        ru1 = v1; rTu = Tv; rb03 = B.a(1,3);
    }

    double denum = (Tw * ru1 - rTu);
    if (denum == 0.0)
        return 0.0;
    d0 = - (B.a(2,3) * ru1 - rb03) / denum;

    if (d0 == 0.0)
     d0 = 1.0;

    d1 = d0 * Tw + B.a(2,3);

    if (d1 == 0.0)
     d1 = 1.0;

    return  rv1 - (1.0 / d1) * (rTv * d0 + rb13);

#endif
 }

 double FCostFunction::functionPro(
      const FloatFlowVector &flow,
      const Matrix44 &B,
      double *distancePtr, /*<[out]*/
      CostFunctionType *typePtr /*<[out]*/
  )
  {

    double Tu, Tv, Tw;
    double d0, d1;

    /* Prepare Flow */
    double u0 = flow.start.x();
    double v0 = flow.start.y();

    double u1 = flow.end.x();
    double v1 = flow.end.y();

    Tu = B.a(0,0) * u0 + B.a(0,1) * v0 + B.a(0,2);
    Tv = B.a(1,0) * u0 + B.a(1,1) * v0 + B.a(1,2);
    Tw = B.a(2,0) * u0 + B.a(2,1) * v0 + B.a(2,2);

     double ru1,  rv1;
     double rTu,  rTv;
     double rb13, rb03;

     CostFunctionType type;

     if (typePtr == NULL || *typePtr == AUTO)
         type = (fabs(v1 - v0) > fabs(u1 - u0)) ? FV : FU;
     else
         type = *typePtr;

     if (type == FU) // Flow is more vertical then horizontal
     {
         rv1 = v1; rTv = Tv; rb13 = B.a(1,3);
         ru1 = u1; rTu = Tu; rb03 = B.a(0,3);
     } else {
         rv1 = u1; rTv = Tu; rb13 = B.a(0,3);
         ru1 = v1; rTu = Tv; rb03 = B.a(1,3);
     }

     double denum = (Tw * ru1 - rTu);
     if (denum == 0.0)
         return 0.0;
     d0 = -(B.a(2,3) * ru1 - rb03) / denum;

     if (d0 == 0.0)
      d0 = 1.0;

     d1 = d0 * Tw + B.a(2,3);

     if (d1 == 0.0)
      d1 = 1.0;

     double f = rv1 - (1.0 / d1) * (rTv * d0 + rb13);

     if (typePtr)
         *typePtr = type;
     if (distancePtr)
         *distancePtr = d1;

     return  f;
  }

double FCostFunction::function(
        const FloatFlowVector &flow,
        CameraId cameraId,
        const PinholeCameraIntrinsics   &cameraIntrinsics,
        const ShiftRotateTransformation &cameraExtrinsics,
        const ShiftRotateTransformation &carEvolution)
 {
     Matrix44 B = getBMatrix(cameraId, cameraIntrinsics, cameraExtrinsics, carEvolution);
     return function(flow, B);
 }


double FCostFunction::functionPro(const FloatFlowVector &flow,
        CameraId cameraId,
        const PinholeCameraIntrinsics   &cameraIntrinsics,
        const ShiftRotateTransformation &cameraExtrinsics,
        const ShiftRotateTransformation &carEvolution,
        double *distancePtr, /*<[out]*/
        CostFunctionType *typePtr /*<[out]*/)
{
     Matrix44 B = getBMatrix(cameraId, cameraIntrinsics, cameraExtrinsics, carEvolution);
     return functionPro(flow, B, distancePtr, typePtr);
}


FCostFunction::~FCostFunction()
{
    // TODO Auto-generated destructor stub
}

} //namespace corecvs

