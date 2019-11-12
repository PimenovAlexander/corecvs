//
// Created by Myasnikov Vladislav on 10/27/19.
//

#include "core/fileformats/dxf_support/entities/dxfEntity.h"
#include <iostream>

namespace corecvs {

void DxfEntity::print() const {
    std::cout << "Handle: " << data->handle << std::endl;
    std::cout << "Flags: " << data->flags << std::endl;
    std::cout << "Layer name: " << data->layerName << std::endl;
}

void DxfLineEntity::print() const {
    std::cout << "* * * Line Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Start point: " << data->x1 << ", " << data->y1 << ", " << data->z1 << std::endl;
    std::cout << "End point: " << data->x2 << ", " << data->y2 << ", " << data->z2 << std::endl;
    std::cout << "# # # Line Entity # # #" << std::endl;
}

void DxfLineEntity::draw(RGB24Buffer *buffer) {
    buffer->drawLine(data->x1, data->y1, data->x2, data->y2, RGBColor::Blue());
}

} // namespace corecvs
