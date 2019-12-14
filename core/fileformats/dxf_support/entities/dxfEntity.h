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
    explicit DxfEntity(DxfEntityData *data) : data(data) {}
    virtual void draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) {}
    virtual void print() const;
    virtual DxfEntityData* getData() { return data; };
private:
    DxfEntityData *data;
};

// LINE Entity
class DxfLineEntity : public DxfEntity {
public:
    explicit DxfLineEntity(DxfLineData *data) : DxfEntity(data), data(data) {}
    void draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) override;
    void print() const override;
    DxfLineData* getData() override { return data; };
private:
    DxfLineData *data;
};

// LWPOLYLINE Entity
class DxfLwPolylineEntity : public DxfEntity {
public:
    explicit DxfLwPolylineEntity(DxfLwPolylineData *data) : DxfEntity(data), data(data) {}
    void draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) override;
    void print() const override;
    DxfLwPolylineData* getData() override { return data; };
private:
    DxfLwPolylineData *data;
};

// POLYLINE Entity
class DxfPolylineEntity : public DxfEntity {
public:
    explicit DxfPolylineEntity(DxfPolylineData *data) : DxfEntity(data), data(data) {}
    void draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) override;
    void print() const override;
    DxfPolylineData* getData() override { return data; };
private:
    DxfPolylineData *data;
};

// CIRCLE Entity
class DxfCircleEntity : public DxfEntity {
public:
    explicit DxfCircleEntity(DxfCircleData *data) : DxfEntity(data), data(data) {}
    void draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) override;
    void print() const override;
    DxfCircleData* getData() override { return data; };
private:
    DxfCircleData *data;
};

// ARC Entity
class DxfCircularArcEntity : public DxfEntity {
public:
    explicit DxfCircularArcEntity(DxfCircularArcData *data) : DxfEntity(data), data(data) {}
    void draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) override;
    void print() const override;
    DxfCircularArcData* getData() override { return data; };
private:
    DxfCircularArcData *data;
};

// ELLIPSE Entity
class DxfEllipticalArcEntity : public DxfEntity {
public:
    explicit DxfEllipticalArcEntity(DxfEllipticalArcData *data) : DxfEntity(data), data(data) {}
    void draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) override;
    void print() const override;
    DxfEllipticalArcData* getData() override { return data; };
private:
    DxfEllipticalArcData *data;
};

} // namespace corecvs

#endif //DXF_SUPPORT_DXFENTITY_H
