#include "core/rectification/stereoAligner.h"
/** \page pRectification Rectification documentation page

  \section Introduction
     This document is devoted to rectification and triangulation process.

  \section Problem Formulation

  \subsection Origin Frames and coordinate systems

  We have following coordinate systems:
  <ul>
  	  <li>World</li>
  	  <li>Camera related</li>
  	  <li>Image related</li>
  	  <li>Rectified image related</li>
  </ul>

  \subsection World coordinate system

  This system is one of the widely used systems to describe 3D objects position in space.
  It is chosen based on convenience. For example this could be distances measured from
  the corner of the room.

  \subsection Camera related coordinate system


  This system is related to camera. It's origin is in the camera's optical center

  \subsection Image related coordinate system

  This system is related to top left corner of the image

  To transform Camera related to Image related systems you may use matrix K formed by function

  \ref corecvs::CameraIntrinsics::getKMatrix()

  \section Coordinate transformation
  \subsection Notations

  \section Rectification
  \subsection Estimating relative camera position

  Related classes:
  <ul>
     <li>\ref corecvs::EssentialEstimator "EssentialEstimator class"</li>
  </ul>


  \subsection Creating rectification transformations

  Related classes:
  <ul>
     <li>\ref corecvs::StereoAligner "StereoAligner class"</li>
  </ul>


  \section Triangulation

  Related classes:
  <ul>
     <li>\ref corecvs::EssentialMatrix "EssentialMatrix" - matrix in shift and rotation form</li>
     <li>\ref corecvs::EssentialDecomposition "EssentialDecomposition" - matrix in shift and rotation form</li>
     <li>\ref corecvs::Triangulator "Triangulator" - reconstruction of buffers </li>
  </ul>


*/
