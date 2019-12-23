//
// Created by Myasnikov Vladislav on 12/5/19.
//

#ifndef DXF_SUPPORT_DXFDRAWING_H
#define DXF_SUPPORT_DXFDRAWING_H

#include "core/fileformats/dxf_support/dxfCodes.h"
#include "core/fileformats/dxf_support/objects/dxfObject.h"
#include "core/fileformats/dxf_support/entities/dxfEntity.h"
#include "core/fileformats/dxf_support/blocks/dxfBlock.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/math/vector/vector2d.h"
#include "core/math/vector/vector3d.h"

namespace corecvs {

class DxfEntity;
class DxfBlock;

class DxfDrawing {
public:
    DxfDrawing(std::map<std::string,DxfLayerObject*> layers, std::map<std::string,DxfBlock*> blocks, std::map<std::string,DxfObject*> otherObjects,
            std::map<std::string,std::list<DxfEntity*>> layerEntities, std::map<std::string,DxfBlockRecordObject*> blockRecords)
    : layers(layers), blocks(blocks), otherObjects(otherObjects), layerEntities(layerEntities), blockRecords(blockRecords) {}

    void setScalingFactor(double value) {
        scalingFactor = value;
        recalculatePaperSpaceDimensions();
    }

    void setPaddings(int left, int right, int top, int bottom) {
        paddingLeft = left;
        paddingRight = right;
        paddingTop = top;
        paddingBottom = bottom;
        recalculatePaperSpaceDimensions();
    }

    void setClockwiseDirection(bool value) { isClockwiseDirectionSet = value; }

    double getDrawingValue(double value) { return value * scalingFactor; }
    Vector2dd getDrawingValues(double x, double y);
    Vector2dd getDrawingValues(Vector2dd point) { return getDrawingValues(point.x(), point.y()); }
    bool isClockwiseDirection() { return isClockwiseDirectionSet; }

    RGB24Buffer* draw();
    void print();

private:
    DxfDrawingUnits units = DxfDrawingUnits::UNITLESS;
    double scalingFactor = 1.0;
    bool isClockwiseDirectionSet = false;
    int width = 0;
    int height = 0;
    int paddingLeft = 0;
    int paddingRight = 0;
    int paddingTop = 0;
    int paddingBottom = 0;
    Vector3dd basePoint;
    Vector2dd lowerLeftCorner;
    Vector2dd upperRightCorner;

    std::map<std::string,DxfLayerObject*> layers;
    std::map<std::string,DxfBlock*> blocks;
    std::map<std::string,DxfObject*> otherObjects;
    std::map<std::string,std::list<DxfEntity*>> layerEntities;
    std::map<std::string,DxfBlockRecordObject*> blockRecords;

    double getRealValue(double value);

    Vector2d<int> getPaperSpaceDimensions() { return Vector2d(width, height); }

    Rectangled translate2WCS(Rectangled box) {
        return Rectangled(box.ulCorner() + basePoint.xy(), box.size);
    }

    void recalculatePaperSpaceDimensions() {
        width = (int) getDrawingValue(upperRightCorner.x() - lowerLeftCorner.x() + paddingLeft + paddingRight);
        height = (int) getDrawingValue(upperRightCorner.y() - lowerLeftCorner.y() + paddingTop + paddingBottom);
    }

    void prepareToDraw();
};

} // namespace corecvs

#endif //DXF_SUPPORT_DXFDRAWING_H
