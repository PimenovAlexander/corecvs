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
    DxfEntityData(int handle, std::string layerName, std::string lineTypeName, int colorNumber, bool isVisible)
    : handle(handle), layerName(std::move(layerName)), lineTypeName(std::move(lineTypeName)), colorNumber(colorNumber), isVisible(isVisible) {}
    DxfEntityData(const DxfEntityData &data) = default;

    int handle;
    std::string layerName;
    std::string lineTypeName;
    RGBColor rgbColor;
    int colorNumber;
    bool isVisible;
};

// LINE Data
class DxfLineData : public DxfEntityData {
public:
    DxfLineData(const DxfEntityData &data, Vector3dd startPoint, Vector3dd endPoint)
    : DxfEntityData(data), startPoint(startPoint), endPoint(endPoint) {}

    Vector3dd startPoint;
    Vector3dd endPoint;
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

// POLYLINE Data
class DxfPolylineData : public DxfEntityData {
public:
    DxfPolylineData(const DxfEntityData &data, double thickness, bool isClosed)
    : DxfEntityData(data), thickness(thickness), isClosed(isClosed) {}

    std::vector<Vector3dd> vertices;
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

} // namespace corecvs

#endif //DXF_SUPPORT_DXFENTITYDATA_H
