//
// Created by Myasnikov Vladislav on 10/27/19.
//

#include "core/fileformats/dxf_support/entities/dxfEntity.h"
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

// Drawing
void DxfLineEntity::draw(RGB24Buffer *buffer, DxfDrawingUnits units, int height, int marginLeft, int marginTop) {
    auto x1 = DxfCodes::getDrawingValue(data->x1, units);
    auto y1 = DxfCodes::getDrawingValue(data->y1, units);
    auto x2 = DxfCodes::getDrawingValue(data->x2, units);
    auto y2 = DxfCodes::getDrawingValue(data->y2, units);
    buffer->drawLine(x1 + marginLeft, y1 + marginTop, x2 + marginLeft, y2 + marginTop, data->rgbColor);
}

void DxfLwPolylineEntity::draw(RGB24Buffer *buffer, DxfDrawingUnits units, int height, int marginLeft, int marginTop) {
    if (data->vertexNumber > 1) {
        for (unsigned long i = 0; i < data->vertices.size() - 1; i++) {
            std::vector<double> curVertex = {data->vertices[i].x(),data->vertices[i].y()};
            std::vector<double> nextVertex = {data->vertices[i+1].x(),data->vertices[i+1].y()};
            auto curPosition = DxfCodes::getDrawingValues(curVertex, units);
            auto nextPosition = DxfCodes::getDrawingValues(nextVertex, units);
            buffer->drawLine(curPosition->at(0) + marginLeft, curPosition->at(1) + marginTop, nextPosition->at(0) + marginLeft, nextPosition->at(1) + marginTop, data->rgbColor);
            delete curPosition;
            delete nextPosition;
        }
        // closed polyline
        if (data->flags == 1) {
            std::vector<double> curVertex = {data->vertices[0].x(),data->vertices[0].y()};
            std::vector<double> nextVertex = {data->vertices[data->vertexNumber-1].x(),data->vertices[data->vertexNumber-1].y()};
            auto curPosition = DxfCodes::getDrawingValues(curVertex, units);
            auto nextPosition = DxfCodes::getDrawingValues(nextVertex, units);
            buffer->drawLine(curPosition->at(0) + marginLeft, curPosition->at(1) + marginTop, nextPosition->at(0) + marginLeft, nextPosition->at(1) + marginTop, data->rgbColor);
            delete curPosition;
            delete nextPosition;
        }
    }
}

} // namespace corecvs
