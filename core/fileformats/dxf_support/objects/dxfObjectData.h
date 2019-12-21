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
    DxfObjectData(int handle, std::string name)
    : handle(handle), name(std::move(name)) {}
    DxfObjectData(const DxfObjectData &data) = default;

    int handle;
    std::string name;
};

// LAYER Data
class DxfLayerData : public DxfObjectData {
public:
    DxfLayerData(const DxfObjectData &data, int colorNumber, bool isPlotted, std::string lineTypeName)
    : DxfObjectData(data), colorNumber(colorNumber), isPlotted(isPlotted), lineTypeName(std::move(lineTypeName)) {}

    int colorNumber;
    bool isPlotted;
    std::string lineTypeName;
};

// LINETYPE Data
class DxfLineTypeData : public DxfObjectData {
public:
    DxfLineTypeData(const DxfObjectData &data, int elementAmount, double patternLength)
    : DxfObjectData(data), elementAmount(elementAmount), patternLength(patternLength) {}

    int elementAmount;
    double patternLength;
};

}

#endif //DXF_SUPPORT_DXFOBJECTDATA_H
