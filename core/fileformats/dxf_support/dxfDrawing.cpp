//
// Created by Myasnikov Vladislav on 23.12.2019.
//

#include "core/fileformats/dxf_support/dxfDrawing.h"

namespace corecvs {

double DxfDrawing::getRealValue(double value) {
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

Vector2dd DxfDrawing::getDrawingValues(double x, double y) {
    Vector2dd result;
    result.x() = getDrawingValue(x - basePoint.x() - lowerLeftCorner.x() + paddingLeft);
    result.y() = (double) height - getDrawingValue(y - basePoint.y() - lowerLeftCorner.y() + paddingBottom);
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

RGB24Buffer* DxfDrawing::draw() {
    prepareToDraw();

    std::map<std::string,std::list<DxfEntity*>> visibleEntities = {};
    bool isFirstVisibleEntity = true;
    for (auto& kv : layerEntities) {
        auto layer = layers[kv.first];
        if (layer->data.isPlotted) {
            visibleEntities[kv.first] = {};
            for (DxfEntity *entity : kv.second) {
                if (entity->data.isVisible && entity->data.colorNumber >= 0) {
                    if (entity->data.block != nullptr) {
                        if (entity->data.block->isModelSpace) continue;
                        basePoint = entity->data.block->basePoint;
                    } else basePoint = Vector3dd(0, 0, 0);
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
                    visibleEntities[kv.first].push_back(entity);
                }
            }
        }
    }

    recalculatePaperSpaceDimensions();

    auto dimensions = getPaperSpaceDimensions();
    auto buffer = new RGB24Buffer(dimensions.y(), dimensions.x(), RGBColor::White());
    for (auto& kv : visibleEntities) {
        for (DxfEntity* entity : kv.second) {
            if (entity->data.block != nullptr) {
                basePoint = entity->data.block->basePoint;
            } else basePoint = Vector3dd(0, 0, 0);
            entity->draw(buffer, this);
        }
    }
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