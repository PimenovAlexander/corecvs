//
// Created by Myasnikov Vladislav on 17.10.2019.
//

#ifndef DXF_SUPPORT_DXFLOADER_H
#define DXF_SUPPORT_DXFLOADER_H

#include "fileformats/dxf_support/dxfCodes.h"
#include "fileformats/dxf_support/dxfBuilder.h"
#include "buffers/rgb24/wuRasterizer.h"
#include "buffers/rgb24/rgb24Buffer.h"
#include "buffers/rgb24/abstractPainter.h"
#include "fileformats/bufferLoader.h"
#include "math/vector/vector2d.h"
#include "math/vector/vector3d.h"
#include <map>

namespace corecvs {

/**
 * DxfLoader is intended to load and represent DXF file into special structure for further drawing.
 *
 * Brief description of DxfLoader's work:
 * 1. File is loaded by sent string name.
 * 2. In a loop, all pairs (group code, value) are extracted.
 * 3. For each pair, depending on the group code, one of the following actions is performed:
 *    3.1. if it's a separation code, a new entity is created and transferred to the DxfBuilder or a sub-entity is created (e.g. a vertex in a polyline)
 *    3.2. a new system variable is set to the DxfBuilder
 *    3.3. the pair is stored in a 'values' map
 * 4. In the end, when all pairs were processed, DxfDrawing is created by DxfBuilder.
 *
 * DxfBuilder is helper-class for creating DxfDrawing.
 *
 * DxfDrawing is part representation of DXF file content.
 * It contains graphic entities that are associated with a certain layer and, optionally, are tied to a certain block.
 * Also this class contains some non-graphic objects (e.g. line types).
 * Several parameters can be set for the DxfDrawing for further display (e.g. paddings and scaling factor).
 * The class can return RGB24Buffer with visible entities.
 *
 * DxfEntity is abstract graphic DXF-entity.
 * To add a new entity, you need to inherit from DxfEntity and implement three methods:
 * 1. print          --- to print main information about the entity
 * 2. draw           --- to draw the entity
 * 3. getBoundingBox --- to calculate the bounding box
 * Also you need to inherit from DxfEntityData in order to add entity-specific properties.
 *
 * DxfObject is abstract non-graphic DXF-object.
 * To add a new object, you need to inherit from DxfObject and implement one method:
 * 1. print          --- to print main information about the object
 * Also you need to inherit from DxfObjectData in order to add object-specific properties.
 * 
 * DxfCodes contains DXF-specific codes and some helpful functions.
 */
class DxfLoader {
public:
    explicit DxfLoader(DxfBuilder &dxfBuilder)
    : dxfBuilder(dxfBuilder) {}

    int load(std::string const &fileName);

private:
    DxfBuilder &dxfBuilder;
    DxfElementType currentEntityType = DxfElementType::DXF_UNKNOWN_TYPE;
    DxfElementType currentElementType = DxfElementType::DXF_UNKNOWN_TYPE;
    std::string variableName;
    std::map<int, std::string> rawValues;
    std::vector<Vector2d<double>> current2dVertices = {};     // vertices of LwPolylineEntity
    std::vector<DxfVertexData*> current3dVertices = {};       // vertices of PolylineEntity
    DxfPolylineData *polylineData = nullptr;
    DxfBlock *currentBlock = nullptr;

    int processDxfPair(int code, std::string const &value);
    void addVariable();
    void addLayer();
    void addLineType();
    void addBlockRecord();
    void addLine();
    void addLwPolyline();
    void addPolyline();
    void addCircle();
    void addCircularArc();
    void addEllipticalArc();
    void addPoint();
    void addBlock();
    void addBlockReference();
    void handleLwPolyline(int groupCode);
    void handlePolyline();
    void handleVertex();
    void handleVertexSequence();
    void handleBlock();
    DxfEntityData getEntityData();
    DxfObjectData getObjectData();
    static bool getTruncatedLine(std::string &s, std::istream &stream);

    bool hasRawValue(int code) {
        return rawValues.count(code) == 1;
    }

    int getIntValue(int code, int defValue) {
        if (!hasRawValue(code)) {
            return defValue;
        }
        char* p;
        return (int) std::strtol(rawValues[code].c_str(), &p, 10);
    }

    double getDoubleValue(int code, double defValue) {
        if (!hasRawValue(code)) {
            return defValue;
        }
        char* p;
        return std::strtod(rawValues[code].c_str(), &p);
    }

    std::string getStringValue(int code, std::string const &defValue) {
        if (!hasRawValue(code)) {
            return defValue;
        }
        return rawValues[code];
    }
};

class DXFToRGB24BufferLoader : public BufferLoader<RGB24Buffer> {
public:
    DXFToRGB24BufferLoader() = default;
    ~DXFToRGB24BufferLoader() override = default;

    bool acceptsFile(std::string const &name) override;
    RGB24Buffer* load(std::string const &name) override;

    std::string name() override { return "DXFToRGB24BufferLoader"; }
    std::vector<std::string> extentions() override { return {extension}; }
    static string extension;
};

} // namespace corecvs

#endif //DXF_SUPPORT_DXFLOADER_H
