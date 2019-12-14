//
// Created by Myasnikov Vladislav on 10/27/19.
//

#include "core/fileformats/dxf_support/entities/dxfEntity.h"
#include "core/geometry/conic.h"
#include "core/buffers/rgb24/bezierRasterizer.h"
#include "core/utils/utils.h"
#include <iostream>
#include <core/buffers/rgb24/wuRasterizer.h>
#include <core/buffers/rgb24/abstractPainter.h>

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

void DxfCircularArcEntity::print() const {
    std::cout << "* * * Arc Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Center point: " << data->center << std::endl;
    std::cout << "Radius: " << data->radius << std::endl;
    std::cout << "Thickness: " << data->thickness << std::endl;
    std::cout << "Angle range: " << data->startAngle << ".." << data->endAngle << std::endl;
    std::cout << std::endl;
}

void DxfEllipticalArcEntity::print() const {
    std::cout << "* * * Ellipse Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Center point: " << data->center << std::endl;
    std::cout << "End point of major axis: " << data->majorAxisEndPoint << std::endl;
    std::cout << "Ratio: " << data->ratio << std::endl;
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

void DxfCircleEntity::draw(class corecvs::RGB24Buffer *buffer, class corecvs::DxfDrawingAttrs *attrs) {
    auto vertex = attrs->getDrawingValues(data->center.x(), data->center.y());
    Circle2d circle(vertex, attrs->getDrawingValue(data->radius));
    buffer->drawArc(circle, data->rgbColor);
}

void DxfCircularArcEntity::draw(class corecvs::RGB24Buffer *buffer, class corecvs::DxfDrawingAttrs *attrs) {
    auto startAngle = degToRad(std::min(data->startAngle, data->endAngle));
    auto endAngle = degToRad(std::max(data->startAngle, data->endAngle));

    auto point1 = Vector2dd(data->center.x() + data->radius * std::cos(startAngle), data->center.y() + data->radius * std::sin(startAngle));
    auto point4 = Vector2dd(data->center.x() + data->radius * std::cos(endAngle), data->center.y() + data->radius * std::sin(endAngle));

    auto ax = point1.x() - data->center.x();
    auto ay = point1.y() - data->center.y();
    auto bx = point4.x() - data->center.x();
    auto by = point4.y() - data->center.y();
    auto r = std::sqrt(ax * ax + ay * ay);
    auto d = std::sqrt((ax + bx) * (ax + bx) + (ay + by) * (ay + by));
    double k;
    if (std::abs(by - ay) > M_E) {
        k = (ax + bx) * (r / d - 0.5) * 8.0 / 3.0 / (by - ay);
    } else {
        k = (ay + by) * (r / d - 0.5) * 8.0 / 3.0 / (ax - bx);
    }

    // TODO: add splitting into 1 or 2 segments

    // intermediate points
    auto point2 = Vector2dd(point1.x() - k * ay, point1.y() + k * ax);
    auto point3 = Vector2dd(point4.x() + k * by, point4.y() - k * bx);

    point1 = attrs->getDrawingValues(point1);
    point2 = attrs->getDrawingValues(point2);
    point3 = attrs->getDrawingValues(point3);
    point4 = attrs->getDrawingValues(point4);

    auto curve = Curve({point1, point2, point3, point4});
    WuRasterizer rast = WuRasterizer();
    BezierRasterizer<RGB24Buffer, WuRasterizer> bezier(*buffer, rast, data->rgbColor);
    bezier.cubicBezierCasteljauApproximationByFlatness(curve);
}

void DxfEllipticalArcEntity::draw(class corecvs::RGB24Buffer *buffer, class corecvs::DxfDrawingAttrs *attrs) {
    AbstractPainter<RGB24Buffer> painter(buffer);
    auto center = Vector2dd(data->center.x(), data->center.y());
    auto deltaX = data->majorAxisEndPoint.x();
    auto deltaY = data->majorAxisEndPoint.y();
    auto a = std::sqrt(deltaX * deltaX + deltaY * deltaY);
    auto b = a * data->ratio;

    double angle = 0;
    if (deltaX == 0) angle = M_PI / 2;
    else if (deltaY != 0) {
        auto tan = deltaY / deltaX;
        angle = std::atan(tan);
    }
    double cosAngle = std::cos(angle);
    double sinAngle = std::sin(angle);

    // compute focuses
    double c  = std::sqrt(a * a - b * b);
    double dx = c * cosAngle;
    double dy = c * sinAngle;
    auto focus1 = Vector2dd(center.x() - dx, center.y() - dy);
    auto focus2 = Vector2dd(center.x() + dx, center.y() + dy);

    // divide arc into 1 or 2 segments
    auto startAngle = data->startAngle;
    auto endAngle = data->endAngle;
    auto doublePi = 2 * M_PI;
    while (endAngle < startAngle) endAngle += doublePi;
    std::vector<Vector2dd> controlAngles = {Vector2dd(startAngle, std::min(endAngle, startAngle + M_PI - 0.00001))};
    if (endAngle - startAngle > M_PI) controlAngles.emplace_back(Vector2dd(startAngle + M_PI, endAngle));

    for (Vector2dd eta : controlAngles) {
        auto eta1 = eta.x();
        auto eta2 = eta.y();

        // start point
        auto cosEta1 = std::cos(eta1);
        auto sinEta1 = std::sin(eta1);
        dx = a * cosEta1;
        dy = b * sinEta1;
        auto point1 = Vector2dd(center.x() + dx * cosAngle - dy * sinAngle, center.y() + dx * sinAngle + dy * cosAngle);

        // end point
        auto cosEta2 = std::cos(eta2);
        auto sinEta2 = std::sin(eta2);
        dx = a * cosEta2;
        dy = b * sinEta2;
        auto point4 = Vector2dd(center.x() + dx * cosAngle - dy * sinAngle, center.y() + dx * sinAngle + dy * cosAngle);

        // intermediate points
        auto alpha = std::sin(eta2 - eta1) * (std::sqrt(4 + 3 * std::pow(std::tan((eta2 - eta1) / 2), 2) - 1) / 3);
        auto point2 = point1 + alpha * Vector2dd(-a * cosAngle * sinEta1 - b * sinAngle * cosEta1,
                                                 -a * sinAngle * sinEta1 + b * cosAngle * cosEta1);
        auto point3 = point4 - alpha * Vector2dd(-a * cosAngle * sinEta2 - b * sinAngle * cosEta2,
                                                 -a * sinAngle * sinEta2 + b * cosAngle * cosEta2);

        point1 = attrs->getDrawingValues(point1);
        point2 = attrs->getDrawingValues(point2);
        point3 = attrs->getDrawingValues(point3);
        point4 = attrs->getDrawingValues(point4);

        auto curve = Curve({point1, point2, point3, point4});
        WuRasterizer rast = WuRasterizer();
        BezierRasterizer<RGB24Buffer, WuRasterizer> bezier(*buffer, rast, data->rgbColor);
        bezier.cubicBezierCasteljauApproximationByFlatness(curve);
    }
}

} // namespace corecvs
