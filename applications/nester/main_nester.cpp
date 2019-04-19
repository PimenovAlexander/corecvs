#include <fstream>

#include "core/utils/utils.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/buffers/rgb24/rgb24Buffer.h"

#include "core/fileformats/svgLoader.h"
#include <core/reflection/commandLineSetter.h>

#include <core/buffers/bufferFactory.h>

using namespace corecvs;
using namespace std;

void addSubPolygons (SvgShape *shape, vector<Polygon> &inputPolygons)
{
    SYNC_PRINT(("addSubPolygons(%s):called\n", SvgShape::getName(shape->type)));
    if (shape->type == SvgShape::POLYGON_SHAPE)
    {
        SvgPolygon *polygon = static_cast<SvgPolygon*>(shape);
        SYNC_PRINT(("adding polygon of %d sides\n", polygon->polygon.size()));
        inputPolygons.push_back(polygon->polygon);
    }
    if (shape->type == SvgShape::GROUP_SHAPE)
    {
        SvgGroup *group = static_cast<SvgGroup*>(shape);
        for (SvgShape * s : group->shapes)
        {
            addSubPolygons(s, inputPolygons);
        }
    }
}

int main(int argc, char **argv)
{
    CommandLineSetter s(argc, argv);
    vector<std::string> params = s.nonPrefix();

    std::string svgName = (params.size() < 2) ? "poly.svg" : params[1];

    SvgFile svg;
    SvgLoader loader;

    std::ifstream svgFile;

    svgFile.open(svgName, std::ifstream::in);
    if (svgFile.fail())
    {
        SYNC_PRINT(("Can't open svg file <%s> for reading\n", svgName.c_str()));
        return 1;
    } else {
        SYNC_PRINT(("Opened svg file <%s> for reading\n", svgName.c_str()));
    }

    loader.loadSvg(svgFile, svg);

    vector<Polygon> inputPolygons;
    for (auto s: svg.shapes )
    {
       addSubPolygons(s, inputPolygons);
    }

    SYNC_PRINT(("Loaded %d polygons\n", inputPolygons.size()));

    /****
     *  Put your code here
     ****/


    /* Some debug output */
    Rectangled area = Rectangled::Empty();
    for (Polygon& p: inputPolygons )
    {
        for (Vector2dd &v : p) {
            cout << "Point:" << v << endl;
            area.extendToFit(v);
        }
    }

    cout << "Surrounding Area " << area << endl;

    int h = s.getInt("h", 1000);
    int w = s.getInt("w", 1000);


    RGB24Buffer *buffer = new RGB24Buffer(h,w);
    Vector2dd scale(buffer->w, buffer->h);

    for (Polygon& p: inputPolygons )
    {
        for (size_t i = 0; i < p.size(); i++)
        {
            Vector2dd  p1 = ((p.getPoint    (i) - area.corner) / area.size) * scale;
            Vector2dd  p2 = ((p.getNextPoint(i) - area.corner) / area.size) * scale;

            cout << "Point:" << p.getPoint(i) << "=>" << (p.getPoint(i) - area.corner) << "=>" << p1 << endl;

            buffer->drawLine(
                    p1, p2,
                    RGBColor::parula((i + 1.0) / p.size()));
        }
    }

    BufferFactory::getInstance()->saveRGB24Bitmap(buffer, "debug.bmp");

    return 0;

}


