//
// Created by Myasnikov Vladislav on 10/21/19.
//

#ifndef DXF_SUPPORT_IMPLDXFBUILDER_H
#define DXF_SUPPORT_IMPLDXFBUILDER_H

#include "core/fileformats/dxf_support/iDxfBuilder.h"
#include "core/fileformats/dxf_support/dxfCodes.h"
#include "core/math/vector/vector3d.h"
#include <string>
#include <list>
#include <map>

namespace corecvs {

class ImplDxfBuilder : public IDxfBuilder {
public:
    ImplDxfBuilder() = default;

    // Variables
    void setIntVariable(int code, std::string const &name, int value) override;
    void setDoubleVariable(int code, std::string const &name, double value) override;
    void setStringVariable(int code, std::string const &name, std::string const &value) override;
    void set2DVectorVariable(int code, std::string const &name,  double x, double y) override;
    void set3DVectorVariable(int code, std::string const &name,  double x, double y, double z) override;

    // Objects
    void addLayer(DxfLayerObject *object) override;
    void addLineType(DxfLineTypeObject *object) override;

    // Entities
    void addLine(DxfLineEntity *entity) override;
    void addLwPolyline(DxfLwPolylineEntity *entity) override;

    RGB24Buffer* draw() override;

private:
    DxfDrawingAttrs attrs;
    Vector3dd leftTopCorner;
    Vector3dd rightBottomCorner;
    std::list<DxfEntity*> entities = {};
    std::list<DxfObject*> objects = {};
    std::map<std::string,DxfLayerObject*> layers = {};

    void prepareToDraw();
};

} // namespace corecvs

#endif //DXF_SUPPORT_IMPLDXFBUILDER_H
