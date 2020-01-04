//
// Created by Myasnikov Vladislav on 29.10.2019.
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

    virtual void print();

    DxfObjectData &data;

    ~DxfObject() {}
};

// LAYER Object
class DxfLayerObject : public DxfObject {
public:
    explicit DxfLayerObject(DxfLayerData &data)
    : DxfObject(data), data(data) {}

    void print() override;

    DxfLayerData &data;

    ~DxfLayerObject() {}
};

// LINETYPE Object
class DxfLineTypeObject : public DxfObject {
public:
    explicit DxfLineTypeObject(DxfLineTypeData &data)
    : DxfObject(data), data(data) {}

    void print() override;

    DxfLineTypeData &data;

    ~DxfLineTypeObject() {}
};

// BLOCK_RECORD Object
class DxfBlockRecordObject : public DxfObject {
public:
    explicit DxfBlockRecordObject(DxfBlockRecordData &data)
    : DxfObject(data), data(data) {}

    void print() override;

    DxfBlockRecordData &data;

    ~DxfBlockRecordObject() {}
};

} // namespace corecvs

#endif //DXF_SUPPORT_DXFOBJECT_H
