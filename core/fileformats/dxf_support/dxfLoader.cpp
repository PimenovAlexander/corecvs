//
// Created by Myasnikov Vladislav on 17.10.2019.
//

#include "core/fileformats/dxf_support/dxfLoader.h"
#include "core/fileformats/dxf_support/dxfCodes.h"
#include "core/fileformats/dxf_support/dxfBuilder.h"
#include "core/utils/utils.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/rgb24/rgbColor.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace corecvs {

int DxfLoader::load(const std::string &fileName) {
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
//                std::cout << "Error. Group code must be integer!" << std::endl;
            } else if (!ss.eof()) {
//                std::cout << "Error. Group code must be alone at the line!" << std::endl;
            }

            linePosition += 2;
            processDxfPair(currentCode, valueString);
        }
    }
    return 0;
}

int DxfLoader::processDxfPair(int code, const std::string &value) {
    switch (code) {
        case DxfCodes::DXF_ENTITY_SEPARATOR_CODE:
//            std::cout << "Separator code : " << value << std::endl;
            if (currentElementType != DxfElementType::DXF_UNKNOWN_TYPE) {
                switch (currentElementType) {
                    case DxfElementType::DXF_LAYER:
                        addLayer();
                        break;
                    case DxfElementType::DXF_LINE_TYPE:
                        addLineType();
                        break;
                    case DxfElementType::DXF_LINE:
                        addLine();
                        break;
                    case DxfElementType::DXF_BLOCK_RECORD:
                        addBlockRecord();
                        break;
                    case DxfElementType::DXF_BLOCK:
                        handleBlock();
                        break;
                    case DxfElementType::DXF_END_BLOCK:
                        addBlock();
                        break;
                    case DxfElementType::DXF_INSERT:
                        addBlockReference();
                        break;
                    case DxfElementType::DXF_LW_POLYLINE:
                        addLwPolyline();
                        break;
                    case DxfElementType::DXF_CIRCLE:
                        addCircle();
                        break;
                    case DxfElementType::DXF_ARC:
                        addCircularArc();
                        break;
                    case DxfElementType::DXF_ELLIPSE:
                        addEllipticalArc();
                        break;
                    case DxfElementType::DXF_POINT:
                        addPoint();
                        break;
                    case DxfElementType::DXF_POLYLINE:
                        handlePolyline();
                        break;
                    case DxfElementType::DXF_VERTEX:
                        handleVertex();
                        break;
                    case DxfElementType::DXF_SEQ_END:
                        handleVertexSequence();
                        break;
                    case DxfElementType::DXF_UNKNOWN_TYPE:
                        break;
                }
            }
            currentElementType = DxfCodes::getElementType(value);
            rawValues.clear();

            if (!variableName.empty()) {
                addVariable();
                variableName.clear();
                rawValues.clear();
            }
            break;
        case DxfCodes::DXF_VARIABLE_CODE:
            if (!variableName.empty()) addVariable();
            variableName = value;
            rawValues.clear();
            break;
        default:
            rawValues[code] = value;
            switch (currentElementType) {
                case DxfElementType::DXF_LW_POLYLINE:
                    handleLwPolyline(code);
                    break;
//                default:
//                    if (variableName.empty()) std::cout << "Error. Unknown group code! : " << code << " : " << value << std::endl;
            }
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

DxfEntityData DxfLoader::getEntityData() {
    return DxfEntityData(
            getStringValue(DxfCodes::DXF_HANDLE_CODE, ""),
            getStringValue(DxfCodes::DXF_LAYER_NAME_CODE, ""),
            getStringValue(DxfCodes::DXF_LINE_TYPE_NAME_CODE, DxfCodes::DXF_LINE_TYPE_NAME_DEFAULT),
            getIntValue(DxfCodes::DXF_COLOR_NUMBER_CODE, DxfCodes::DXF_COLOR_NUMBER_DEFAULT),
            getIntValue(DxfCodes::DXF_OBJECT_VISIBILITY_CODE, 0) == 0,
            getStringValue(DxfCodes::DXF_BLOCK_RECORD_HANDLE_CODE, ""),
            currentBlock
            );
}

DxfObjectData DxfLoader::getObjectData() {
    return DxfObjectData(
            getStringValue(DxfCodes::DXF_HANDLE_CODE, ""),
            getStringValue(DxfCodes::DXF_ELEMENT_NAME_CODE, ""),
            getStringValue(DxfCodes::DXF_OWNER_DICTIONARY_HANDLE_CODE, "")
            );
}

void DxfLoader::addVariable() {
    auto codes = DxfCodes::getVariableCodes(variableName);
    if (codes.empty()) {
//        std::cout << "Error. Unknown variable: " << variableName << std::endl;
        return;
    }
    std::vector<int> types;
    std::vector<std::string> values;
    for (int code : codes) {
        if (!hasRawValue(code)) {
//            std::cout << "Error. No value for code: " << code << " for variable: " << variableName << std::endl;
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
                dxfBuilder.setIntVariable(code, variableName, getIntValue(code, 0));
                break;
            case DxfCodes::DXF_DOUBLE:
                dxfBuilder.setDoubleVariable(code, variableName, getDoubleValue(code, 0));
                break;
            case DxfCodes::DXF_STRING:
                dxfBuilder.setStringVariable(code, variableName, getStringValue(code, ""));
                break;
        }
    } else if (codes.size() == 2) {
        dxfBuilder.set2DVectorVariable(codes[0], variableName, getDoubleValue(codes[0], 0), getDoubleValue(codes[1], 0));
    } else if (codes.size() == 3) {
        dxfBuilder.set3DVectorVariable(codes[0], variableName, getDoubleValue(codes[0], 0), getDoubleValue(codes[1], 0), getDoubleValue(codes[2], 0));
    }
}

void DxfLoader::addLayer() {
    auto data = new DxfLayerData(
            getObjectData(),
            getIntValue(DxfCodes::DXF_COLOR_NUMBER_CODE, 0),
            getIntValue(290, 1) != 0,
            getStringValue(DxfCodes::DXF_LINE_TYPE_NAME_CODE, "")
            );
    dxfBuilder.addLayer(new DxfLayerObject(*data));
}

void DxfLoader::addLineType() {
    auto data = new DxfLineTypeData(
            getObjectData(),
            getIntValue(73, 0),
            getDoubleValue(40, 0)
            );
    dxfBuilder.addLineType(new DxfLineTypeObject(*data));
}

void DxfLoader::addBlockRecord() {
    auto data = new DxfBlockRecordData(
            getObjectData(),
            getIntValue(DxfCodes::DXF_BLOCK_SCALABILITY_CODE, 0)
            );
    dxfBuilder.addBlockRecord(new DxfBlockRecordObject(*data));
}

void DxfLoader::addBlock() {
    dxfBuilder.addBlock(currentBlock);
    currentBlock = nullptr;
}

void DxfLoader::addLine() {
    auto data = new DxfLineData(
            getEntityData(),
            Vector3dd(getDoubleValue(10, 0), getDoubleValue(20, 0), getDoubleValue(30, 0)),
            Vector3dd(getDoubleValue(11, 0), getDoubleValue(21, 0),getDoubleValue (31, 0)),
            getDoubleValue(DxfCodes::DXF_THICKNESS_CODE, 1)
    );
    auto entity = new DxfLineEntity(*data);
    if (currentBlock != nullptr) currentBlock->entities.push_back(entity);
    dxfBuilder.addEntity(entity);
}

void DxfLoader::addLwPolyline() {
    auto data = new DxfLwPolylineData(
            getEntityData(),
            current2dVertices,
            getDoubleValue(DxfCodes::DXF_THICKNESS_CODE, 1),
            ((uint8_t) getIntValue(DxfCodes::DXF_FLAGS_CODE, 0) & 0b00000001u) == 1
            );
    auto entity = new DxfLwPolylineEntity(*data);
    if (currentBlock != nullptr) currentBlock->entities.push_back(entity);
    dxfBuilder.addEntity(entity);
    current2dVertices.clear();
}

void DxfLoader::addPolyline() {
    polylineData->vertices = current3dVertices;
    auto entity = new DxfPolylineEntity(*polylineData);
    if (currentBlock != nullptr) currentBlock->entities.push_back(entity);
    dxfBuilder.addEntity(entity);
}

void DxfLoader::addCircle() {
    auto data = new DxfCircleData(
            getEntityData(),
            Vector3dd(getDoubleValue(10, 0), getDoubleValue(20, 0), getDoubleValue(30, 0)),
            getDoubleValue(DxfCodes::DXF_RADIUS_CODE, 0),
            getDoubleValue(DxfCodes::DXF_THICKNESS_CODE, 1)
            );
    auto entity = new DxfCircleEntity(*data);
    if (currentBlock != nullptr) currentBlock->entities.push_back(entity);
    dxfBuilder.addEntity(entity);
}

void DxfLoader::addCircularArc() {
    auto data = new DxfCircularArcData(
            getEntityData(),
            Vector3dd(getDoubleValue(10, 0), getDoubleValue(20, 0), getDoubleValue(30, 0)),
            getDoubleValue(DxfCodes::DXF_RADIUS_CODE, 0),
            getDoubleValue(DxfCodes::DXF_THICKNESS_CODE, 1),
            getDoubleValue(DxfCodes::DXF_START_ANGLE_CODE, 0),
            getDoubleValue(DxfCodes::DXF_END_ANGLE_CODE, 0)
            );
    auto entity = new DxfCircularArcEntity(*data);
    if (currentBlock != nullptr) currentBlock->entities.push_back(entity);
    dxfBuilder.addEntity(entity);
}

void DxfLoader::addEllipticalArc() {
    auto data = new DxfEllipticalArcData(
            getEntityData(),
            Vector3dd(getDoubleValue(10, 0), getDoubleValue(20, 0), getDoubleValue(30, 0)),
            Vector3dd(getDoubleValue(11, 0), getDoubleValue(21, 0), getDoubleValue(31, 0)),
            getDoubleValue(40, 0),
            getDoubleValue(41, 0),
            getDoubleValue(42, 0)
            );
    auto entity = new DxfEllipticalArcEntity(*data);
    if (currentBlock != nullptr) currentBlock->entities.push_back(entity);
    dxfBuilder.addEntity(entity);
}

void DxfLoader::addPoint() {
    auto data = new DxfPointData(
            getEntityData(),
            Vector3dd(getDoubleValue(10, 0), getDoubleValue(20, 0), getDoubleValue(30, 0)),
            getDoubleValue(DxfCodes::DXF_THICKNESS_CODE, 1)
    );
    auto entity = new DxfPointEntity(*data);
    if (currentBlock != nullptr) currentBlock->entities.push_back(entity);
    dxfBuilder.addEntity(entity);
}

void DxfLoader::addBlockReference() {
    auto data = new DxfBlockReferenceData(
            getEntityData(),
            getStringValue(DxfCodes::DXF_ELEMENT_NAME_CODE, ""),
            Vector3dd(getDoubleValue(10, 0), getDoubleValue(20, 0), getDoubleValue(30, 0)),
            Vector3dd(getDoubleValue(41, 1), getDoubleValue(42, 1), getDoubleValue(43, 1)),
            getDoubleValue(DxfCodes::DXF_START_ANGLE_CODE, 0)
    );
    auto entity = new DxfBlockReferenceEntity(*data);
    if (currentBlock != nullptr) currentBlock->entities.push_back(entity);
    dxfBuilder.addEntity(entity);
}

void DxfLoader::handleBlock() {
    currentBlock = new DxfBlock(
            getStringValue(DxfCodes::DXF_HANDLE_CODE, ""),
            getStringValue(DxfCodes::DXF_ELEMENT_NAME_CODE, ""),
            getStringValue(DxfCodes::DXF_LAYER_NAME_CODE, ""),
            Vector3dd(getDoubleValue(10, 0), getDoubleValue(20, 0), getDoubleValue(30, 0)),
            getStringValue(DxfCodes::DXF_BLOCK_RECORD_HANDLE_CODE, ""),
            getIntValue(67, 0) == 0
    );
}

void DxfLoader::handleLwPolyline(int groupCode) {
    if (groupCode == 20) {
        current2dVertices.emplace_back(Vector2dd(getDoubleValue(10, 0), getDoubleValue(20, 0)));
    }
}

void DxfLoader::handlePolyline() {
    currentEntityType = DxfElementType::DXF_POLYLINE;
    polylineData = new DxfPolylineData(
            getEntityData(),
            getDoubleValue(DxfCodes::DXF_THICKNESS_CODE, 1),
            ((uint8_t) getIntValue(DxfCodes::DXF_FLAGS_CODE, 0) & 0b00000001u) == 1
            );
}

void DxfLoader::handleVertex() {
    auto data = new DxfVertexData(
            getEntityData(),
            Vector3dd(getDoubleValue(10, 0), getDoubleValue(20, 0), getDoubleValue(30, 0)),
            getDoubleValue(DxfCodes::DXF_BULGE_CODE, 0)
            );
    current3dVertices.emplace_back(data);
}

void DxfLoader::handleVertexSequence() {
    switch (currentEntityType) {
        case DxfElementType::DXF_POLYLINE:
            addPolyline();
            break;
        default:
            break;
    }
    currentEntityType = DxfElementType::DXF_UNKNOWN_TYPE;
    current3dVertices.clear();
}

// DXFToRGB24BufferLoader implementation
string DXFToRGB24BufferLoader::extension = ".dxf";

bool DXFToRGB24BufferLoader::acceptsFile(std::string const &name) {
    std::string lowercase = name;
    std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), ::tolower);
    return HelperUtils::endsWith(lowercase, extension);
}

RGB24Buffer* DXFToRGB24BufferLoader::load(const string &name) {
    DxfBuilder builder;
    DxfLoader loader(builder);
    int resultCode = loader.load(name);
    if (resultCode == -1) {
        std::cout << "Error. Can't open file: " << name << std::endl;
        return nullptr;
    } else {
        std::cout << "DXF file is loaded: " << name << " result code: " << resultCode << std::endl;
        auto drawing = builder.getDrawing();
        drawing.setScalingFactor(10.0);
        drawing.setPaddings(110, 110, 110, 110);
        return drawing.draw();
    }
}

} // namespace corecvs