//
// Created by Myasnikov Vladislav on 23.12.2019.
//

#include "core/fileformats/dxf_support/blocks/dxfBlock.h"
#include "core/buffers/rgb24/rgb24Buffer.h"

namespace corecvs {

void DxfBlock::print() {
    std::cout << "* * * Block * * *" << std::endl;
    std::cout << "ID: " << (id.empty() ? "none" : id) << std::endl;
    std::cout << "Block record ID: " << (blockRecordID.empty() ? "none" : blockRecordID) << std::endl;
    std::cout << "Name: " << name << std::endl;
    std::cout << "Layer name: " << layerName << std::endl;
    std::cout << "Base point: " << basePoint << std::endl;
    std::cout << "Entity amount: " << entities.size() << std::endl;
    std::cout << "Space: " << (isModelSpace ? "Model" : "Paper") << std::endl;
    std::cout << std::endl;
}

} // namespace corecvs