//
// Created by Myasnikov Vladislav on 10/29/19.
//

#include <iostream>
#include "core/fileformats/dxf_support/objects/dxfObject.h"

namespace corecvs {

void DxfObject::print() const {
    std::cout << "Handle: " << data->handle << std::endl;
    std::cout << "Flags: " << data->flags << std::endl;
    std::cout << "Name: " << data->name << std::endl;
}

void DxfLayerObject::print() const {
    std::cout << "* * * Layer Object * * *" << std::endl;
    DxfObject::print();
    std::cout << "Color number: " << data->colorNumber << std::endl;
    std::cout << "Plotting flag: " << data->plottingFlag << std::endl;
    std::cout << "Line type name: " << data->lineTypeName << std::endl;
    std::cout << std::endl;
}

void DxfLineTypeObject::print() const {
    std::cout << "* * * Line Type Object * * *" << std::endl;
    DxfObject::print();
    std::cout << "Element amount: " << data->elementAmount << std::endl;
    std::cout << "Pattern length: " << data->patternLength << std::endl;
    std::cout << std::endl;
}

} // namespace corecvs