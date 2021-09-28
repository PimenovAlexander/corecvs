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

    mean = Vector2dd::Zero();
    count = 0;
}

void LabelGcodeInterpreter::labelObject()
{
    if (!rectangle.isEmpty() && count != 0) {
        //Vector2dd pos = rectangle.center();
        Vector2dd pos = mean / count;
        labelList.emplace_back(label, part_label, pos);
        SYNC_PRINT(("Adding %s, %s, [%lf, %lf]\n",
            labelList.back().l1.c_str(),
            labelList.back().l2.c_str(),
            labelList.back().pos.x(),
            labelList.back().pos.y()));
    }

    rectangle = Rectangled::Empty();
    mean = Vector2dd::Zero();
    count = 0;
}

void LabelGcodeInterpreter::drawLabels()
{
    int SCALE = /*4 * scale*/ 1;

    for (Label &l : labelList)
    {

        int label_width = l.l1.length() * 5 * SCALE;
        int part_label_width = l.l2.length() * 5 * SCALE;

        AbstractPainter<RGB24Buffer> p(canvas);

        Vector2dd pos = l.pos;
        p.drawFormat(
           pos.x() - part_label_width / 2,
           pos.y() - 5 * SCALE * 0.75, RGBColor::Black(), SCALE, "%s", l.l2.c_str());
        p.drawFormat(
           pos.x() - label_width / 2,
           pos.y() + 5 * SCALE * 0.75, RGBColor::Black(), SCALE, "%s", l.l1.c_str());
    }

}

void LabelGcodeInterpreter::executeProgram(const GCodeProgram &program)
{
    GCodeInterpreter::executeProgram(program);
    labelObject();
    drawLabels();
}

bool LabelGcodeInterpreter::commentHook(const std::string &text)
{
    SYNC_PRINT(("LabelGcodeInterpreter::commentHook (%s)\n", text.c_str()));

    labelObject();

    string input = text;
    //string input = "sbhcsjdbc 59_2_151551_6 sdfsd";
    //regex re("(\\d*)_(\\d*)_(\\d*)_(\\d*)");
    //regex re("(\\d)(\\d)");

    regex re("Seq (\\d+) - [^0-9]*(\\d+)[^0-9]*(\\d+)");
    smatch m;

    part_label="";
    label="";

    while (std::regex_search (input,m,re)) {
        part_label = m[2];
        label      = m[3];
        for (auto x:m) {
           std::cout << x << "|";
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
        canvas->drawLine(pos1, pos2, RGBColor::Gray(180));
        rectangle.extendToFit(pos1);
        rectangle.extendToFit(pos2);

        mean += pos2;
        count++;

        //SvgLine *line = new SvgLine(pos1, pos2);
        //svg->shapes.push_back(line);
    }
    return true;
}

bool LabelGcodeInterpreter::arkHook(const corecvs::GCodeInterpreter::MachineState &before, const corecvs::GCodeInterpreter::MachineState &after, const corecvs::PlaneFrame &frame, double maxArg)
{
    SYNC_PRINT(("LabelGcodeInterpreter::arkHook ():\n"));

    AbstractPainter<RGB24Buffer> p(canvas);
    Vector2dd pos1 = before.position.xy() * scale;
    Vector2dd pos2 = after.position.xy()  * scale;

    Vector2dd origin(canvas->w - 1, canvas->h - 1);

    double arg1 = 0.0;
    double arg2 = 0.0;

    for (int i = 0; i <= 60; i++) {
        arg1 = arg2;
        arg2 += maxArg / 60.0;
        pos1 = (frame.p1 + frame.e1 * cos(arg1) + frame.e2 * sin(arg1)).xy() * scale;
        pos2 = (frame.p1 + frame.e1 * cos(arg2) + frame.e2 * sin(arg2)).xy() * scale;

        pos1 = Vector2dd(origin.x() + pos1.x(), origin.y() - pos1.y());
        pos2 = Vector2dd(origin.x() + pos2.x(), origin.y() - pos2.y());
        canvas->drawLine(pos1, pos2, RGBColor::Gray(180));
        rectangle.extendToFit(pos1);
        rectangle.extendToFit(pos2);

        mean += pos2;
        count++;

    }

    return true;
}
