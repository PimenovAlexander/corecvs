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

    void setCorners(Vector3dd leftTop, Vector3dd rightBottom) {
        leftTopCorner = leftTop;
        rightBottomCorner = rightBottom;
        width = (int) getDrawingValue(rightBottomCorner.x() - leftTopCorner.x());
        height = (int) getDrawingValue(rightBottomCorner.y() - leftTopCorner.y());
    }

    void setMargins(int left, int right, int top, int bottom) {
        marginLeft = left;
        marginRight = right;
        marginTop = top;
        marginBottom = bottom;
    }

    Vector2d<int> getFullDimensions() {
        return Vector2d(width + marginLeft + marginRight, height + marginTop + marginBottom);
    }

    Vector2d<double> getDrawingValues(double x, double y) {
        Vector2d<double> result;
        result.x() = getDrawingValue(x - leftTopCorner.x()) + marginLeft;
        result.y() = (double) height - getDrawingValue(y - leftTopCorner.y()) + marginTop;
        return result;
    }

    double getDrawingValue(double value) {
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
    DxfDrawingUnits units = DxfDrawingUnits::CENTIMETERS;
    // in pixels:
    int width = 0;
    int height = 0;
    int marginLeft = 0;
    int marginRight = 0;
    int marginTop = 0;
    int marginBottom = 0;
    // in drawing coordinates:
    Vector3dd leftTopCorner;
    Vector3dd rightBottomCorner;
};

} // namespace corecvs

#endif //CORECVS_DXFDRAWINGATTRS_H
