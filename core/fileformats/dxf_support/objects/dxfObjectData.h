//
// Created by Myasnikov Vladislav on 29.10.2019.
//

#ifndef DXF_SUPPORT_DXFOBJECTDATA_H
#define DXF_SUPPORT_DXFOBJECTDATA_H

#include <string>

namespace corecvs {

// Abstract Object Data
class DxfObjectData {
public:
    DxfObjectData(std::string handle, std::string name, std::string ownerDictionaryID)
    : handle(std::move(handle)), name(std::move(name)), ownerDictionaryID(std::move(ownerDictionaryID)) {}
    DxfObjectData(const DxfObjectData &data) = default;

    std::string handle;
    std::string name;
    std::string ownerDictionaryID;
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

// BLOCK_RECORD Data
class DxfBlockRecordData : public DxfObjectData {
public:
    DxfBlockRecordData(const DxfObjectData &data, int scalability)
    : DxfObjectData(data), scalability(scalability) {}

    int scalability;
};

}

#endif //DXF_SUPPORT_DXFOBJECTDATA_H
