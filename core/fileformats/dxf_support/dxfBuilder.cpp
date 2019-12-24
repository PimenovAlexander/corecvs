//
// Created by Myasnikov Vladislav on 21.10.2019.
//

#include "core/fileformats/dxf_support/dxfBuilder.h"
#include <iostream>

namespace corecvs {

// Variables
void DxfBuilder::setIntVariable(int code, std::string const &name, int value) {
//    std::cout << "Int Variable: " << name << " (code: " << code << ", value: " << value << ") is set" << std::endl;
}

void DxfBuilder::setDoubleVariable(int code, std::string const &name, double value) {
//    std::cout << "Double Variable: " << name << " (code: " << code << ", value: " << value << ") is set" << std::endl;
}

void DxfBuilder::setStringVariable(int code, std::string const &name, std::string const &value) {
//    std::cout << "String Variable: " << name << " (code: " << code << ", value: " << value << ") is set" << std::endl;
}

void DxfBuilder::set2DVectorVariable(int code, std::string const &name, double x, double y) {
//    std::cout << "2D Vector Variable: " << name << " (code: " << code << ", value: " << x << "," << y << ") is set" << std::endl;
}

void DxfBuilder::set3DVectorVariable(int code, std::string const &name, double x, double y, double z) {
//    std::cout << "3D Vector Variable: " << name << " (code: " << code << ", value: " << x << "," << y << "," << z << ") is set" << std::endl;
}

// Objects
void DxfBuilder::addLayer(DxfLayerObject *object) {
    layers[object->data.name] = object;
}

void DxfBuilder::addLineType(DxfLineTypeObject *object) {
    otherObjects[object->data.name] = object;
}

void DxfBuilder::addBlockRecord(DxfBlockRecordObject *object) {
    blockRecords[object->data.name] = object;
}

// Entities
void DxfBuilder::addEntity(DxfEntity *entity) {
    if (layerEntities[entity->data.layerName].empty()) layerEntities[entity->data.layerName] = {};
    layerEntities[entity->data.layerName].push_back(entity);
}

// Blocks
void DxfBuilder::addBlock(DxfBlock *block) {
    blocks[block->name] = block;

    if (blockEntities[block->name].empty()) blockEntities[block->name] = {};
    for (DxfEntity* entity : block->entities) blockEntities[block->name].push_back(entity);
}

DxfDrawing DxfBuilder::getDrawing() {
    return DxfDrawing(layers, blocks, otherObjects, layerEntities, blockRecords, blockEntities);
}

} // namespace corecvs