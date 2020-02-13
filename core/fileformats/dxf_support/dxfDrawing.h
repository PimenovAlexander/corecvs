#ifndef DXF_SUPPORT_DXFDRAWING_H
#define DXF_SUPPORT_DXFDRAWING_H
//
// Created by Myasnikov Vladislav on 5.12.2019.
//

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
    DxfDrawing(std::map<std::string, DxfLayerObject*> layers, std::map<std::string, DxfBlock*> blocks, std::map<std::string, DxfObject*> otherObjects,
               std::map<std::string, std::list<DxfEntity*>> layerEntities, std::map<std::string, DxfBlockRecordObject*> blockRecords, std::map<std::string, std::list<DxfEntity*>> blockEntities)
               : layers(layers), blocks(blocks), otherObjects(otherObjects), layerEntities(layerEntities), blockRecords(blockRecords), blockEntities(blockEntities) {}

    ~DxfDrawing();

    void setScalingFactor(double factor) {
        scalingFactor = factor;
        recalculatePaperSpaceDimension();
    }

    void setPaddings(int left, int right, int top, int bottom) {
        paddingLeft = left;
        paddingRight = right;
        paddingTop = top;
        paddingBottom = bottom;
        recalculatePaperSpaceDimension();
    }

    void setClockwiseDirection(bool value) { isClockwiseDirectionSet = value; }

    double getDrawingValue(double value) { return value * scalingFactor; }

    Vector2dd getDrawingValues(double x, double y);

    Vector2dd getDrawingValues(Vector2dd point) { return getDrawingValues(point.x(), point.y()); }

    bool isClockwiseDirection() { return isClockwiseDirectionSet; }

    RGB24Buffer *draw();

    void print();

private:
    DxfDrawingUnits units = DxfDrawingUnits::UNITLESS;
    double scalingFactor = 1.0;
    bool isClockwiseDirectionSet = false;
    Vector2d32 paperSpaceDimension;
    Vector2dd lowerLeftCorner;
    Vector2dd upperRightCorner;
    Vector3dd basePoint;
    Vector3dd entityScalingFactor;
    bool isFirstVisibleEntity;
    int paddingLeft = 0;
    int paddingRight = 0;
    int paddingTop = 0;
    int paddingBottom = 0;

    std::map<std::string, DxfLayerObject*> layers;
    std::map<std::string, DxfBlockRecordObject*> blockRecords;
    std::map<std::string, DxfObject*> otherObjects;
    std::map<std::string, DxfBlock*> blocks;
    std::map<std::string, std::list<DxfEntity*>> layerEntities;
    std::map<std::string, std::list<DxfEntity*>> blockEntities;

    Rectangled translate2WCS(Rectangled box) {
        return Rectangled(box.ulCorner() + basePoint.xy(), box.size);
    }

    void recalculatePaperSpaceDimension() {
        paperSpaceDimension.x() = (int) getDrawingValue(upperRightCorner.x() - lowerLeftCorner.x() + paddingLeft + paddingRight);
        paperSpaceDimension.y() = (int) getDrawingValue(upperRightCorner.y() - lowerLeftCorner.y() + paddingTop + paddingBottom);
    }

    void prepareToDraw();
    void calculateVisibleSpace(DxfLayerObject *layer, std::list<DxfEntity*> &entities);
    void drawLayer(RGB24Buffer *buffer, DxfLayerObject *layer, std::list<DxfEntity*> &entities);
};

} // namespace corecvs

#endif //DXF_SUPPORT_DXFDRAWING_H
