//
// Created by Myasnikov Vladislav on 29.10.2019.
//

#ifndef DXF_SUPPORT_DXFENTITYDATA_H
#define DXF_SUPPORT_DXFENTITYDATA_H

#include <string>
#include "buffers/rgb24/rgb24Buffer.h"
#include "math/vector/vector2d.h"
#include "math/vector/vector3d.h"
#include "fileformats/dxf_support/blocks/dxfBlock.h"
#include "fileformats/dxf_support/entities/dxfEntity.h"

namespace corecvs {

// Abstract Entity Data
class DxfEntityData {
public:
    DxfEntityData(std::string handle, std::string layerName, std::string lineTypeName, int colorNumber, bool isVisible, std::string blockRecordID, DxfBlock *block)
    : handle(std::move(handle)), layerName(std::move(layerName)), lineTypeName(std::move(lineTypeName)), colorNumber(colorNumber), isVisible(isVisible), blockRecordID(std::move(blockRecordID)), block(block) {}
    DxfEntityData(const DxfEntityData &data) = default;

    std::string handle;
    std::string layerName;
    std::string lineTypeName;
    RGBColor rgbColor;
    int colorNumber;
    bool isVisible;
    std::string blockRecordID;
    DxfBlock *block;
};

// LINE Data
class DxfLineData : public DxfEntityData {
public:
    DxfLineData(const DxfEntityData &data, Vector3dd startPoint, Vector3dd endPoint, double thickness)
    : DxfEntityData(data), startPoint(startPoint), endPoint(endPoint), thickness(thickness) {}

    Vector3dd startPoint;
    Vector3dd endPoint;
    double thickness;
};

// LWPOLYLINE Data
class DxfLwPolylineData : public DxfEntityData {
public:
    DxfLwPolylineData(const DxfEntityData &data, std::vector<Vector2dd> &vertices, double thickness, bool isClosed)
    : DxfEntityData(data), vertices(vertices), thickness(thickness), isClosed(isClosed) {}

    std::vector<Vector2dd> vertices;
    double thickness;
    bool isClosed;
};

// VERTEX Data
class DxfVertexData : public DxfEntityData {
public:
    DxfVertexData(const DxfEntityData &data, Vector3dd location, double bulge)
    : DxfEntityData(data), location(location), bulge(bulge) {}

    Vector3dd location;
    double bulge;
};

// POLYLINE Data
class DxfPolylineData : public DxfEntityData {
public:
    DxfPolylineData(const DxfEntityData &data, double thickness, bool isClosed)
    : DxfEntityData(data), thickness(thickness), isClosed(isClosed) {}

    std::vector<DxfVertexData*> vertices;
    double thickness;
    bool isClosed;
};

// CIRCLE Data
class DxfCircleData : public DxfEntityData {
public:
    DxfCircleData(const DxfEntityData &data, Vector3dd center, double radius, double thickness)
    : DxfEntityData(data), center(center), radius(radius), thickness(thickness) {}

    Vector3dd center;
    double radius;
    double thickness;
};

// ARC Data
class DxfCircularArcData : public DxfEntityData {
public:
    DxfCircularArcData(const DxfEntityData &data, Vector3dd center, double radius, double thickness, double startAngle, double endAngle)
    : DxfEntityData(data), center(center), radius(radius), thickness(thickness), startAngle(startAngle), endAngle(endAngle) {}

    Vector3dd center;
    double radius;
    double thickness;
    double startAngle;
    double endAngle;
};

// ELLIPSE Data
class DxfEllipticalArcData : public DxfEntityData {
public:
    DxfEllipticalArcData(const DxfEntityData &data, Vector3dd center, Vector3dd majorAxisEndPoint, double ratio, double startAngle, double endAngle)
    : DxfEntityData(data), center(center), majorAxisEndPoint(majorAxisEndPoint), ratio(ratio), startAngle(startAngle), endAngle(endAngle) {}

    Vector3dd center;
    Vector3dd majorAxisEndPoint;
    double ratio;
    double startAngle;
    double endAngle;
};

// POINT Data
class DxfPointData : public DxfEntityData {
public:
    DxfPointData(const DxfEntityData &data, Vector3dd location, double thickness)
    : DxfEntityData(data), location(location), thickness(thickness) {}

    Vector3dd location;
    double thickness;
};

// INSERT Data
class DxfBlockReferenceData : public DxfEntityData {
public:
    DxfBlockReferenceData(const DxfEntityData &data, std::string blockName, Vector3dd insertionPoint, Vector3dd scaleFactor, double rotationAngle)
    : DxfEntityData(data), blockName(std::move(blockName)), insertionPoint(insertionPoint), scaleFactor(scaleFactor), rotationAngle(rotationAngle) {}

    std::string blockName;
    Vector3dd insertionPoint;
    Vector3dd scaleFactor;
    double rotationAngle;
};

} // namespace corecvs

#endif //DXF_SUPPORT_DXFENTITYDATA_H
