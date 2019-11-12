//
// Created by Myasnikov Vladislav on 10/21/19.
//

#include "core/fileformats/dxf_support/implDxfBuilder.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/rgb24/rgbColor.h"
#include <iostream>

namespace corecvs {

// HEADER Section Processing Methods
void ImplDxfBuilder::setIntVariable(int code, std::string const &name, int value) {
    std::cout << "Int Variable: " << name << " (code: " << code << ", value: " << value << ") is set" << std::endl;
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
    entity->print();
    entities.push_back(entity);
}

RGB24Buffer* ImplDxfBuilder::draw() {
    RGB24Buffer* buffer = new RGB24Buffer(1000, 1000, RGBColor::White());
    for (DxfEntity* entity : entities) {
        entity->draw(buffer);
        entity->print();
    }
    return buffer;
}

} // namespace corecvs