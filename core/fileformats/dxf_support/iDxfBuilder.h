//
// Created by Myasnikov Vladislav on 10/17/19.
//

#ifndef DXF_SUPPORT_IDXFBUILDER_H
#define DXF_SUPPORT_IDXFBUILDER_H

#include <string>
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/dxf_support/objects/dxfObject.h"
#include "core/fileformats/dxf_support/entities/dxfEntity.h"

namespace corecvs {

class IDxfBuilder {
public:
    // Variables
    virtual void setIntVariable(int code, std::string const &name, int value) {}
    virtual void setDoubleVariable(int code, std::string const &name, double value) {}
    virtual void setStringVariable(int code, std::string const &name, std::string const &value) {}
    virtual void set2DVectorVariable(int code, std::string const &name,  double x, double y) {}
    virtual void set3DVectorVariable(int code, std::string const &name,  double x, double y, double z) {}

    // Objects
    virtual void addLayer(DxfLayerObject *object) {}
    virtual void addLineType(DxfLineTypeObject *object) {}

    // Entities
    virtual void addLine(DxfLineEntity *entity) {}
    virtual void addLwPolyline(DxfLwPolylineEntity *entity) {}
    virtual void addEntity(DxfEntity *entity) {}

    virtual RGB24Buffer* draw() {}
};

} // namespace corecvs

#endif //DXF_SUPPORT_CUSTOMDXFBUILDER_H
