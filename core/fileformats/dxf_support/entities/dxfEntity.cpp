//
// Created by Myasnikov Vladislav on 27.10.2019.
//
#include <iostream>

#include "core/fileformats/dxf_support/entities/dxfEntity.h"
#include "core/geometry/conic.h"
#include "core/utils/utils.h"
#include "core/buffers/rgb24/bezierRasterizer.h"
#include "core/buffers/rgb24/wuRasterizer.h"
#include "core/buffers/rgb24/abstractPainter.h"

namespace corecvs {

// Data printing
void DxfEntity::print() {
    std::cout << "Handle: " << data.handle << std::endl;
    std::cout << "Block record ID: " << (data.blockRecordID.empty() ? "none" : data.blockRecordID) << std::endl;
    std::cout << "Layer name: " << data.layerName << std::endl;
    std::cout << "Line type name: " << data.lineTypeName << std::endl;
    std::cout << "RGB color: " << (int) data.rgbColor.r() << " " << (int) data.rgbColor.g() << " " << (int) data.rgbColor.b() << " " << std::endl;
    std::cout << "Color number: " << data.colorNumber << std::endl;
    std::cout << "Visibility: " << (data.isVisible ? "on" : "off") << std::endl;
}

void DxfLineEntity::print() {
    std::cout << "* * * Line Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Start point: " << data.startPoint << std::endl;
    std::cout << "End point: " << data.endPoint << std::endl;
    std::cout << "Thickness: " << data.thickness << std::endl;
    std::cout << std::endl;
}

void DxfLwPolylineEntity::print() {
    std::cout << "* * * LwPolyline Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Thickness: " << data.thickness << std::endl;
    std::cout << "Vertex amount: " << data.vertices.size() << std::endl;
    int i = 1;
    for (Vector2d vertex : data.vertices) std::cout << "Vertex " << i++ << ": " << vertex << std::endl;
    std::cout << std::endl;
}

void DxfPolylineEntity::print() {
    std::cout << "* * * Polyline Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Thickness: " << data.thickness << std::endl;
    std::cout << "Vertex amount: " << data.vertices.size() << std::endl;
    int i = 1;
    for (DxfVertexData* vertex : data.vertices) {
        std::cout << "Vertex " << i++ << ": " << vertex->location << std::endl;
        std::cout << "+-->Bulge: " << vertex->bulge << std::endl;
    }
    std::cout << std::endl;
}

void DxfCircleEntity::print() {
    std::cout << "* * * Circle Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Center point: " << data.center << std::endl;
    std::cout << "Radius: " << data.radius << std::endl;
    std::cout << "Thickness: " << data.thickness << std::endl;
    std::cout << std::endl;
}

void DxfCircularArcEntity::print() {
    std::cout << "* * * Circular Arc Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Center point: " << data.center << std::endl;
    std::cout << "Radius: " << data.radius << std::endl;
    std::cout << "Thickness: " << data.thickness << std::endl;
    std::cout << "Angle range: " << data.startAngle << ".." << data.endAngle << std::endl;
    std::cout << std::endl;
}

void DxfEllipticalArcEntity::print() {
    std::cout << "* * * Elliptical Arc Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Center point: " << data.center << std::endl;
    std::cout << "End point of major axis: " << data.majorAxisEndPoint << std::endl;
    std::cout << "Ratio: " << data.ratio << std::endl;
    std::cout << "Angle range: " << data.startAngle << ".." << data.endAngle << std::endl;
    std::cout << std::endl;
}

void DxfPointEntity::print() {
    std::cout << "* * * Point Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Location: " << data.location << std::endl;
    std::cout << "Thickness: " << data.thickness << std::endl;
    std::cout << std::endl;
}

void DxfBlockReferenceEntity::print() {
    std::cout << "* * * Block Reference Entity * * *" << std::endl;
    DxfEntity::print();
    std::cout << "Block name: " << data.blockName << std::endl;
    std::cout << "Insertion point: " << data.insertionPoint << std::endl;
    std::cout << "Scale factor: " << data.scaleFactor << std::endl;
    std::cout << "Rotation angle: " << data.rotationAngle << std::endl;
    std::cout << std::endl;
}

// Drawing
void DxfLineEntity::draw(RGB24Buffer *buffer, DxfDrawing *drawing) {
    auto startPoint = drawing->getDrawingValues(data.startPoint.xy());
    auto endPoint   = drawing->getDrawingValues(data.endPoint.xy());
    auto thickness  = data.thickness;
    if (thickness > 1) {
        auto alpha = (endPoint - startPoint).argument();
        auto stepDelta    = Vector2dd(std::sin(alpha), std::cos(alpha));
        auto maxDelta     = stepDelta * thickness / 2;
        auto currentDelta = Vector2dd(0, 0);
        stepDelta *= 0.5;
        while (currentDelta.l2Metric() < maxDelta.l2Metric()) {
            buffer->drawLine(startPoint + currentDelta, endPoint + currentDelta, data.rgbColor);
            buffer->drawLine(startPoint - currentDelta, endPoint - currentDelta, data.rgbColor);
            currentDelta += stepDelta;
        }
    } else buffer->drawLine(startPoint, endPoint, data.rgbColor);
}

void DxfLwPolylineEntity::draw(RGB24Buffer *buffer, DxfDrawing *drawing) {
    int vertexNumber = data.vertices.size();
    if (vertexNumber > 1) {
        for (unsigned long i = 0; i < vertexNumber - !data.isClosed; i++) {
            auto startPoint = data.vertices[i       % vertexNumber];
            auto endPoint   = data.vertices[(i + 1) % vertexNumber];
            auto lineData   = DxfLineData(data, Vector3dd(startPoint, 0), Vector3dd(endPoint, 0), data.thickness);
            auto lineEntity = DxfLineEntity(lineData);
            lineEntity.draw(buffer, drawing);
        }
    } else if (vertexNumber == 1) {
        auto pointData   = DxfPointData(data, Vector3dd(data.vertices[0], 0), data.thickness);
        auto pointEntity = DxfPointEntity(pointData);
        pointEntity.draw(buffer, drawing);
    }
}

void DxfPolylineEntity::draw(RGB24Buffer *buffer, DxfDrawing *drawing) {
    int vertexNumber = data.vertices.size();
    if (vertexNumber > 1) {
        for (unsigned long i = 0; i < vertexNumber - !data.isClosed; i++) {
            Vector2dd startPoint = data.vertices[i     % vertexNumber]->location.xy();
            Vector2dd endPoint   = data.vertices[(i+1) % vertexNumber]->location.xy();
            auto bulge = data.vertices[i % vertexNumber]->bulge;

            if (bulge == 0) {
                auto lineData   = DxfLineData(data, Vector3dd(startPoint, 0), Vector3dd(endPoint, 0), data.thickness);
                auto lineEntity = DxfLineEntity(lineData);
                lineEntity.draw(buffer, drawing);
            } else {
                auto delta = startPoint - endPoint;
                auto chordLength = delta.l2Metric();
                auto radius = chordLength / 4 * (1 / std::abs(bulge) + std::abs(bulge));
                auto mediumPoint = (startPoint + endPoint) / 2;
                auto apothemLength = std::sqrt(radius * radius - (chordLength / 2) * (chordLength / 2));
                auto startEndPointAngle = delta.argument();
                auto centerPoint = mediumPoint + apothemLength * (
                        bulge < 0
                        ? Vector2dd(std::cos(startEndPointAngle + M_PI / 2), std::sin(startEndPointAngle + M_PI / 2))
                        : Vector2dd(std::cos(startEndPointAngle - M_PI / 2), std::sin(startEndPointAngle - M_PI / 2))
                        );

                auto startAngle = (startPoint - centerPoint).argument();
                auto endAngle   = (endPoint   - centerPoint).argument();

                auto circularArcData = DxfCircularArcData(data, Vector3dd(centerPoint.x(), centerPoint.y(), 0), radius, data.thickness, radToDeg(startAngle), radToDeg(endAngle));
                auto circularArc     = DxfCircularArcEntity(circularArcData);

                auto wasClockwiseDirection = drawing->isClockwiseDirection();
                drawing->setClockwiseDirection(bulge < 0);
                circularArc.draw(buffer, drawing);
                drawing->setClockwiseDirection(wasClockwiseDirection);
            }
        }
    } else if (vertexNumber == 1) {
        auto pointData   = DxfPointData(data, data.vertices[0]->location, data.thickness);
        auto pointEntity = DxfPointEntity(pointData);
        pointEntity.draw(buffer, drawing);
    }
}

void DxfCircleEntity::draw(RGB24Buffer *buffer, DxfDrawing *drawing) {
    auto centerPoint = drawing->getDrawingValues(data.center.xy());
    Circle2d circle(centerPoint, drawing->getDrawingValue(data.radius));
    buffer->drawArc(circle, data.rgbColor);
}

void DxfCircularArcEntity::draw(RGB24Buffer *buffer, DxfDrawing *drawing) {
    auto centerPoint = Vector2dd(data.center.xy());
    auto startAngle = degToRad(data.startAngle);
    auto endAngle   = degToRad(data.endAngle);

    // divide arc into 1 or 2 segments
    std::vector<Vector2dd> controlAngles;
    if (drawing->isClockwiseDirection()) {
        while (endAngle > startAngle) endAngle -= 2 * M_PI;
        controlAngles.emplace_back(Vector2dd(startAngle, std::max(endAngle, startAngle - M_PI + 0.00001)));
        if (startAngle - endAngle > M_PI) controlAngles.emplace_back(Vector2dd(startAngle - M_PI, endAngle));
    } else {
        while (endAngle < startAngle) endAngle += 2 * M_PI;
        controlAngles.emplace_back(Vector2dd(startAngle, std::min(endAngle, startAngle + M_PI - 0.00001)));
        if (endAngle - startAngle > M_PI) controlAngles.emplace_back(Vector2dd(startAngle + M_PI, endAngle));
    }

    // draw 1 or 2 segments
    for (Vector2dd alpha : controlAngles) {
        auto a = Vector2dd(std::cos(alpha.x()), std::sin(alpha.x())) * data.radius;
        auto b = Vector2dd(std::cos(alpha.y()), std::sin(alpha.y())) * data.radius;
        auto r = a.l2Metric();
        auto d = (a + b).l2Metric();
        double k = std::abs(b.y() - a.y()) > M_E
                ? (a.x() + b.x()) * (r / d - 0.5) * 8.0 / 3.0 / (b.y() - a.y())
                : (a.y() + b.y()) * (r / d - 0.5) * 8.0 / 3.0 / (a.x() - b.x());

        auto startPoint  = centerPoint + a;
        auto endPoint    = centerPoint + b;
        auto secondPoint = startPoint  + Vector2dd(-a.y(), a.x()) * k;
        auto thirdPoint  = endPoint    + Vector2dd(b.y(), -b.x()) * k;

        startPoint  = drawing->getDrawingValues(startPoint);
        secondPoint = drawing->getDrawingValues(secondPoint);
        thirdPoint  = drawing->getDrawingValues(thirdPoint);
        endPoint    = drawing->getDrawingValues(endPoint);

        WuRasterizer rast = WuRasterizer();
        BezierRasterizer<RGB24Buffer, WuRasterizer> bezier(*buffer, rast, data.rgbColor);
        bezier.cubicBezierCasteljauApproximationByFlatness(Curve({startPoint, secondPoint, thirdPoint, endPoint}));
    }
}

void DxfEllipticalArcEntity::draw(RGB24Buffer *buffer, DxfDrawing *drawing) {
    auto centerPoint = data.center.xy();
    auto delta = data.majorAxisEndPoint.xy();
    auto a = delta.l2Metric();
    auto b = a * data.ratio;

    double rotationAngle = 0;
    if (delta.x() == 0) rotationAngle = M_PI / 2;
    else if (delta.y() != 0) {
        auto tan = delta.y() / delta.x();
        rotationAngle = std::atan(tan);
    }
    double cosRotationAngle = std::cos(rotationAngle);
    double sinRotationAngle = std::sin(rotationAngle);
    auto startAngle = data.startAngle;
    auto endAngle   = data.endAngle;
    while (endAngle < startAngle) endAngle += 2 * M_PI;

    // divide arc into 1 or 2 segments
    std::vector<Vector2dd> controlAngles = {Vector2dd(startAngle, std::min(endAngle, startAngle + M_PI - 0.00001))};
    if (endAngle - startAngle > M_PI) controlAngles.emplace_back(Vector2dd(startAngle + M_PI, endAngle));

    // draw 1 or 2 segments
    for (Vector2dd alpha : controlAngles) {
        auto cosStartAlpha = std::cos(alpha.x());
        auto sinStartAlpha = std::sin(alpha.x());
        auto cosEndAlpha   = std::cos(alpha.y());
        auto sinEndAlpha   = std::sin(alpha.y());

        auto startDelta = Vector2dd(a * cosStartAlpha, b * sinStartAlpha);
        auto startPoint = centerPoint + Vector2dd(startDelta.x() * cosRotationAngle - startDelta.y() * sinRotationAngle, startDelta.x() * sinRotationAngle + startDelta.y() * cosRotationAngle);

        auto endDelta = Vector2dd(a * cosEndAlpha, b * sinEndAlpha);
        auto endPoint = centerPoint + Vector2dd(endDelta.x() * cosRotationAngle - endDelta.y() * sinRotationAngle, endDelta.x() * sinRotationAngle + endDelta.y() * cosRotationAngle);

        auto theta = std::sin(alpha.y() - alpha.x()) * (std::sqrt(4 + 3 * std::pow(std::tan((alpha.y() - alpha.x()) / 2), 2) - 1) / 3);
        auto secondPoint = startPoint + theta * Vector2dd(-a * cosRotationAngle * sinStartAlpha - b * sinRotationAngle * cosStartAlpha, -a * sinRotationAngle * sinStartAlpha + b * cosRotationAngle * cosStartAlpha);
        auto thirdPoint  = endPoint - theta * Vector2dd(-a * cosRotationAngle * sinEndAlpha - b * sinRotationAngle * cosEndAlpha, -a * sinRotationAngle * sinEndAlpha + b * cosRotationAngle * cosEndAlpha);

        startPoint  = drawing->getDrawingValues(startPoint);
        secondPoint = drawing->getDrawingValues(secondPoint);
        thirdPoint  = drawing->getDrawingValues(thirdPoint);
        endPoint    = drawing->getDrawingValues(endPoint);

        WuRasterizer rast = WuRasterizer();
        BezierRasterizer<RGB24Buffer, WuRasterizer> bezier(*buffer, rast, data.rgbColor);
        bezier.cubicBezierCasteljauApproximationByFlatness(Curve({startPoint, secondPoint, thirdPoint, endPoint}));
    }
}

void DxfPointEntity::draw(RGB24Buffer *buffer, DxfDrawing *drawing) {
    auto centerPoint = drawing->getDrawingValues(data.location.xy());
    auto thickness   = data.thickness;
    if (thickness > 1) {
        AbstractPainter<RGB24Buffer> painter(buffer);
        painter.drawCircle(centerPoint, thickness / 2, data.rgbColor);
    } else buffer->drawPixel(centerPoint, data.rgbColor);
}

void DxfBlockReferenceEntity::draw(RGB24Buffer *buffer , DxfDrawing *drawing) {}

// Bounding box getting
Rectangled DxfLineEntity::getBoundingBox() {
    auto lowerLeftCorner  = Vector2dd(std::min(data.startPoint.x(), data.endPoint.x()), std::min(data.startPoint.y(), data.endPoint.y()));
    auto upperRightCorner = Vector2dd(std::max(data.startPoint.x(), data.endPoint.x()), std::max(data.startPoint.y(), data.endPoint.y()));
    auto dimensions = upperRightCorner - lowerLeftCorner;
    return Rectangled(lowerLeftCorner, dimensions);
}

Rectangled DxfLwPolylineEntity::getBoundingBox() {
    if (!data.vertices.empty()) {
        auto lowerLeftCorner  = Vector2dd(data.vertices[0]);
        auto upperRightCorner = lowerLeftCorner;
        for (size_t i = 1; i < data.vertices.size(); i++) {
            auto x = data.vertices[i].x();
            auto y = data.vertices[i].y();
            if (x < lowerLeftCorner.x()) lowerLeftCorner.x() = x;
            else if (x > upperRightCorner.x()) upperRightCorner.x() = x;
            if (y < lowerLeftCorner.y()) lowerLeftCorner.y() = y;
            else if (y > upperRightCorner.y()) upperRightCorner.y() = y;
        }
        auto dimensions = upperRightCorner - lowerLeftCorner;
        return Rectangled(lowerLeftCorner, dimensions);
    }
    return Rectangled::Empty();
}

Rectangled DxfPolylineEntity::getBoundingBox() {
    if (!data.vertices.empty()) {
        auto lowerLeftCorner  = Vector2dd(data.vertices[0]->location.xy());
        auto upperRightCorner = lowerLeftCorner;
        for (size_t i = 1; i < data.vertices.size(); i++) {
            auto x = data.vertices[i]->location.x();
            auto y = data.vertices[i]->location.y();
            if (x < lowerLeftCorner.x()) lowerLeftCorner.x() = x;
            else if (x > upperRightCorner.x()) upperRightCorner.x() = x;
            if (y < lowerLeftCorner.y()) lowerLeftCorner.y() = y;
            else if (y > upperRightCorner.y()) upperRightCorner.y() = y;
        }
        auto dimensions = upperRightCorner - lowerLeftCorner;
        return Rectangled(lowerLeftCorner, dimensions);
    }
    return Rectangled::Empty();
}

Rectangled DxfCircleEntity::getBoundingBox() {
    auto lowerLeftCorner  = Vector2dd(data.center.x() - data.radius, data.center.y() - data.radius);
    auto upperRightCorner = Vector2dd(data.center.x() + data.radius, data.center.y() + data.radius);
    auto dimensions = upperRightCorner - lowerLeftCorner;
    return Rectangled(lowerLeftCorner, dimensions);
}

Rectangled DxfCircularArcEntity::getBoundingBox() {
    auto lowerLeftCorner  = Vector2dd(data.center.x() - data.radius, data.center.y() - data.radius);
    auto upperRightCorner = Vector2dd(data.center.x() + data.radius, data.center.y() + data.radius);
    auto dimensions = upperRightCorner - lowerLeftCorner;
    return Rectangled(lowerLeftCorner, dimensions);
}

Rectangled DxfEllipticalArcEntity::getBoundingBox() {
    auto majorRadius = std::sqrt(data.majorAxisEndPoint.x() * data.majorAxisEndPoint.x() + data.majorAxisEndPoint.y() * data.majorAxisEndPoint.y());
    auto lowerLeftCorner  = Vector2dd(data.center.x() - majorRadius, data.center.y() - majorRadius);
    auto upperRightCorner = Vector2dd(data.center.x() + majorRadius, data.center.y() + majorRadius);
    auto dimensions = upperRightCorner - lowerLeftCorner;
    return Rectangled(lowerLeftCorner, dimensions);
}

Rectangled DxfPointEntity::getBoundingBox() {
    auto lowerLeftCorner  = Vector2dd(data.location.xy());
    auto upperRightCorner = lowerLeftCorner;
    auto dimensions = upperRightCorner - lowerLeftCorner;
    return Rectangled(lowerLeftCorner, dimensions);
}

Rectangled DxfBlockReferenceEntity::getBoundingBox() {
    return Rectangled::Empty();
}

} // namespace corecvs
