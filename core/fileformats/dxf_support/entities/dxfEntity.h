//
// Created by Myasnikov Vladislav on 10/27/19.
//

#ifndef DXF_SUPPORT_DXFENTITY_H
#define DXF_SUPPORT_DXFENTITY_H

#include <string>
#include <core/fileformats/dxf_support/dxfDrawingAttrs.h>
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/dxf_support/dxfCodes.h"
#include "core/fileformats/dxf_support/entities/dxfEntityData.h"

namespace corecvs {

// Abstract Entity
class DxfEntity {
public:
    explicit DxfEntity(DxfEntityData &data)
    : data(data) {}

    virtual ~DxfEntity() { delete &data; }

    virtual void draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) {}
    virtual void print();
    virtual std::pair<Vector2dd,Vector2dd> getBoundingBox() {}

    DxfEntityData &data;
};

// LINE Entity
class DxfLineEntity : public DxfEntity {
public:
    explicit DxfLineEntity(DxfLineData &data)
    : DxfEntity(data), data(data) {}

    ~DxfLineEntity() override { delete &data; }

    void draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) override;
    void print() override;
    std::pair<Vector2dd,Vector2dd> getBoundingBox() override;

    DxfLineData &data;
};

// LWPOLYLINE Entity
class DxfLwPolylineEntity : public DxfEntity {
public:
    explicit DxfLwPolylineEntity(DxfLwPolylineData &data)
    : DxfEntity(data), data(data) {}

    ~DxfLwPolylineEntity() override { delete &data; }

    void draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) override;
    void print() override;
    std::pair<Vector2dd,Vector2dd> getBoundingBox() override;

    DxfLwPolylineData &data;
};

// POLYLINE Entity
class DxfPolylineEntity : public DxfEntity {
public:
    explicit DxfPolylineEntity(DxfPolylineData &data)
    : DxfEntity(data), data(data) {}

    ~DxfPolylineEntity() override { delete &data; }

    void draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) override;
    void print() override;
    std::pair<Vector2dd,Vector2dd> getBoundingBox() override;

    DxfPolylineData &data;
};

// CIRCLE Entity
class DxfCircleEntity : public DxfEntity {
public:
    explicit DxfCircleEntity(DxfCircleData &data)
    : DxfEntity(data), data(data) {}

    ~DxfCircleEntity() override { delete &data; }

    void draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) override;
    void print() override;
    std::pair<Vector2dd,Vector2dd> getBoundingBox() override;

    DxfCircleData &data;
};

// ARC Entity
class DxfCircularArcEntity : public DxfEntity {
public:
    explicit DxfCircularArcEntity(DxfCircularArcData &data)
    : DxfEntity(data), data(data) {}

    ~DxfCircularArcEntity() override { delete &data; }

    void draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) override;
    void print() override;
    std::pair<Vector2dd,Vector2dd> getBoundingBox() override;

    DxfCircularArcData &data;
};

// ELLIPSE Entity
class DxfEllipticalArcEntity : public DxfEntity {
public:
    explicit DxfEllipticalArcEntity(DxfEllipticalArcData &data)
    : DxfEntity(data), data(data) {}

    ~DxfEllipticalArcEntity() override { delete &data; }

    void draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) override;
    void print() override;
    std::pair<Vector2dd,Vector2dd> getBoundingBox() override;

    DxfEllipticalArcData &data;
};

// POINT Entity
class DxfPointEntity : public DxfEntity {
public:
    explicit DxfPointEntity(DxfPointData &data)
    : DxfEntity(data), data(data) {}

    ~DxfPointEntity() override { delete &data; }

    void draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) override;
    void print() override;
    std::pair<Vector2dd,Vector2dd> getBoundingBox() override;

    DxfPointData &data;
};

} // namespace corecvs

#endif //DXF_SUPPORT_DXFENTITY_H
