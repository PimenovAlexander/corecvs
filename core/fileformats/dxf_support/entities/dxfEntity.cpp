//
// Created by Myasnikov Vladislav on 10/27/19.
//

#include "core/fileformats/dxf_support/entities/dxfEntity.h"
#include "core/geometry/conic.h"
#include <iostream>

namespace corecvs {

// Data printing
void DxfEntity::print() const {
    std::cout << "Handle: " << data->handle << std::endl;
    std::cout << "Flags: " << data->flags << std::endl;
    std::cout << "Layer name: " << data->layerName << std::endl;
    std::cout << "Line type name: " << data->lineTypeName << std::endl;
    std::cout << "RGB color: " << (int) data->rgbColor.r() << " " << (int) data->rgbColor.g() << " " << (int) data->rgbColor.b() << " " << std::endl;
    std::cout << "Color number: " << data->colorNumber << std::endl;
}

void DxfLineEntity::print() const {
    std::cout << "* * * Line Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Start point: " << data->x1 << ", " << data->y1 << ", " << data->z1 << std::endl;
    std::cout << "End point: " << data->x2 << ", " << data->y2 << ", " << data->z2 << std::endl;
    std::cout << std::endl;
}

void DxfLwPolylineEntity::print() const {
    std::cout << "* * * LwPolyline Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Vertex amount: " << data->vertexNumber << std::endl;
    int i = 1;
    for (Vector2d vertex : data->vertices) {
        std::cout << "Vertex " << i++ << ": " << vertex.x() << " " << vertex.y() << std::endl;
    }
    std::cout << std::endl;
}

void DxfPolylineEntity::print() const {
    std::cout << "* * * Polyline Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Vertex amount: " << data->vertices.size() << std::endl;
    int i = 1;
    for (Vector3d vertex : data->vertices) {
        std::cout << "Vertex " << i++ << ": " << vertex.x() << " " << vertex.y() << " " << vertex.z() << std::endl;
    }
    std::cout << std::endl;
}

void DxfCircleEntity::print() const {
    std::cout << "* * * Circle Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Center point: " << data->center << std::endl;
    std::cout << "Radius: " << data->radius << std::endl;
    std::cout << "Thickness: " << data->thickness << std::endl;
    std::cout << std::endl;
}

void DxfArcEntity::print() const {
    std::cout << "* * * Arc Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Center point: " << data->center << std::endl;
    std::cout << "Radius: " << data->radius << std::endl;
    std::cout << "Thickness: " << data->thickness << std::endl;
    std::cout << "Angle range: " << data->startAngle << ".." << data->endAngle << std::endl;
    std::cout << std::endl;
}

// Drawing
void DxfLineEntity::draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) {
    auto startVertex = attrs->getDrawingValues(data->x1, data->y1);
    auto endVertex = attrs->getDrawingValues(data->x2, data->y2);
    buffer->drawLine(startVertex.x(), startVertex.y(), endVertex.x(), endVertex.y(), data->rgbColor);
}

void DxfLwPolylineEntity::draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) {
    if (data->vertexNumber > 1) {
        for (unsigned long i = 0; i < data->vertices.size() - 1; i++) {
            auto startVertex = attrs->getDrawingValues(data->vertices[i].x(), data->vertices[i].y());
            auto endVertex = attrs->getDrawingValues(data->vertices[i+1].x(), data->vertices[i+1].y());
            buffer->drawLine(startVertex.x(), startVertex.y(), endVertex.x(), endVertex.y(), data->rgbColor);
        }
        // closed polyline
        if (data->flags == 1) {
            auto startVertex = attrs->getDrawingValues(data->vertices[0].x(), data->vertices[0].y());
            auto endVertex = attrs->getDrawingValues(data->vertices[data->vertexNumber-1].x(), data->vertices[data->vertexNumber-1].y());
            buffer->drawLine(startVertex.x(), startVertex.y(), endVertex.x(), endVertex.y(), data->rgbColor);
        }
    }
}

void DxfPolylineEntity::draw(RGB24Buffer *buffer, DxfDrawingAttrs *attrs) {
    int vertexNumber = data->vertices.size();
    if (vertexNumber > 1) {
        for (unsigned long i = 0; i < vertexNumber - 1; i++) {
            auto startVertex = attrs->getDrawingValues(data->vertices[i].x(), data->vertices[i].y());
            auto endVertex = attrs->getDrawingValues(data->vertices[i+1].x(), data->vertices[i+1].y());
            buffer->drawLine(startVertex.x(), startVertex.y(), endVertex.x(), endVertex.y(), data->rgbColor);
        }
        // closed polyline
        if (data->flags == 1) {
            auto startVertex = attrs->getDrawingValues(data->vertices[0].x(), data->vertices[0].y());
            auto endVertex = attrs->getDrawingValues(data->vertices[vertexNumber-1].x(), data->vertices[vertexNumber-1].y());
            buffer->drawLine(startVertex.x(), startVertex.y(), endVertex.x(), endVertex.y(), data->rgbColor);
        }
    }
}

void DxfCircleEntity::draw(class corecvs::RGB24Buffer * buffer, class corecvs::DxfDrawingAttrs * attrs) {
    auto vertex = attrs->getDrawingValues(data->center.x(), data->center.y());
    Circle2d circle(vertex, attrs->getDrawingValue(data->radius));
    buffer->drawArc(circle, data->rgbColor);
}

void DxfArcEntity::draw(class corecvs::RGB24Buffer * buffer, class corecvs::DxfDrawingAttrs * attrs) {
    //TODO: draw arc
}

} // namespace corecvs
