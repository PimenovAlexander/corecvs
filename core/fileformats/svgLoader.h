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
#include "core/tinyxml/tinyxml2.h"
#include "core/geometry/polygons.h"
#include "core/geometry/ellipse.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/rgb24/wuRasterizer.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/buffers/rgb24/bezierRasterizer.h"
#include "bufferLoader.h"

namespace corecvs {

using namespace std;
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
typedef map<const char*, f_type, str_cmp> shape_map;
typedef unordered_map<char, char> escape_map;
typedef map<const char*, RGBColor, str_cmp> color_map;

class SvgShape
{
public:

    static color_map color_m;

    const char *id;
    const char *stroke;
    const char *fill;
    int stroke_width;

    virtual void draw(RGB24Buffer *buffer) {}
protected:
    RGBColor getColor()
    {
        if (stroke)
        {
            auto iter = color_m.find(stroke);
            if (iter != color_m.end())
            {
                return iter->second;
            }
        }
        return RGBColor::Black();
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
};

class SvgPath: public SvgShape
{
public:
    vector<Command> commands;

    virtual void draw(RGB24Buffer *buffer) override
    {
        if (commands.size() == 0)
        {
            return;
        }

        cursor = {0, 0};
        start_point = {0, 0};
        dest = {0, 0};
        RGBColor color = getColor();
        Curve curve;
        WuRasterizer rast = WuRasterizer();
        BezierRasterizer<RGB24Buffer, WuRasterizer> bezier(*buffer, rast, color);
        for (int i = 0; i < commands.size(); i++)
        {
            last_p[0] = 0;
            last_p[1] = 0;
            switch(commands[i].command)
            {
                case 'm':
                    last_p[0] = cursor[0];
                    last_p[1] = cursor[1];
                case 'M':
                    cursor[0] = commands[i].params[0] + last_p[0];
                    cursor[1] = commands[i].params[1] + last_p[1];
                    start_point = cursor;
                    break;
                case 'l':
                    last_p[0] = cursor[0];
                    last_p[1] = cursor[1];
                case 'L':
                    dest[0] = commands[i].params[0] + last_p[0];
                    dest[1] = commands[i].params[1] + last_p[1];
                    buffer->drawLine(cursor, dest, color);
                    cursor = dest;
                    break;
                case 'h':
                    last_p[0] = cursor[0];
                case 'H':
                    buffer->drawHLine(fround(cursor[0]), fround(cursor[1]), fround(commands[i].params[0] + last_p[0]), color);
                    cursor[0] = commands[i].params[0] + last_p[0];
                    break;
                case 'v':
                    last_p[1] = cursor[1];
                case 'V':
                    buffer->drawVLine(fround(cursor[0]), fround(cursor[1]), fround(commands[i].params[0] + last_p[1]), color);
                    cursor[1] = commands[i].params[0] + last_p[1];
                    break;
                case 'Z': case 'z':
                    buffer->drawLine(cursor, start_point, color);
                    cursor = start_point;
                    break;
                case 'c':
                    last_p[0] = cursor[0];
                    last_p[1] = cursor[1];
                case 'C':
                    control_c[0] = commands[i].params[2] + last_p[0];
                    control_c[1] = commands[i].params[3] + last_p[1];
                    dest[0] = commands[i].params[4] + last_p[0];
                    dest[1] = commands[i].params[5] + last_p[1];
                    curve = 
                    {
                        cursor,
                        {commands[i].params[0] + last_p[0], commands[i].params[1] + last_p[1]},
                        control_c,
                        dest
                    };
                    bezier.cubicBezierCasteljauApproximationByFlatness(curve);
                    cursor = dest;
                    break;
                case 's':
                    last_p[0] = cursor[0];
                    last_p[1] = cursor[1];
                case 'S':
                    dest[0] = commands[i].params[2] + last_p[0];
                    dest[1] = commands[i].params[3] + last_p[1];
                    if (i > 0)
                    {
                        if ((commands[i-1].command != 'C' && commands[i-1].command != 'c') &&
                            (commands[i-1].command != 'S' && commands[i-1].command != 's'))
                        {
                            control_c[0] = commands[i].params[0] + last_p[0];
                            control_c[1] = commands[i].params[1] + last_p[1];
                        }
                        else
                        {
                            control_c[0] = 2 * cursor[0] - control_c[0];
                            control_c[1] = 2 * cursor[1] - control_c[1];
                        }
                    }
                    curve =
                    {
                        cursor,
                        control_c,
                        {commands[i].params[0] + last_p[0], commands[i].params[1] + last_p[1]},
                        dest
                    };
                    control_c[0] = commands[i].params[0] + last_p[0];
                    control_c[1] = commands[i].params[1] + last_p[1];
                    bezier.cubicBezierCasteljauApproximationByFlatness(curve);
                    cursor = dest;
                    break;
                case 'q':
                case 'Q':
                case 't':
                case 'T':
                    break;
                case 'a':
                case 'A':
                    break;
            }
        }
    }
private:
    Vector2dd start_point;
    Vector2dd cursor;
    Vector2dd dest;
    Vector2dd last_p;
    Vector2dd control_c;
    Vector2dd control_q;
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

    int loadSvg(istream &input, SvgFile &svg);
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
