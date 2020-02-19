//
// Created by Myasnikov Vladislav on 27.10.2019.
//

#ifndef DXF_SUPPORT_DXFENTITY_H
#define DXF_SUPPORT_DXFENTITY_H

#include <string>
#include "fileformats/dxf_support/dxfDrawing.h"
#include "buffers/rgb24/rgb24Buffer.h"
#include "fileformats/dxf_support/dxfCodes.h"
#include "fileformats/dxf_support/entities/dxfEntityData.h"

namespace corecvs {

class DxfDrawing;

// Abstract Entity
class DxfEntity {
public:
    explicit DxfEntity(DxfEntityData &data)
    : data(data) {}

    virtual void draw(RGB24Buffer */*buffer*/, DxfDrawing */*drawing*/) {}
    virtual void print();
    virtual Rectangled getBoundingBox() {
        return Rectangled();
    }

    DxfEntityData &data;

    virtual ~DxfEntity() {};
};

// LINE Entity
class DxfLineEntity : public DxfEntity {
public:
    explicit DxfLineEntity(DxfLineData &data)
    : DxfEntity(data), data(data) {}

    void draw(RGB24Buffer */*buffer*/, DxfDrawing */*drawing*/) override;
    void print() override;
    Rectangled getBoundingBox() override;

    DxfLineData &data;
};

// LWPOLYLINE Entity
class DxfLwPolylineEntity : public DxfEntity {
public:
    explicit DxfLwPolylineEntity(DxfLwPolylineData &data)
    : DxfEntity(data), data(data) {}

    void draw(RGB24Buffer */*buffer*/, DxfDrawing */*drawing*/) override;
    void print() override;
    Rectangled getBoundingBox() override;

    DxfLwPolylineData &data;
};

// POLYLINE Entity
class DxfPolylineEntity : public DxfEntity {
public:
    explicit DxfPolylineEntity(DxfPolylineData &data)
    : DxfEntity(data), data(data) {}

    void draw(RGB24Buffer */*buffer*/, DxfDrawing */*drawing*/) override;
    void print() override;
    Rectangled getBoundingBox() override;

    DxfPolylineData &data;
};

// CIRCLE Entity
class DxfCircleEntity : public DxfEntity {
public:
    explicit DxfCircleEntity(DxfCircleData &data)
    : DxfEntity(data), data(data) {}

    void draw(RGB24Buffer */*buffer*/, DxfDrawing */*drawing*/) override;
    void print() override;
    Rectangled getBoundingBox() override;

    DxfCircleData &data;
};

// ARC Entity
class DxfCircularArcEntity : public DxfEntity {
public:
    explicit DxfCircularArcEntity(DxfCircularArcData &data)
    : DxfEntity(data), data(data) {}

    void draw(RGB24Buffer */*buffer*/, DxfDrawing */*drawing*/) override;
    void print() override;
    Rectangled getBoundingBox() override;

    DxfCircularArcData &data;
};

// ELLIPSE Entity
class DxfEllipticalArcEntity : public DxfEntity {
public:
    explicit DxfEllipticalArcEntity(DxfEllipticalArcData &data)
    : DxfEntity(data), data(data) {}

    void draw(RGB24Buffer */*buffer*/, DxfDrawing */*drawing*/) override;
    void print() override;
    Rectangled getBoundingBox() override;

    DxfEllipticalArcData &data;
};

// POINT Entity
class DxfPointEntity : public DxfEntity {
public:
    explicit DxfPointEntity(DxfPointData &data)
    : DxfEntity(data), data(data) {}

    void draw(RGB24Buffer */*buffer*/, DxfDrawing */*drawing*/) override;
    void print() override;
    Rectangled getBoundingBox() override;

    DxfPointData &data;
};

// INSERT Entity
class DxfBlockReferenceEntity : public DxfEntity {
public:
    explicit DxfBlockReferenceEntity(DxfBlockReferenceData &data)
    : DxfEntity(data), data(data) {}

    void draw(RGB24Buffer */*buffer*/, DxfDrawing */*drawing*/) override;
    void print() override;
    Rectangled getBoundingBox() override;

    DxfBlockReferenceData &data;
};

} // namespace corecvs

#endif //DXF_SUPPORT_DXFENTITY_H
