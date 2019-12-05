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
    DxfLineData(DxfEntityData *data, double x1, double y1, double z1, double x2, double y2, double z2)
    : DxfEntityData(data), x1(x1), y1(y1), z1(z1), x2(x2), y2(y2), z2(z2) {}

    double x1, y1, z1;
    double x2, y2, z2;
};

// LWPOLYLINE Data
class DxfLwPolylineData : public DxfEntityData {
public:
    DxfLwPolylineData(DxfEntityData *data, int vertexNumber, std::vector<Vector2d<double>> &vertices)
    : DxfEntityData(data), vertexNumber(vertexNumber), vertices(vertices) {}

    int vertexNumber;
    std::vector<Vector2d<double>> vertices;
};

// POLYLINE Data
class DxfPolylineData : public DxfEntityData {
public:
    DxfPolylineData(DxfEntityData *data, std::vector<Vector3d<double>> &vertices)
    : DxfEntityData(data), vertices(vertices) {}

    std::vector<Vector3d<double>> vertices;
};

} // namespace corecvs

#endif //DXF_SUPPORT_DXFENTITYDATA_H
