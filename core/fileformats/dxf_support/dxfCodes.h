//
// Created by Myasnikov Vladislav on 10/17/19.
//

#ifndef DXF_SUPPORT_DXFCODES_H
#define DXF_SUPPORT_DXFCODES_H

#include <map>
#include <vector>
#include <string>
#include <list>
#include <iostream>

namespace corecvs {

enum class DxfElementType {
    DXF_LAYER, DXF_LINE_TYPE, DXF_LINE, DXF_LW_POLYLINE, DXF_UNKNOWN_TYPE
};

enum class DxfDrawingUnits {
    UNITLESS, INCHES, FEET, MILES, MILLIMETERS, CENTIMETERS, METERS, KILOMETERS, MICROINCHES, MILS, YARDS,
    ANGSTROMS, NANOMETERS, MICRONS, DECIMETERS, DECAMETERS, HECTOMETERS, GIGAMETERS, ASTRONOMICAL, LIGHT_YEARS, PARSECS
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
    static const int DXF_VERTEX_AMOUNT_CODE = 90;

    static const std::string DXF_LINE_TYPE_NAME_DEFAULT;
    static const int DXF_COLOR_NUMBER_DEFAULT = 256;

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

    static std::vector<uint8_t> getRGB(int number) {
        if (AUTOCAD_COLORS.count(number) == 1) {
            return AUTOCAD_COLORS[number];
        }
        return {0,0,0};
    }

    static DxfDrawingUnits getDrawingUnits(int value) {
        if (DRAWING_UNITS.count(value) == 1) {
            return DRAWING_UNITS[value];
        }
        return DxfDrawingUnits::UNITLESS;
    }

private:
    static std::map<std::string, std::vector<int>> VARIABLE_CODES;
    static std::map<std::string, DxfElementType> ELEMENT_TYPES;
    static std::map<int, std::vector<uint8_t>> AUTOCAD_COLORS;
    static std::map<int, DxfDrawingUnits> DRAWING_UNITS;
    static int CODE_RANGES[27][3];
};

} // namespace corecvs

#endif //DXF_SUPPORT_DXFCODES_H
