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
    std::cout << "Int Variable: " << name << " (code: " << code << ", value: " << value << ") is set" << std::endl;
    if (name == "$INSUNITS") attrs.setUnits(DxfCodes::getDrawingUnits(value));
}

void ImplDxfBuilder::setDoubleVariable(int code, std::string const &name, double value) {
    std::cout << "Double Variable: " << name << " (code: " << code << ", value: " << value << ") is set" << std::endl;
}

void ImplDxfBuilder::setStringVariable(int code, std::string const &name, std::string const &value) {
    std::cout << "String Variable: " << name << " (code: " << code << ", value: " << value << ") is set" << std::endl;
}

void ImplDxfBuilder::set2DVectorVariable(int code, std::string const &name, double x, double y) {
    std::cout << "2D Vector Variable: " << name << " (code: " << code << ", value: " << x << "," << y << ") is set" << std::endl;
}

void ImplDxfBuilder::set3DVectorVariable(int code, std::string const &name, double x, double y, double z) {
    std::cout << "3D Vector Variable: " << name << " (code: " << code << ", value: " << x << "," << y << "," << z << ") is set" << std::endl;
    if (name == "$EXTMIN") leftTopCorner = Vector3dd(x,y,z);
    else if (name == "$EXTMAX") rightBottomCorner = Vector3dd(x,y,z);
}

// Objects
void ImplDxfBuilder::addLayer(DxfLayerObject *object) {
    layers.insert(std::make_pair(object->getData()->name, object));
    objects.push_back(object);
}

void ImplDxfBuilder::addLineType(corecvs::DxfLineTypeObject *object) {
    objects.push_back(object);
}

// Entities
void ImplDxfBuilder::addEntity(DxfEntity *entity) {
    entities.push_back(entity);
}

// Drawing
void ImplDxfBuilder::prepareToDraw() {
    attrs.setCorners(leftTopCorner, rightBottomCorner);
    attrs.setMargins(100, 100, 100, 100);

    for (DxfEntity* entity : entities) {
        auto data = entity->getData();
        if (data->colorNumber == 256) data->colorNumber = layers[data->layerName]->getData()->colorNumber;
        auto rgb = DxfCodes::getRGB(data->colorNumber);
        if (!rgb.empty()) data->rgbColor = RGBColor(rgb[0], rgb[1], rgb[2]);
    }

/* Uncomment for additional info printing */
//    for (DxfObject* object : objects) object->print();
//    for (DxfEntity* entity : entities) entity->print();
    std::cout << "Left-top corner: " << leftTopCorner << std::endl;
    std::cout << "Right-bottom corner: " << rightBottomCorner << std::endl;
}

RGB24Buffer* ImplDxfBuilder::draw() {
    prepareToDraw();
    auto dimensions = attrs.getFullDimensions();
    auto buffer = new RGB24Buffer(dimensions.y(), dimensions.x(), RGBColor::White());
    for (DxfEntity* entity : entities) entity->draw(buffer, &attrs);
    return buffer;
}

} // namespace corecvs