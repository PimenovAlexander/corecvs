//
// Created by Myasnikov Vladislav on 10/27/19.
//

#include "core/fileformats/dxf_support/entities/dxfEntity.h"
#include <iostream>

namespace corecvs {

// Data printing
void DxfEntity::print() const {
    std::cout << "Handle: " << data->handle << std::endl;
    std::cout << "Flags: " << data->flags << std::endl;
    std::cout << "Layer name: " << data->layerName << std::endl;
    std::cout << "Line type name: " << data->lineTypeName << std::endl;
}

void DxfLineEntity::print() const {
    std::cout << "* * * Line Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Start point: " << data->x1 << ", " << data->y1 << ", " << data->z1 << std::endl;
    std::cout << "End point: " << data->x2 << ", " << data->y2 << ", " << data->z2 << std::endl;
    std::cout << std::endl;
}

// Drawing
void DxfLineEntity::draw(RGB24Buffer *buffer, DxfDrawingUnits units) {
    auto x1 = DxfCodes::getDrawingValue(data->x1, units);
    auto y1 = DxfCodes::getDrawingValue(data->y1, units);
    auto x2 = DxfCodes::getDrawingValue(data->x2, units);
    auto y2 = DxfCodes::getDrawingValue(data->y2, units);
    buffer->drawLine(x1, y1, x2, y2, data->rgbColor);
}

} // namespace corecvs
