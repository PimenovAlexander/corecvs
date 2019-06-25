#include "labelGcodeInterpreter.h"
#include <regex>
#include <core/buffers/rgb24/abstractPainter.h>
#include <core/geometry/rectangle.h>

using namespace std;
using namespace corecvs;

LabelGcodeInterpreter::LabelGcodeInterpreter()
{
    part_label = "None";
    label = "None";
    rectangle = Rectangled::Empty();

}

void LabelGcodeInterpreter::labelObject()
{
    int SCALE = 10 * scale;
    int label_width = label.length() * 5 * SCALE;
    int part_label_width = part_label.length() * 5 * SCALE;

    if (!rectangle.isEmpty()) {
        AbstractPainter<RGB24Buffer> p(canvas);
        p.drawFormat(
           rectangle.center().x() - part_label_width / 2,
           rectangle.center().y() - 5 * SCALE * 0.75, RGBColor::Black(), SCALE, "%s", part_label.c_str());
        p.drawFormat(
           rectangle.center().x() - label_width / 2,
           rectangle.center().y() + 5 * SCALE * 0.75, RGBColor::Black(), SCALE, "%s", label.c_str());
    }

    rectangle = Rectangled::Empty();
}

void LabelGcodeInterpreter::executeProgram(const GCodeProgram &program)
{
    GCodeInterpreter::executeProgram(program);
    labelObject();
}

bool LabelGcodeInterpreter::commentHook(const std::string &text)
{
    SYNC_PRINT(("LabelGcodeInterpreter::commentHook (%s)\n", text.c_str()));

    labelObject();

    string input = text;
    //string input = "sbhcsjdbc 59_2_151551_6 sdfsd";
    regex re("(\\d*)_(\\d*)_(\\d*)_(\\d*)");
    //regex re("(\\d)(\\d)");
    smatch m;

    part_label="";
    label="";

    while (std::regex_search (input,m,re)) {
        part_label = m[1];
        label      = m[3];
        for (auto x:m) {
           std::cout << x << " ";
        }
        std::cout << std::endl;
        input = m.suffix().str();
    }
    return true;
}

bool LabelGcodeInterpreter::straightHook(int type, const GCodeInterpreter::MachineState &before, const GCodeInterpreter::MachineState &after)
{
    // SYNC_PRINT(("LabelGcodeInterpreter::straightHook (%d)\n", type));

    AbstractPainter<RGB24Buffer> p(canvas);
    Vector2dd pos1 = before.position.xy() * scale;
    Vector2dd pos2 = after.position.xy()  * scale;

    Vector2dd origin(canvas->w - 1, canvas->h - 1);
    pos1 = Vector2dd(origin.x() + pos1.x(), origin.y() - pos1.y());
    pos2 = Vector2dd(origin.x() + pos2.x(), origin.y() - pos2.y());

    //cout << "Line " << pos1 << "->" << pos2 << endl;

    if (type == 1) {
        canvas->drawLine(pos1, pos2, RGBColor::Gray());
        rectangle.extendToFit(pos1);
        rectangle.extendToFit(pos2);

        //SvgLine *line = new SvgLine(pos1, pos2);
        //svg->shapes.push_back(line);
    }
    return true;
}

bool LabelGcodeInterpreter::arkHook(const corecvs::GCodeInterpreter::MachineState &before, const corecvs::GCodeInterpreter::MachineState &after, const corecvs::PlaneFrame &frame, double maxArg)
{
    SYNC_PRINT(("LabelGcodeInterpreter::arkHook ():\n"));
    return true;
}
