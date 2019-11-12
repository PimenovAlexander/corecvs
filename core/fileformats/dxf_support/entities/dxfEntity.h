//
// Created by Myasnikov Vladislav on 10/27/19.
//

#ifndef DXF_SUPPORT_DXFENTITY_H
#define DXF_SUPPORT_DXFENTITY_H

#include <string>
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/dxf_support/entities/dxfEntityData.h"

namespace corecvs {

// Abstract Entity
class DxfEntity {
public:
    explicit DxfEntity(DxfEntityData *data)
    : data(data) {}

    virtual void draw(RGB24Buffer *buffer) {}
    virtual void print() const;

private:
    DxfEntityData *data;
};

// LINE Entity
class DxfLineEntity : public DxfEntity {
public:
    explicit DxfLineEntity(DxfLineData *data)
    : DxfEntity(data),
    data(data) {}

    void draw(RGB24Buffer *buffer) override;
    void print() const override;

    DxfLineData *data;
};

} // namespace corecvs

#endif //DXF_SUPPORT_DXFENTITY_H
