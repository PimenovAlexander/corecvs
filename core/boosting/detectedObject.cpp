/**
 * \file detectedObject.cpp
 * \brief Add Comment Here
 *
 * \date May 22, 2012
 * \author alexander
 */

#include "boosting/detectedObject.h"

namespace corecvs
{

DetectedObject::DetectedObject(
        const Vector2dd &pos,
        const Vector2dd &size
) :
    imagePosition     (pos),
    imagePositionStab (pos),
    imageSize         (size),
    imageSizeStab     (size),
    position3D        (0.0),
    size              (0.0),
    meanFlow          (0.0),
    flowPoints        (0),
    meanDisparity     (0),
    stereoPoints      (0),
    isMain            (false),

    isVirtualObject   (false)
{}

DetectedObject::~DetectedObject()
{
    // TODO Auto-generated destructor stub
}

} /* namespace corecvs */
