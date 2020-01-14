//
// Created by Myasnikov Vladislav on 23.12.2019.
//

#include "core/fileformats/dxf_support/dxfDrawing.h"

namespace corecvs {

Vector2dd DxfDrawing::getDrawingValues(double x, double y) {
    Vector2dd result;
    result.x() = getDrawingValue(x + basePoint.x() - lowerLeftCorner.x() + paddingLeft);
    result.y() = (double) paperSpaceDimension.y() - getDrawingValue(y + basePoint.y() - lowerLeftCorner.y() + paddingBottom);
    return result;
}

void DxfDrawing::prepareToDraw() {
    for (auto& kv : layerEntities) {
        auto layer = layers[kv.first];
        for (DxfEntity *entity : kv.second) {
            if (entity->data.colorNumber == 256) entity->data.colorNumber = layer->data.colorNumber;
            auto rgb = DxfCodes::getRGB(entity->data.colorNumber);
            if (!rgb.empty()) entity->data.rgbColor = RGBColor(rgb[0], rgb[1], rgb[2]);
        }
    }
}

void DxfDrawing::calculateVisibleSpace(DxfLayerObject *layer, std::list<DxfEntity*> &entities) {
    if (layer->data.isPlotted) {
        for (DxfEntity *entity : entities) {
            if (entity->data.isVisible && entity->data.colorNumber >= 0) {
                auto blockReference = dynamic_cast<DxfBlockReferenceEntity*> (entity);
                if (blockReference != nullptr) {
                    auto oldBasePoint = basePoint;
                    auto oldEntityScalingFactor = entityScalingFactor;
                    basePoint = blockReference->data.insertionPoint;
                    entityScalingFactor = blockReference->data.scaleFactor;

                    calculateVisibleSpace(layer, blocks[blockReference->data.blockName]->entities);

                    basePoint = oldBasePoint;
                    entityScalingFactor = oldEntityScalingFactor;
                } else {
                    auto box = translate2WCS(entity->getBoundingBox());
                    if (isFirstVisibleEntity) {
                        lowerLeftCorner = box.ulCorner();
                        upperRightCorner = box.lrCorner();
                        isFirstVisibleEntity = false;
                    } else {
                        if (box.ulCorner().x() < lowerLeftCorner.x()) lowerLeftCorner.x() = box.ulCorner().x();
                        if (box.lrCorner().x() > upperRightCorner.x()) upperRightCorner.x() = box.lrCorner().x();
                        if (box.ulCorner().y() < lowerLeftCorner.y()) lowerLeftCorner.y() = box.ulCorner().y();
                        if (box.lrCorner().y() > upperRightCorner.y()) upperRightCorner.y() = box.lrCorner().y();
                    }
                }
            }
        }
    }
}

void DxfDrawing::drawLayer(RGB24Buffer *buffer, DxfLayerObject *layer, std::list<DxfEntity*> &entities) {
    if (layer->data.isPlotted) {
        for (DxfEntity *entity : entities) {
            if (entity->data.isVisible && entity->data.colorNumber >= 0) {
                auto blockReference = dynamic_cast<DxfBlockReferenceEntity*> (entity);
                if (blockReference != nullptr) {
                    auto oldBasePoint = basePoint;
                    auto oldEntityScalingFactor = entityScalingFactor;
                    basePoint = blockReference->data.insertionPoint;
                    entityScalingFactor = blockReference->data.scaleFactor;

                    drawLayer(buffer, layer, blocks[blockReference->data.blockName]->entities);

                    basePoint = oldBasePoint;
                    entityScalingFactor = oldEntityScalingFactor;
                } else {
                    entity->draw(buffer, this);
                }
            }
        }
    }
}

RGB24Buffer* DxfDrawing::draw() {
    prepareToDraw();

    isFirstVisibleEntity = true;
    std::map<std::string,std::list<DxfEntity*>> nonBlockLayerEntities;
    for (auto& kv : layers) {
        nonBlockLayerEntities[kv.first] = {};
        for (DxfEntity* entity : layerEntities[kv.first]) {
            if (entity->data.block == nullptr) nonBlockLayerEntities[kv.first].push_back(entity);
        }
        calculateVisibleSpace(kv.second, nonBlockLayerEntities[kv.first]);
    }

    recalculatePaperSpaceDimension();

    auto buffer = new RGB24Buffer(paperSpaceDimension.y(), paperSpaceDimension.x(), RGBColor::White());
    for (auto& kv : layers) drawLayer(buffer, kv.second, nonBlockLayerEntities[kv.first]);

    print();
    return buffer;
}

void DxfDrawing::print() {
    for (auto& kv : layers) kv.second->print();
    for (auto& kv : blocks) kv.second->print();
    for (auto& kv : blockRecords) kv.second->print();

    std::cout << "Lower-left corner: " << lowerLeftCorner << std::endl;
    std::cout << "Upper-right corner: " << upperRightCorner << std::endl;
}

} // namespace corecvs