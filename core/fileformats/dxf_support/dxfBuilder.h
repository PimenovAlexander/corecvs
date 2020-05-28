//
// Created by Myasnikov Vladislav on 21.10.2019.
//

#ifndef DXF_SUPPORT_DXFBUILDER_H
#define DXF_SUPPORT_DXFBUILDER_H

#include <string>
#include <list>
#include <map>
#include "fileformats/dxf_support/dxfCodes.h"
#include "fileformats/dxf_support/dxfDrawing.h"
#include "fileformats/dxf_support/objects/dxfObject.h"
#include "fileformats/dxf_support/entities/dxfEntity.h"
#include "fileformats/dxf_support/blocks/dxfBlock.h"
#include "math/vector/vector3d.h"

namespace corecvs {

class DxfBuilder {
public:
    // Variables
    void setIntVariable(int code, std::string const &name, int value);
    void setDoubleVariable(int code, std::string const &name, double value);
    void setStringVariable(int code, std::string const &name, std::string const &value);
    void set2DVectorVariable(int code, std::string const &name, double x, double y);
    void set3DVectorVariable(int code, std::string const &name, double x, double y, double z);

    // Objects
    void addLayer(DxfLayerObject *object);
    void addLineType(DxfLineTypeObject *object);
    void addBlockRecord(DxfBlockRecordObject *object);

    // Entities
    void addEntity(DxfEntity *entity);

    // Blocks
    void addBlock(DxfBlock *block);

    DxfDrawing getDrawing();

private:
    std::map<std::string,DxfLayerObject*> layers;
    std::map<std::string,DxfBlockRecordObject*> blockRecords;
    std::map<std::string,DxfObject*> otherObjects;
    std::map<std::string,DxfBlock*> blocks;
    std::map<std::string,std::list<DxfEntity*>> layerEntities;
    std::map<std::string,std::list<DxfEntity*>> blockEntities;
};

} // namespace corecvs

#endif //DXF_SUPPORT_DXFBUILDER_H
