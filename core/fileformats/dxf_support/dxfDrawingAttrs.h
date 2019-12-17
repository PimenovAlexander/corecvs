//
// Created by Myasnikov Vladislav on 12/5/19.
//

#ifndef CORECVS_DXFDRAWINGATTRS_H
#define CORECVS_DXFDRAWINGATTRS_H

#include "core/fileformats/dxf_support/dxfCodes.h"
#include "core/math/vector/vector2d.h"
#include "core/math/vector/vector3d.h"

namespace corecvs {

class DxfDrawingAttrs {
public:
    void setUnits(DxfDrawingUnits _units) {
        units = _units;
    }

    void setScalingFactor(double value) {
        scalingFactor = value;
        recalculatePaperSpaceDimensions();
    }

    void setCorners(Vector2dd lowerLeft, Vector2dd upperRight) {
        lowerLeftCorner = lowerLeft;
        upperRightCorner = upperRight;
        recalculatePaperSpaceDimensions();
    }

    void setPaddings(int left, int right, int top, int bottom) {
        paddingLeft = left;
        paddingRight = right;
        paddingTop = top;
        paddingBottom = bottom;
        recalculatePaperSpaceDimensions();
    }

    Vector2d<int> getPaperSpaceDimensions() {
        return Vector2d(width, height);
    }

    Vector2dd getDrawingValues(double x, double y) {
        Vector2dd result;
        result.x() = getDrawingValue(x - lowerLeftCorner.x() + paddingLeft);
        result.y() = (double) height - getDrawingValue(y - lowerLeftCorner.y() + paddingBottom);
        return result;
    }

    Vector2dd getDrawingValues(Vector2dd point) {
        return getDrawingValues(point.x(), point.y());
    }

    double getDrawingValue(double value) {
        return value * scalingFactor;
    }

    double getRealValue(double value) {
        switch(units) {
            case DxfDrawingUnits::UNITLESS:     return(value * 1.0);
            case DxfDrawingUnits::INCHES:       return(value * 25.4);
            case DxfDrawingUnits::FEET:         return(value * 25.4 * 12);
            case DxfDrawingUnits::MILES:        return(value *  1609344.0);
            case DxfDrawingUnits::MILLIMETERS:  return(value * 1.0);
            case DxfDrawingUnits::CENTIMETERS:  return(value * 10.0);
            case DxfDrawingUnits::METERS:       return(value * 1000.0);
            case DxfDrawingUnits::KILOMETERS:   return(value * 1000000.0);
            case DxfDrawingUnits::MICROINCHES:  return(value * 25.4 / 1000.0);
            case DxfDrawingUnits::MILS:         return(value * 25.4 / 1000.0);
            case DxfDrawingUnits::YARDS:        return(value * 3 * 12 * 25.4);
            case DxfDrawingUnits::ANGSTROMS:    return(value * 0.0000001);
            case DxfDrawingUnits::NANOMETERS:   return(value * 0.000001);
            case DxfDrawingUnits::MICRONS:      return(value * 0.001);
            case DxfDrawingUnits::DECIMETERS:   return(value * 100.0);
            case DxfDrawingUnits::DECAMETERS:   return(value * 10000.0);
            case DxfDrawingUnits::HECTOMETERS:  return(value * 100000.0);
            case DxfDrawingUnits::GIGAMETERS:   return(value * 1000000000000.0);
            case DxfDrawingUnits::ASTRONOMICAL: return(value * 149597870690000.0);
            case DxfDrawingUnits::LIGHT_YEARS:  return(value * 9454254955500000000.0);
            case DxfDrawingUnits::PARSECS:      return(value * 30856774879000000000.0);
        }
    }

private:
    DxfDrawingUnits units = DxfDrawingUnits::UNITLESS;
    double scalingFactor = 1.0;
    int width = 0;
    int height = 0;
    int paddingLeft = 0;
    int paddingRight = 0;
    int paddingTop = 0;
    int paddingBottom = 0;
    Vector2dd lowerLeftCorner;
    Vector2dd upperRightCorner;

    void recalculatePaperSpaceDimensions() {
        width = (int) getDrawingValue(upperRightCorner.x() - lowerLeftCorner.x() + paddingLeft + paddingRight);
        height = (int) getDrawingValue(upperRightCorner.y() - lowerLeftCorner.y() + paddingTop + paddingBottom);
    }
};

} // namespace corecvs

#endif //CORECVS_DXFDRAWINGATTRS_H
