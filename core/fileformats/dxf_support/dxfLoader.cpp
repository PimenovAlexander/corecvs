//
// Created by Myasnikov Vladislav on 10/17/19.
//

#include "core/fileformats/dxf_support/dxfLoader.h"
#include "core/fileformats/dxf_support/dxfCodes.h"
#include "core/fileformats/dxf_support/implDxfBuilder.h"
#include "core/utils/utils.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/rgb24/rgbColor.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace corecvs {

int DxfLoader::load(const std::string &fileName, corecvs::IDxfBuilder *dxfBuilder) {
    std::ifstream stream(fileName);
    if (!stream) {
        return -1;
    }

    int linePosition = 1;
    std::string codeString;
    std::string valueString;

    while (!stream.eof()) {
        if (DxfLoader::getTruncatedLine(codeString, stream) && DxfLoader::getTruncatedLine(valueString, stream)) {
            int currentCode;
            std::istringstream ss(codeString);
            ss >> currentCode;

            if (ss.fail()) {
                std::cout << "Error. Group code must be integer!" << std::endl;
            } else if (!ss.eof()) {
                std::cout << "Error. Group code must be alone at the line!" << std::endl;
            }

            linePosition += 2;
            processDxfPair(dxfBuilder, currentCode, valueString);
        }
    }
    return 0;
}

int DxfLoader::processDxfPair(corecvs::IDxfBuilder *dxfBuilder, int code, const std::string &value) {
    switch (code) {
        case DxfCodes::DXF_ENTITY_SEPARATOR_CODE:
            std::cout << "Separator code : " << value << std::endl;
            if (currentElementType != DxfElementType::DXF_UNKNOWN_TYPE) {
                switch (currentElementType) {
                    case DxfElementType::DXF_LAYER:
                        addLayer(dxfBuilder);
                        break;
                    case DxfElementType::DXF_LINE_TYPE:
                        addLineType(dxfBuilder);
                        break;
                    case DxfElementType::DXF_LINE:
                        addLine(dxfBuilder);
                        break;
                    case DxfElementType::DXF_UNKNOWN_TYPE:
                        break;
                }

                rawValues.clear();

                currentElementType = DxfCodes::getElementType(value);
            } else {
                currentElementType = DxfCodes::getElementType(value);
            }
            if (!variableName.empty()) {
                addVariable(dxfBuilder);
                variableName.clear();
                rawValues.clear();
            }
            break;
        case DxfCodes::DXF_VARIABLE_CODE:
            if (!variableName.empty()) addVariable(dxfBuilder);
            variableName = value;
            rawValues.clear();
            break;
        default:
            rawValues[code] = value;
            if (variableName.empty())
            std::cout << "Error. Unknown group code! : " << code << " : " << value << std::endl;
    }
    return 0;
}

bool DxfLoader::getTruncatedLine(std::string &s, std::istream &stream) {
    if (!stream.eof()) {
        std::string line;
        getline(stream, line);
        int i = 0;
        int j = (int) line.size() - 1;
        while ((line[i] == ' ' || line[i] == '\t') && i <= j) i++;
        if (i <= j) {
            while ((line[j] == ' ' || line[j] == '\t' || line[j] == '\n' || line[j] == '\r') && j >= i) j--;
            if (i <= j) {
                s = line.substr(i, j - i + 1);
            } else {
                s = "";
            }
            return true;
        } else {
            return false;
        }
    } else {
        s[0] = '\0';
        return false;
    }
}

DxfEntityData* DxfLoader::getEntityData() {
    auto data = new DxfEntityData();
    data->handle = getIntValue(DxfCodes::DXF_HANDLE_CODE, 0);
    data->flags = getIntValue(DxfCodes::DXF_FLAGS_CODE, 0);
    data->layerName = getStringValue(DxfCodes::DXF_LAYER_NAME_CODE, "");
    data->lineTypeName = getStringValue(DxfCodes::DXF_LINE_TYPE_NAME_CODE, DxfCodes::DXF_LINE_TYPE_NAME_DEFAULT);
    auto rgb = DxfCodes::getRGB(getIntValue(DxfCodes::DXF_COLOR_NUMBER_CODE, DxfCodes::DXF_COLOR_NUMBER_DEFAULT));
    if (!rgb.empty()) data->rgbColor = RGBColor(rgb[0], rgb[1], rgb[2]);
    return data;
}

DxfObjectData* DxfLoader::getObjectData() {
    auto handle = getIntValue(DxfCodes::DXF_HANDLE_CODE, 0);
    auto flags = getIntValue(DxfCodes::DXF_FLAGS_CODE, 0);
    auto name = getStringValue(DxfCodes::DXF_ELEMENT_NAME_CODE, "");
    auto data = new DxfObjectData(handle, flags, name);
    return data;
}

void DxfLoader::addVariable(corecvs::IDxfBuilder *dxfBuilder) {
    auto codes = DxfCodes::getVariableCodes(variableName);
    if (codes.empty()) {
        std::cout << "Error. Unknown variable: " << variableName << std::endl;
        return;
    }
    std::vector<int> types;
    std::vector<std::string> values;
    for (int code : codes) {
        if (!hasRawValue(code)) {
            std::cout << "Error. No value for code: " << code << " for variable: " << variableName << std::endl;
            return;
        } else {
            types.emplace_back(DxfCodes::getCodeType(code));
            values.emplace_back(rawValues[code]);
        }
    }

    if (codes.size() == 1) {
        int code = codes[0];
        switch (types[0]) {
            case DxfCodes::DXF_INT16:
            case DxfCodes::DXF_INT32:
            case DxfCodes::DXF_INT64:
                dxfBuilder->setIntVariable(code, variableName, getIntValue(code, 0));
                break;
            case DxfCodes::DXF_DOUBLE:
                dxfBuilder->setDoubleVariable(code, variableName, getDoubleValue(code, 0));
                break;
            case DxfCodes::DXF_STRING:
                dxfBuilder->setStringVariable(code, variableName, getStringValue(code, ""));
                break;
        }
    } else if (codes.size() == 2) {
        dxfBuilder->set2DVectorVariable(codes[0], variableName, getDoubleValue(codes[0], 0), getDoubleValue(codes[1], 0));
    } else if (codes.size() == 3) {
        dxfBuilder->set3DVectorVariable(codes[0], variableName, getDoubleValue(codes[0], 0), getDoubleValue(codes[1], 0), getDoubleValue(codes[2], 0));
    }
}

void DxfLoader::addLayer(corecvs::IDxfBuilder *dxfBuilder) {
    auto baseData = getObjectData();
    auto allData = new DxfLayerData(baseData, getIntValue(DxfCodes::DXF_COLOR_NUMBER_CODE, 0), (bool) getIntValue(290, 0), getStringValue(DxfCodes::DXF_LINE_TYPE_NAME_CODE, ""));
    dxfBuilder->addLayer(new DxfLayerObject(allData));
    delete baseData;
}

void DxfLoader::addLineType(corecvs::IDxfBuilder *dxfBuilder) {
    auto baseData = getObjectData();
    auto allData = new DxfLineTypeData(baseData, getIntValue(73, 0), getDoubleValue(40, 0));
    dxfBuilder->addLineType(new DxfLineTypeObject(allData));
    delete baseData;
}

void DxfLoader::addLine(corecvs::IDxfBuilder *dxfBuilder) {
    auto baseData = getEntityData();
    auto allData = new DxfLineData(baseData, getDoubleValue(10, 0), getDoubleValue(20, 0), getDoubleValue(30, 0),
                               getDoubleValue(11, 0), getDoubleValue(21, 0),getDoubleValue (31, 0));
    dxfBuilder->addLine(new DxfLineEntity(allData));
    delete baseData;
}

// DXFToRGB24BufferLoader implementation
string DXFToRGB24BufferLoader::extension = ".dxf";

bool DXFToRGB24BufferLoader::acceptsFile(std::string const &name) {
    std::string lowercase = name;
    std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), ::tolower);
    return HelperUtils::endsWith(lowercase, extension);
}

RGB24Buffer* DXFToRGB24BufferLoader::load(const string &name) {
    DxfLoader loader;
    ImplDxfBuilder builder;
    int resultCode = loader.load(name, &builder);
    if (resultCode == -1) {
        std::cout << "Error. Can't open file: " << name << std::endl;
        return NULL;
    } else {
         std::cout << "DXF dile is loaded: " << name << " result code: " << resultCode << std::endl;
         return builder.draw();
    }
}

} // namespace corecvs