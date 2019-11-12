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
    if (name == "$INSUNITS") units = DxfCodes::getDrawingUnits(value);
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
    object->print();
}

void ImplDxfBuilder::addLineType(corecvs::DxfLineTypeObject *object) {
    object->print();
}

// Entities
void ImplDxfBuilder::addLine(DxfLineEntity *entity) {
    entities.push_back(entity);
}

// Drawing
RGB24Buffer* ImplDxfBuilder::draw() {
    auto x1 = DxfCodes::getDrawingValue(leftTopCorner.x(), units);
    auto y1 = DxfCodes::getDrawingValue(leftTopCorner.y(), units);
    auto x2 = DxfCodes::getDrawingValue(rightBottomCorner.x(), units);
    auto y2 = DxfCodes::getDrawingValue(rightBottomCorner.y(), units);
    width = y2 - y1;
    height = x2 - x1;

    RGB24Buffer* buffer = new RGB24Buffer(width, height, RGBColor::White());
    for (DxfEntity* entity : entities) {
        entity->draw(buffer, units);
        entity->print();
    }

    std::cout << "Left-top corner: " << leftTopCorner << std::endl;
    std::cout << "Right-bottom corner: " << rightBottomCorner << std::endl;

    return buffer;
}

} // namespace corecvs