//
// Created by Myasnikov Vladislav on 23.12.2019.
//

#ifndef DXF_SUPPORT_DXFBLOCK_H
#define DXF_SUPPORT_DXFBLOCK_H

#include <string>
#include "fileformats/dxf_support/entities/dxfEntity.h"
#include "buffers/rgb24/rgb24Buffer.h"
#include "math/vector/vector3d.h"

namespace corecvs {

class DxfEntity;

class DxfBlock {
public:
    DxfBlock(std::string id, std::string name, std::string layerName, Vector3dd basePoint, std::string blockRecordID, bool isModelSpace)
    : id(std::move(id)),
      name(std::move(name)),
      layerName(std::move(layerName)),
      basePoint(basePoint),
      blockRecordID(std::move(blockRecordID)),
      isModelSpace(isModelSpace)
    {}

    void print();

    std::string id;
    std::string name;
    std::string layerName;
    std::string blockRecordID;
    Vector3dd basePoint;
    bool isModelSpace; // otherwise: paper space
    std::list<DxfEntity*> entities = {};
};

} // namespace corecvs

#endif //DXF_SUPPORT_DXFBLOCK_H
