/**
 * \file svgLoader.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Dec 1, 2018
 * \author sergey skaredov
 */

#include <fstream>

#include "core/buffers/rgb24/bresenhamRasterizer.h"
#include "core/buffers/rgb24/bezierRasterizer.h"

#include "core/utils/utils.h"
#include "core/tinyxml2/tinyxml2.h"
#include "core/fileformats/svgLoader.h"

namespace corecvs {

using namespace std;
using namespace tinyxml2;

escape_map SvgLoader::escape_m =
{
    { ',', ' '},
    {'\f', ' '},
    {'\n', ' '},
    {'\r', ' '},
    {'\t', ' '},
    {'\v', ' '},
    {'\0', ' '},
    { '-', '-'}
};

color_map SvgShape::color_m =
{
    { "black", RGBColor::Black() },
    { "white", RGBColor::White() },
    {  "gray", RGBColor::Gray()  },
    {   "red", RGBColor::Red()   },
    { "green", RGBColor::Green() },
    {  "blue", RGBColor::Blue()  },
    {  "navy", RGBColor::Navy()  },
    {"purple", RGBColor::Purple()},
    {"yellow", RGBColor::Yellow()},
    {"orange", RGBColor::Orange()}
};

SvgLoader::SvgLoader()
{
    shape_m.emplace("rect", &SvgLoader::getRect);
    shape_m.emplace("circle", &SvgLoader::getCircle);
    shape_m.emplace("ellipse", &SvgLoader::getEllipse);
    shape_m.emplace("line", &SvgLoader::getLine);
    shape_m.emplace("polyline", &SvgLoader::getPolyLine);
    shape_m.emplace("polygon", &SvgLoader::getPolygon);
    shape_m.emplace("path", &SvgLoader::getPath);

    shape_m.emplace("g", &SvgLoader::getGroup);

}

SvgLoader::~SvgLoader()
{}

int SvgLoader::loadSvg(istream &input, SvgFile &svg)
{
    input.seekg(0, input.end);
    int len = input.tellg();
    input.seekg(0, input.beg);

    char data[len];
    input.read(data, len);

    XMLDocument xml;
    int errorID = xml.Parse(data, len);

    if (errorID)
    {
        cout << "SvgLoader::loadSvg: fail loading data" << endl;
        return errorID;
    }

    parseXML(xml, svg);    
    return 0;
}

int SvgLoader::parseXML(XMLDocument &xml, SvgFile &svg)
{
    XMLElement *root = xml.FirstChildElement("svg");
    if (!root)
    {
        cout << "SvgLoader::parseXML: couldn't find root 'svg'\n" << endl;
        return 1;
    }

    svg.width = root->DoubleAttribute("width");
    svg.height = root->DoubleAttribute("height");

    const char *_viewBox = root->Attribute("viewBox");
    
    if (_viewBox) {
        vector<Vector2dd> viewBox = parsePoints(string(_viewBox));

        if (viewBox.size() == 2)
        {
            svg.viewBox = viewBox;
        }
    }
    
    XMLElement *current = root->FirstChildElement();
    while(current)
    {
        const char *name = current->Name();
        if (trace) {
            cout << "Processing <" << name << ">" << endl;
        }

        auto iter = shape_m.find(name);
        if (iter != shape_m.end())
        {
            f_type getShape = iter->second;
            SvgShape *shape = (this->*getShape)(current);
            svg.shapes.push_back(shape);
        }
        
        current = current->NextSiblingElement();
    }
    return 0;
}

vector<Vector2dd> SvgLoader::parsePoints(string data)
{
    if (trace) {
        SYNC_PRINT(("SvgLoader::parsePoints(%s): called", data.c_str()));
    }
    data = HelperUtils::escapeString(data, escape_m, " ");
    vector<string> coords = HelperUtils::stringSplit(data, ' ');
    vector<Vector2dd> points;
    for(size_t i = 0; i < coords.size(); i += 2) {
        double x = HelperUtils::parseDouble(coords[i]);
        double y = HelperUtils::parseDouble(coords[i + 1]);
        points.push_back(Vector2dd(x, y));
    }
    return points;
}

vector<Vector2dd> SvgLoader::parsePointsPairs(string data)
{
    if (trace) {
        SYNC_PRINT(("SvgLoader::parsePointsPairs(%s): called\n", data.c_str()));
    }
    data = HelperUtils::escapeString(data, escape_m, " ");
    vector<string> coords = HelperUtils::stringSplit(data, " ,");

    vector<Vector2dd> points;
    for(size_t i = 0; i < coords.size(); i += 2) {
        double x = HelperUtils::parseDouble(coords[i]);
        double y = HelperUtils::parseDouble(coords[i + 1]);
        points.push_back(Vector2dd(x, y));
    }
    return points;
}

void SvgLoader::initShape(XMLElement *element, SvgShape *shape)
{
    shape->id = element->Attribute("id");
    shape->stroke = element->Attribute("stroke");
    shape->fill = element->Attribute("fill");
    shape->stroke_width = element->IntAttribute("stroke-width");
}

SvgShape* SvgLoader::getRect(XMLElement *element)
{
    SvgRect *rect = new SvgRect();
    initShape(element, rect);
    rect->x = element->DoubleAttribute("x");
    rect->y = element->DoubleAttribute("y");
    rect->w = element->DoubleAttribute("width");
    rect->h = element->DoubleAttribute("height");
    
    if (trace)
    {
        cout << "getRect: x=" << rect->x << ", ";
        cout << "y=" << rect->y << ", ";
        cout << "w=" << rect->w << ", ";
        cout << "h=" << rect->h << endl;
    }
    return rect;
}

SvgShape* SvgLoader::getCircle(XMLElement *element)
{
    SvgCircle *circle = new SvgCircle();
    initShape(element, circle);
    circle->cx = element->DoubleAttribute("cx");
    circle->cy = element->DoubleAttribute("cy");
    circle->r = element->DoubleAttribute("r");
    
    if (trace)
    {
        cout << "getCircle: cx=" << circle->cx << ", ";
        cout << "cy=" << circle->cy << ", ";
        cout << "r=" << circle->r << endl;
    }
    return circle;
}

SvgShape* SvgLoader::getEllipse(XMLElement *element)
{
    SvgEllipse *ellipse = new SvgEllipse();
    initShape(element, ellipse);
    ellipse->cx = element->DoubleAttribute("cx");
    ellipse->cy = element->DoubleAttribute("cy");
    ellipse->rx = element->DoubleAttribute("rx");
    ellipse->ry = element->DoubleAttribute("ry");
    
    if (trace)
    {
        cout << "getEllipse: cx=" << ellipse->cx << ", ";
        cout << "cy=" << ellipse->cy << ", ";
        cout << "rx=" << ellipse->rx << ", ";
        cout << "ry=" << ellipse->ry << endl;
    }
    return ellipse;
}

SvgShape* SvgLoader::getLine(XMLElement *element)
{
    SvgLine *line = new SvgLine();
    initShape(element, line);
    line->x1 = element->DoubleAttribute("x1");
    line->y1 = element->DoubleAttribute("y1");
    line->x2 = element->DoubleAttribute("x2");
    line->y2 = element->DoubleAttribute("y2");
        
    if (trace)
    {
        cout << "getLine: x1=" << line->x1 << ", ";
        cout << "y1=" << line->y1 << ", ";
        cout << "x2=" << line->x2 << ", ";
        cout << "y2=" << line->y2 << endl;
    }
    return line;
}

SvgShape* SvgLoader::getPolyLine(XMLElement *element)
{
    SvgPolyLine *polyline = new SvgPolyLine();
    initShape(element, polyline);

    string attr(element->Attribute("points"));
    vector<Vector2dd> points = parsePointsPairs(attr);
    polyline->points.insert(polyline->points.end(), points.begin(), points.end());

    if (trace)
    {
        cout << "getPolyLine: points=";
        for (Vector2dd v: points)
        {
            cout << v << " ";
        }
        cout << endl;
    }
    return polyline;
}

SvgShape* SvgLoader::getPolygon(XMLElement *element)
{
    SvgPolygon *polygon = new SvgPolygon();
    initShape(element, polygon);
    
    string attr(element->Attribute("points"));
    vector<Vector2dd> points = parsePointsPairs(attr);
    polygon->polygon.insert(polygon->polygon.end(), points.begin(), points.end());

    if (trace)
    {
        cout << "getPolygon: points=";
        for (Vector2dd v: points)
        {
            cout << v << " ";
        }
        cout << endl;
    }
    return polygon;
}

SvgShape* SvgLoader::getPath(XMLElement *element)
{
    SvgPath *path = new SvgPath;
    initShape(element, path);
    
    string d(element->Attribute("d"));
    d = HelperUtils::escapeString(d, escape_m, " ");
    
    size_t index = 0;
    while (index < d.size())
    {
        Command cmd;
        char c = d[index];
        switch(c)
        {
            case 'M': case 'm':
            case 'L': case 'l':
            case 'C': case 'c':
            case 'S': case 's':
            case 'Q': case 'q':
            case 'T': case 't':
            case 'H': case 'h':
            case 'V': case 'v':
            case 'A': case 'a':
            {
                cmd.command = c;
                index++;
                size_t new_cmd_pos = d.find_first_not_of("-0123456789. ", index);
                string params = d.substr(index, new_cmd_pos - index);
                for (string p: HelperUtils::stringSplit(params, ' '))
                {
                    if (p.size())
                    {
                        double x;
                        int dots_num = count(p.begin(), p.end(), '.');
                        if (dots_num < 2)
                        {
                            x = HelperUtils::parseDouble(p);
                            cmd.params.push_back(x);
                        }
                        else
                        {
                            int dot = p.find('.');
                            int sec_dot = p.find('.', dot + 1);
                            string first_coord = p.substr(0, sec_dot);
                            x = HelperUtils::parseDouble(first_coord);
                            cmd.params.push_back(x);
                            for (string c: HelperUtils::stringSplit(p.substr(sec_dot), '.'))
                            {
                                if (c.size())
                                {
                                    x = HelperUtils::parseDouble("." + c);
                                    cmd.params.push_back(x);
                                }
                            }
                        }
                    }
                }
                index = new_cmd_pos;
                path->commands.push_back(cmd);
                break;
            }
            case 'Z': case 'z':
            {
                cmd.command = c;
                index++;
                path->commands.push_back(cmd);
                break;
            }
            default:
                index++;
                break;
        }
    }

    if (trace)
    {
        cout << "getPath: d=";
        for (Command cmd: path->commands)
        {
            cout << " " << cmd.command;
            for (double p: cmd.params)
            {
                cout << " " << p;
            }
        }
        cout << endl;
    }

    return path;
}

SvgShape *SvgLoader::getGroup(XMLElement *element)
{
    SYNC_PRINT(("SvgLoader::getGroup(): called\n"));
    SvgGroup *group = new SvgGroup;
    initShape(element, group);

    XMLElement *current = element->FirstChildElement();

    while(current)
    {
        const char *name = current->Name();
        if (trace) {
            cout << "Processing <" << name << ">" << endl;
        }

        auto iter = shape_m.find(name);
        if (iter != shape_m.end())
        {
            f_type getShape = iter->second;
            SvgShape *shape = (this->*getShape)(current);
            group->shapes.push_back(shape);
        }

        current = current->NextSiblingElement();
    }
    return group;
}

RGB24Buffer* SvgFile::draw()
{
    RGB24Buffer *buffer = new RGB24Buffer(fround(height), fround(width), RGBColor::White());
    for (SvgShape *shape: shapes)
    {
            shape->draw(buffer);
    }
    return buffer;
}

string  SVGToRGB24BufferLoader::extension(".svg");

bool SVGToRGB24BufferLoader::acceptsFile(const std::string &name)
{
    std::string lowercase = name;
    std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), ::tolower);

    return HelperUtils::endsWith(lowercase, extension);
}

RGB24Buffer *SVGToRGB24BufferLoader::load(const string &name)
{
    SvgFile svg;
    SvgLoader loader;
    std::ifstream stream(name);
    if (!stream) {
        SYNC_PRINT(("can't open file\n"));
        return NULL;
    }

    loader.loadSvg(stream, svg);

   if (svg.shapes.size() == 8) {
       SYNC_PRINT(("Parsing problem\n"));
       return NULL;
   }

   return svg.draw();
}

void SvgGroup::draw(RGB24Buffer *buffer)
{
    for (SvgShape *shape: shapes)
    {
         shape->draw(buffer);
    }
}

void SvgPath::draw(RGB24Buffer *buffer)
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

    //BresenhamRasterizer rast = BresenhamRasterizer();
    //BezierRasterizer<RGB24Buffer, BresenhamRasterizer> bezier(*buffer, rast, color);

    for (size_t i = 0; i < commands.size(); i++)
    {
        const Command &command = commands[i];

        switch(command.command)
        {
        case 'M': case 'm':
            start_point = command.getVector();
            if (command.command == 'm')
            {
                start_point += cursor;
            }
            cursor = start_point;
            if (commands[i].params.size() > 2)
            {
                for (size_t j = 2; j < command.params.size(); j += 2)
                {
                    dest = command.getVector(j);
                    if (command.command == 'm')
                    {
                        dest += cursor;
                    }
                    buffer->drawLine(cursor, dest, color);
                    cursor = dest;
                }
            }
            break;
            /*=================================*/
        case 'L': case 'l':
            for (size_t j = 0; j < command.params.size(); j += 2)
            {
                dest = command.getVector(j);
                if (command.command == 'l')
                {
                    dest += cursor;
                }
                buffer->drawLine(cursor, dest, color);
                cursor = dest;
            }
            break;
            /*=================================*/
        case 'H': case 'h':
            for (size_t j = 0; j < command.params.size(); j++)
            {
                double x2 = command.params[j];
                if (command.command == 'h')
                {
                    x2 += cursor.x();
                }
                buffer->drawHLine(fround(cursor[0]), fround(cursor[1]), fround(x2), color);
                cursor[0] = x2;
            }
            break;
            /*=================================*/
        case 'V': case 'v':
            for (size_t j = 0; j < command.params.size(); j++)
            {
                double y2 = command.params[j];
                if (command.command == 'v')
                {
                    y2 += cursor.y();
                }
                buffer->drawVLine(fround(cursor[0]), fround(cursor[1]), fround(y2), color);
                cursor[1] = y2;
            }
            break;
            /*=================================*/
        case 'Z': case 'z':
            buffer->drawLine(cursor, start_point, color);
            cursor = start_point;
            break;
            /*=================================*/
        case 'C': case 'c':
            for (size_t j = 0; j < command.params.size(); j += 6)
            {
                control_b = command.getVector(j);
                control_c = command.getVector(j+2);
                dest      = command.getVector(j+4);
                if (command.command == 'c')
                {
                    control_b += cursor;
                    control_c += cursor;
                    dest += cursor;
                }
                curve =
                {
                    cursor,
                    control_b,
                    control_c,
                    dest
                };
                bezier.cubicBezierCasteljauApproximationByFlatness(curve);
                cursor = dest;
                last_p = control_c;
            }
            break;
            /*=================================*/
        case 'S': case 's':
            for (size_t j = 0; j < command.params.size(); j+= 2)
            {
                control_b = command.getVector(j);
                control_c = command.getVector(j);
                dest = command.getVector(j+2);
                if (command.command == 's')
                {
                    control_b += cursor;
                    control_c += cursor;
                    dest += cursor;
                }

                if (i > 0)
                {
                    char prevAction = commands[i-1].command;
                    if ((prevAction == 'C' || prevAction == 'c') ||
                        (prevAction == 'S' || prevAction == 's'))
                    {
                        control_b = 2 * cursor - last_p;
                    }
                }
                curve =
                {
                    cursor,
                    control_b,
                    control_c,
                    dest
                };
                bezier.cubicBezierCasteljauApproximationByFlatness(curve);
                cursor = dest;
                last_p = control_c;
            }
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

bool SvgPath::toPolygon(Polygon &p)
{
    if (commands.size() == 0)
    {
        return false;
    }

    cursor = {0, 0};
    start_point = {0, 0};
    dest = {0, 0};

    p.clear();

    for (size_t i = 0; i < commands.size(); i++)
    {
        const Command &command = commands[i];

        switch(command.command)
        {
        case 'M': case 'm':
            start_point = command.getVector();
            if (command.command == 'm')
            {
                start_point += cursor;
            }
            cursor = start_point;
            p.push_back(cursor);
            if (commands[i].params.size() > 2)
            {
                for (size_t j = 2; j < command.params.size(); j += 2)
                {
                    dest = command.getVector(j);
                    if (command.command == 'm')
                    {
                        dest += cursor;
                    }
                    cursor = dest;
                    p.push_back(cursor);
                }
            }
            break;
            /*=================================*/
        case 'L': case 'l':
            for (size_t j = 0; j < command.params.size(); j += 2)
            {
                dest = command.getVector(j);
                if (command.command == 'l')
                {
                    dest += cursor;
                }
                cursor = dest;
                p.push_back(cursor);
            }
            break;
            /*=================================*/
        case 'H': case 'h':
            for (size_t j = 0; j < command.params.size(); j++)
            {
                double x2 = command.params[j];
                if (command.command == 'h')
                {
                    x2 += cursor.x();
                }
                cursor.x() = x2;
                p.push_back(cursor);
            }
            break;
            /*=================================*/
        case 'V': case 'v':
            for (size_t j = 0; j < command.params.size(); j++)
            {
                double y2 = command.params[j];
                if (command.command == 'v')
                {
                    y2 += cursor.y();
                }
                cursor.y() = y2;
                p.push_back(cursor);
            }
            break;
            /*=================================*/
        case 'Z': case 'z':
            cursor = start_point;
            p.push_back(cursor);
            break;
            /*=================================*/
        case 'C': case 'c':
            return false;
            break;
            /*=================================*/
        case 'S': case 's':
            return false;
            break;
        case 'q':
        case 'Q':
        case 't':
        case 'T':
            return false;
            break;
        case 'a':
        case 'A':
            return false;
            break;
        }
    }

    return true;
}


/** ************************************************
 *
 *   Saver
 *
 ************************************************ **/
SvgSaver::SvgSaver()
{

}

SvgSaver::~SvgSaver()
{

}

int SvgSaver::saveSvg(ostream &output, SvgFile &svg)
{
     XMLDocument xml;
     XMLPrinter printer;
     xml.Print(&printer);
     output << printer.CStr();
     return true;
}

int SvgSaver::dumpSVG(XMLDocument &xml, SvgFile &svg)
{
//    xml.NewElement( svg.
    return 0;
}

XMLElement *SvgSaver::dumpRect(SvgRect *element)
{
    return NULL;
}

XMLElement *SvgSaver::dumpCircle(SvgCircle *element)
{
    return NULL;
}

XMLElement *SvgSaver::dumpEllipse(SvgEllipse *element)
{
    return NULL;
}

XMLElement *SvgSaver::dumpLine(SvgLine *element)
{
    return NULL;
}

XMLElement *SvgSaver::dumpPolyLine(SvgPolyLine *element)
{
    return NULL;
}

XMLElement *SvgSaver::dumpPolygon(SvgPolygon *element)
{
    return NULL;
}

XMLElement *SvgSaver::dumpPath(SvgPath *element)
{
    return NULL;
}

XMLElement *SvgSaver::dumpGroup(SvgGroup *element)
{
    return NULL;
}



}
