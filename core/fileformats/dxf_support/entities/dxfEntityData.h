//
// Created by Myasnikov Vladislav on 10/29/19.
//

#ifndef DXF_SUPPORT_DXFENTITYDATA_H
#define DXF_SUPPORT_DXFENTITYDATA_H

#include <string>
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/math/vector/vector2d.h"
#include "core/math/vector/vector3d.h"

namespace corecvs {

// Abstract Entity Data
class DxfEntityData {
public:
    DxfEntityData() = default;
    explicit DxfEntityData(DxfEntityData *data)
    : handle(data->handle), flags(data->flags), layerName(data->layerName), lineTypeName(data->lineTypeName), rgbColor(data->rgbColor), colorNumber(data->colorNumber) {}
    int handle;
    int flags;
    std::string layerName;
    std::string lineTypeName;
    RGBColor rgbColor;
    int colorNumber;
};

// LINE Data
class DxfLineData : public DxfEntityData {
public:
    DxfLineData(DxfEntityData *data, Vector3dd startPoint, Vector3dd endPoint)
    : DxfEntityData(data), startPoint(startPoint), endPoint(endPoint) {}
    Vector3dd startPoint;
    Vector3dd endPoint;
};

// LWPOLYLINE Data
class DxfLwPolylineData : public DxfEntityData {
public:
    DxfLwPolylineData(DxfEntityData *data, int vertexNumber, std::vector<Vector2dd> &vertices)
    : DxfEntityData(data), vertexNumber(vertexNumber), vertices(vertices) {}
    int vertexNumber;
    std::vector<Vector2dd> vertices;
};

// POLYLINE Data
class DxfPolylineData : public DxfEntityData {
public:
    DxfPolylineData(DxfEntityData *data, std::vector<Vector3dd> &vertices)
    : DxfEntityData(data), vertices(vertices) {}
    std::vector<Vector3dd> vertices;
};

// CIRCLE Data
class DxfCircleData : public DxfEntityData {
public:
    DxfCircleData(DxfEntityData *data, Vector3dd center, double radius, double thickness)
    : DxfEntityData(data), center(center), radius(radius), thickness(thickness) {}
    Vector3dd center;
    double radius;
    double thickness;
};

// ARC Data
class DxfCircularArcData : public DxfEntityData {
public:
    DxfCircularArcData(DxfEntityData *data, Vector3dd center, double radius, double thickness, double startAngle, double endAngle)
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
    DxfEllipticalArcData(DxfEntityData *data, Vector3dd center, Vector3dd majorAxisEndPoint, double ratio, double startAngle, double endAngle)
    : DxfEntityData(data), center(center), majorAxisEndPoint(majorAxisEndPoint), ratio(ratio), startAngle(startAngle), endAngle(endAngle) {}
    Vector3dd center;
    Vector3dd majorAxisEndPoint;
    double ratio;
    double startAngle;
    double endAngle;
};

} // namespace corecvs

#endif //DXF_SUPPORT_DXFENTITYDATA_H
