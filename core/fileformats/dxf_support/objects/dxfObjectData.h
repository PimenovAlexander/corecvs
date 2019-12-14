//
// Created by Myasnikov Vladislav on 10/29/19.
//

#ifndef DXF_SUPPORT_DXFOBJECTDATA_H
#define DXF_SUPPORT_DXFOBJECTDATA_H

#include <string>

namespace corecvs {

// Abstract Object Data
class DxfObjectData {
public:
    DxfObjectData(int handle, int flags, std::string &name)
    : handle(handle), flags(flags), name(name) {}

    explicit DxfObjectData(DxfObjectData *data)
    : handle(data->handle), flags(data->flags), name(data->name) {}

    int handle;
    int flags;
    std::string name;
};

// LAYER Data
class DxfLayerData : public DxfObjectData {
public:
    DxfLayerData(DxfObjectData *data, int colorNumber, bool plottingFlag, std::string const &lineTypeName)
    : DxfObjectData(data), colorNumber(colorNumber), plottingFlag(plottingFlag), lineTypeName(lineTypeName) {}

    int colorNumber;
    bool plottingFlag;
    std::string lineTypeName;
};

// LINETYPE Data
class DxfLineTypeData : public DxfObjectData {
public:
    DxfLineTypeData(DxfObjectData *data, int elementAmount, double patternLength)
    : DxfObjectData(data), elementAmount(elementAmount), patternLength(patternLength) {}

    int elementAmount;
    double patternLength;
};

}

#endif //DXF_SUPPORT_DXFOBJECTDATA_H
