//
// Created by Myasnikov Vladislav on 10/21/19.
//

#include "core/fileformats/dxf_support/implDxfBuilder.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/rgb24/rgbColor.h"
#include <iostream>

namespace corecvs {

// Variables
void ImplDxfBuilder::setIntVariable(int code, std::string const &name, int value) {
//    std::cout << "Int Variable: " << name << " (code: " << code << ", value: " << value << ") is set" << std::endl;
}

void ImplDxfBuilder::setDoubleVariable(int code, std::string const &name, double value) {
//    std::cout << "Double Variable: " << name << " (code: " << code << ", value: " << value << ") is set" << std::endl;
}

void ImplDxfBuilder::setStringVariable(int code, std::string const &name, std::string const &value) {
//    std::cout << "String Variable: " << name << " (code: " << code << ", value: " << value << ") is set" << std::endl;
}

void ImplDxfBuilder::set2DVectorVariable(int code, std::string const &name, double x, double y) {
//    std::cout << "2D Vector Variable: " << name << " (code: " << code << ", value: " << x << "," << y << ") is set" << std::endl;
}

void ImplDxfBuilder::set3DVectorVariable(int code, std::string const &name, double x, double y, double z) {
//    std::cout << "3D Vector Variable: " << name << " (code: " << code << ", value: " << x << "," << y << "," << z << ") is set" << std::endl;
}

// Objects
void ImplDxfBuilder::addLayer(DxfLayerObject *object) {
    layers.insert(std::make_pair(object->data.name, object));
    objects.push_back(object);
}

void ImplDxfBuilder::addLineType(DxfLineTypeObject *object) {
    objects.push_back(object);
}

// Entities
void ImplDxfBuilder::addEntity(DxfEntity *entity) {
    entities.push_back(entity);
}

// Drawing
std::list<DxfEntity*> ImplDxfBuilder::prepareToDraw() {
    std::list<DxfEntity*> result = {};

    bool isFirstVisibleEntity = true;
    for (DxfEntity* entity : entities) {
        auto layer = layers[entity->data.layerName];

        if (entity->data.colorNumber == 256) entity->data.colorNumber = layer->data.colorNumber;
        auto rgb = DxfCodes::getRGB(entity->data.colorNumber);
        if (!rgb.empty()) entity->data.rgbColor = RGBColor(rgb[0], rgb[1], rgb[2]);

        if (layer->data.isPlotted && entity->data.isVisible && entity->data.colorNumber >= 0) {
            result.emplace_back(entity);
            auto box = entity->getBoundingBox();
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

    attrs.setCorners(lowerLeftCorner, upperRightCorner);
    attrs.setPaddings(110, 110, 110, 110);
    attrs.setScalingFactor(10.0);

    return result;
}

void ImplDxfBuilder::print() {
    for (DxfObject* object : objects) object->print();
    for (DxfEntity* entity : entities) entity->print();
    std::cout << "Lower-left corner: " << lowerLeftCorner << std::endl;
    std::cout << "Upper-right corner: " << upperRightCorner << std::endl;
}

RGB24Buffer* ImplDxfBuilder::draw() {
    auto visibleEntities = prepareToDraw();
    auto dimensions = attrs.getPaperSpaceDimensions();
    auto buffer = new RGB24Buffer(dimensions.y(), dimensions.x(), RGBColor::White());
    for (DxfEntity* entity : visibleEntities) entity->draw(buffer, &attrs);
//    print();
    return buffer;
}

} // namespace corecvs