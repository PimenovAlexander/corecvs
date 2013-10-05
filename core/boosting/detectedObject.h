#ifndef DETECTED_OBJECT_H_
#define DETECTED_OBJECT_H_

/**
 * \file detectedObject.h
 * \brief Add Comment Here
 *
 * \date May 22, 2012
 * \author alexander
 */

#include "vector3d.h"
#include "quaternion.h"
#include "vector2d.h"

using corecvs::Vector3dd;
using corecvs::Quaternion;
using corecvs::Vector2dd;

namespace corecvs
{

class DetectedObject
{
public:

    /** Position of the object center in image*/
    Vector2dd imagePosition;
    /** Position of the object center in image after stabilization*/
    Vector2dd imagePositionStab;
    /** Size of the object in image */
    Vector2dd imageSize;
    /** Size of the object in image after stabilization */
    Vector2dd imageSizeStab;

    /** Position of the object in space coordinates */
    Vector3dd position3D;
    /*Quaternion rotation;*/
    /** Size of the object in space coordinates */
    Vector3dd size;

    /** Mean flow in the object plane */
    Vector2dd meanFlow;
    int flowPoints;

    double meanDisparity;
    int stereoPoints;

    bool isMain;

#if 0
    QRect leftHandZone;
    QRect rightHandZone;
    QRect activeHandZone;

    bool leftZoneActive() {
        return activeHandZone == leftHandZone;
    }

    bool rightZoneActive() {
        return activeHandZone == rightHandZone;
    }
#endif
    /**
     * Flag that is set if the object is lost and it's position is predicted
     **/
    bool isVirtualObject;

    DetectedObject(
        const Vector2dd &pos,
        const Vector2dd &size
    );

    virtual ~DetectedObject();
};

} /* namespace corecvs */
#endif /* DETECTEDOBJECT_H_ */
