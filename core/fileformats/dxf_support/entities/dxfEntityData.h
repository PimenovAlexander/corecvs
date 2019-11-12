//
// Created by Myasnikov Vladislav on 10/29/19.
//

#ifndef DXF_SUPPORT_DXFENTITYDATA_H
#define DXF_SUPPORT_DXFENTITYDATA_H

#include <string>

namespace corecvs {

// Abstract Entity Data
class DxfEntityData {
public:
    DxfEntityData(int handle, int flags, std::string &layerName)
    : handle(handle),
    flags(flags),
    layerName(layerName) {}

    explicit DxfEntityData(DxfEntityData *data)
    : handle(data->handle),
    flags(data->flags),
    layerName(data->layerName) {}

    int handle;
    int flags;
    std::string layerName;
};

// LINE Data
class DxfLineData : public DxfEntityData {
public:
    DxfLineData(DxfEntityData *data, double x1, double y1, double z1, double x2, double y2, double z2)
    : DxfEntityData(data),
    x1(x1), y1(y1), z1(z1),
    x2(x2), y2(y2), z2(z2) {}

    double x1, y1, z1;
    double x2, y2, z2;
};

} // namespace corecvs

#endif //DXF_SUPPORT_DXFENTITYDATA_H
