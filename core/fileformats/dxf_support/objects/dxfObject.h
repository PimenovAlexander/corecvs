//
// Created by Myasnikov Vladislav on 10/29/19.
//

#ifndef DXF_SUPPORT_DXFOBJECT_H
#define DXF_SUPPORT_DXFOBJECT_H

#include <string>
#include "core/fileformats/dxf_support/objects/dxfObjectData.h"

namespace corecvs {

// Abstract Object
class DxfObject {
public:
    explicit DxfObject(DxfObjectData &data)
    : data(data) {}

    virtual ~DxfObject() { delete &data; }

    virtual void print();

    DxfObjectData &data;
};

// LAYER Object
class DxfLayerObject : public DxfObject {
public:
    explicit DxfLayerObject(DxfLayerData &data)
    : DxfObject(data), data(data) {}

    ~DxfLayerObject() override { delete &data; }

    void print() override;

    DxfLayerData &data;
};

// LINETYPE Object
class DxfLineTypeObject : public DxfObject {
public:
    explicit DxfLineTypeObject(DxfLineTypeData &data)
    : DxfObject(data), data(data) {}

    ~DxfLineTypeObject() override { delete &data; }

    void print() override;

    DxfLineTypeData &data;
};

} // namespace corecvs

#endif //DXF_SUPPORT_DXFOBJECT_H
