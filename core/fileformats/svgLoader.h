#pragma once
/**
 * \file svgLoader.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Dec 1, 2018
 * \author sergey skaredov
 */

#include <vector>
#include <unordered_map>
#include "core/utils/utils.h"
#include "core/tinyxml2/tinyxml2.h"
#include "core/geometry/polygons.h"
#include "core/geometry/ellipse.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/rgb24/wuRasterizer.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/fileformats/bufferLoader.h"

namespace corecvs {

using namespace tinyxml2;

class SvgShape;
class SvgLoader;

struct str_cmp
{
    bool operator()(const char *a, const char *b) const
    {
        return strcmp(a, b) < 0;
    }
};

typedef SvgShape* (SvgLoader::* f_type)(XMLElement*);
typedef std::map<const char*, f_type, str_cmp> shape_map;
typedef std::unordered_map<char, char> escape_map;
typedef std::map<const char*, RGBColor, str_cmp> color_map;

class SvgShape
{
public:

    static color_map color_m;

    const char *id;
    const char *stroke;
    const char *fill;
    int stroke_width;

    virtual void draw(RGB24Buffer */*buffer*/) {}
protected:
    RGBColor getColor()
    {
        if (stroke)
        {
            string s(stroke);
            if (HelperUtils::startsWith(s, "#"))
            {
                string hex = s.substr(1);
                if (hex.size() == 3)
                {
                    hex.insert(0, 1, hex[0]);
                    hex.insert(2, 1, hex[1]);
                    hex.insert(4, 1, hex[2]);
                }
                uint32_t color = stoul(hex, NULL, 16);
                return RGBColor(color);
            }
            auto iter = color_m.find(stroke);
            if (iter != color_m.end())
            {
                return iter->second;
            }
        }
        return RGBColor::White();
    }
};

class SvgRect: public SvgShape
{
public:
    double x, y;
    double w, h;
    // double rx, ry;

    virtual void draw(RGB24Buffer *buffer) override
    {
        Rectangled rect(x, y, w, h);
        buffer->drawRectangle(rect, getColor(), 0); // with stroke-width = 1
    }
};

class SvgCircle: public SvgShape
{
public:
    double cx;
    double cy;
    double r;

    virtual void draw(RGB24Buffer *buffer) override
    {
        Circle2d circle(cx, cy, r);
        buffer->drawArc(circle, getColor());
    }
};

class SvgEllipse: public SvgShape
{
public:
    double cx, cy;
    double rx, ry;

    virtual void draw(RGB24Buffer *buffer) override
    {
        AbstractPainter<RGB24Buffer> painter(buffer);
        painter.drawEllipse(cx, cy, rx, ry, 0, getColor());
    } 
};

class SvgLine: public SvgShape
{
public:
    double x1, y1;
    double x2, y2;

    virtual void draw(RGB24Buffer *buffer) override
    {
        buffer->drawLine(x1, y1, x2, y2, getColor());
    }
};

class SvgPolyLine: public SvgShape
{
public:
    PointPath points;

    virtual void draw(RGB24Buffer *buffer) override
    {
        AbstractPainter<RGB24Buffer> painter(buffer);
        painter.drawPath(points, getColor());
    }
};

class SvgPolygon: public SvgShape
{
public:
    Polygon polygon;

    virtual void draw(RGB24Buffer *buffer) override
    {
        AbstractPainter<RGB24Buffer> painter(buffer);
        painter.drawPolygon(polygon, getColor());
    }
};

struct Command
{
    char command;
    vector<double> params;

    Vector2dd getVector(int id = 0) const {
        return Vector2dd(params[id], params[id + 1]);
    }
};

class SvgPath: public SvgShape
{
public:
    vector<Command> commands;

    virtual void draw(RGB24Buffer *buffer) override;
private:
    Vector2dd start_point;
    Vector2dd cursor;
    Vector2dd dest;
    Vector2dd last_p;    /**< This stores last control point for Bézier curve */
    Vector2dd control_b; /**< This stores first  control point for Bézier curve */
    Vector2dd control_c; /**< This stores second control point for Bézier curve */
};

class SvgGroup : public SvgShape
{
public:
    vector<SvgShape*> shapes;
    virtual void draw(RGB24Buffer *buffer) override;
};

class SvgFile
{
public:
    double width;
    double height;
    vector<Vector2dd> viewBox;

    vector<SvgShape*> shapes;

    RGB24Buffer* draw();
private:
    Vector2dd start_point;
};

class SvgLoader
{
public:
    SvgLoader();
    virtual ~SvgLoader();

    bool trace = true;

    shape_map shape_m;
    static escape_map escape_m;

    int loadSvg(std::istream &input, SvgFile &svg);
private:
    int parseXML(XMLDocument &xml, SvgFile &svg);
    vector<Vector2dd> parsePoints(string data);
    void initShape(XMLElement *element, SvgShape *shape);
    SvgShape* getRect(XMLElement *element);
    SvgShape* getCircle(XMLElement *element);
    SvgShape* getEllipse(XMLElement *element);
    SvgShape* getLine(XMLElement *element);
    SvgShape* getPolyLine(XMLElement *element);
    SvgShape* getPolygon(XMLElement *element);
    SvgShape* getPath(XMLElement *element);
    SvgShape* getGroup(XMLElement *element);
};

class SVGToRGB24BufferLoader : public BufferLoader<RGB24Buffer>
{
public:
    static string extension;

    SVGToRGB24BufferLoader() {}

    virtual bool acceptsFile(const std::string &name) override;
    virtual RGB24Buffer *load(const std::string &name) override;

    virtual std::string              name()       override { return "SVGToRGB24BufferLoader"; }
    virtual std::vector<std::string> extentions() override { return {extension}; }
    virtual ~SVGToRGB24BufferLoader() {}
};


} // namespace corecvs
