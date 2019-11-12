//
// Created by Myasnikov Vladislav on 10/17/19.
//

#ifndef DXF_SUPPORT_DXFCODES_H
#define DXF_SUPPORT_DXFCODES_H

#include <map>
#include <vector>
#include <string>
#include <list>

namespace corecvs {

enum class DxfElementType {
    DXF_LAYER, DXF_LINE_TYPE, DXF_LINE, DXF_UNKNOWN_TYPE
};

class DxfCodes {
public:
    static const int DXF_INT16 = 0;
    static const int DXF_INT32 = 1;
    static const int DXF_INT64 = 2;
    static const int DXF_DOUBLE = 3;
    static const int DXF_STRING = 4;
    static const int DXF_UNKNOWN_TYPE = 1000;

    static const int DXF_ENTITY_SEPARATOR_CODE = 0;
    static const int DXF_ELEMENT_NAME_CODE = 2;
    static const int DXF_HANDLE_CODE = 5;
    static const int DXF_LINE_TYPE_NAME_CODE = 6;
    static const int DXF_LAYER_NAME_CODE = 8;
    static const int DXF_VARIABLE_CODE = 9;
    static const int DXF_COLOR_NUMBER_CODE = 62;
    static const int DXF_FLAGS_CODE = 70;

    static std::vector<int> getVariableCodes(std::string const &name) {
        if (VARIABLE_CODES.count(name) == 1) {
            return VARIABLE_CODES[name];
        }
        return {};
    }

    static int getCodeType(int code) {
        for (int *range : CODE_RANGES) {
            if (code >= range[0] && code <= range[1]) {
                return range[2];
            }
        }
        return DXF_UNKNOWN_TYPE;
    }

    static DxfElementType getElementType(std::string const &name) {
        if (ELEMENT_TYPES.count(name) == 1) {
            return ELEMENT_TYPES[name];
        }
        return DxfElementType::DXF_UNKNOWN_TYPE;
    }

private:
    static std::map<std::string, std::vector<int>> VARIABLE_CODES;
    static std::map<std::string, DxfElementType> ELEMENT_TYPES;
    static int CODE_RANGES[27][3];
};

} // namespace corecvs

#endif //DXF_SUPPORT_DXFCODES_H
