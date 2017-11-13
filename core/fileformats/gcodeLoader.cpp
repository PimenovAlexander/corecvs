#include "core/fileformats/gcodeLoader.h"
#include "core/utils/utils.h"

#include <sstream>
#include <cctype>

namespace corecvs {

using std::istringstream;
using std::string;
using std::vector;
using std::pair;

GcodeLoader::GcodeLoader()
{}

vector<pair<char, double>> parseLine(const string& gline)
{    
    vector<pair<char, double>> result;
    vector<string> split = HelperUtils::stringSplit(gline, ' ');

    // cout << "parseLine("<< gline << "):called" << endl;
    for (string s: split)
    {
        // cout << "parseLine(): processing <" << s << ">" << endl;
        pair<char, double> p;

        if (s.empty())
            continue;

        if (isalpha(s[0])) {
            p.first = s[0];
        }
        s = s.substr(1);

        /*G CODE is locale independant so we need to parse double in C locale*/
        if (!s.empty()) {
            std::locale mylocale("C");
            istringstream ss(s);
            ss.imbue(mylocale);
            ss >> p.second;
            result.push_back(p);
        }
    }

    return result;
}


int GcodeLoader::loadGcode(istream &input, Mesh3D &mesh)
{
    GCodeProgram program;
    loadGcode(input, program);
    GCodeToMesh converter;
    converter.trace = trace;
    converter.renderToMesh(program, mesh);
    return 0;
}


#if 0
int GcodeLoader::loadGcode(istream &input, Mesh3D &mesh)
{
    string line;

    Vector3dd currentPosition = Vector3dd::Zero();
    mesh.switchColor(true);

    while (!input.eof())
    {
        HelperUtils::getlineSafe (input, line);

        if (line.empty())
            continue;

        if (trace) {
            cout << "Parsing line <" << line << ">" << endl;
        }

        std::transform(line.begin(), line.end(), line.begin(), ::tolower);

        /* Removing comments */
        line = HelperUtils::stringSplit(line, ';').front();
        if (line.empty())
            continue;
        line = HelperUtils::stringSplit(line, '(').front();
        if (line.empty())
            continue;

        if (trace) {
            cout << "Without comments <" << line << ">" << endl;
        }
        vector<pair<char, double>> tokens = parseLine(line);
        for (auto token : tokens)
        {
            cout << " " << token.first << " -> " << token.second << endl;
        }
        if (tokens.empty())
            continue;

        const auto &token = tokens[0];

        switch(token.first)
        {
            case 'g':
            {
                Vector3dd target = currentPosition;
                for (int i = 1; i < (int)tokens.size(); i++)
                {
                    switch (tokens[i].first) {
                        case 'x': target.x() = tokens[i].second; break;
                        case 'y': target.y() = tokens[i].second; break;
                        case 'z': target.z() = tokens[i].second; break;
                        default: break;
                    }
                }

                if (token.second == 0) {
                    cout << "G0 move" << endl;
                    mesh.setColor(RGBColor::Gray());
                    mesh.addLine(currentPosition, target);
                } else if (token.second == 1) {
                    cout << "G1 move" << endl;
                    mesh.setColor(RGBColor::Blue());
                    mesh.addLine(currentPosition, target);
                } else if (token.second == 2 || token.second == 3) {
                    Vector3dd center = currentPosition;
                    mesh.setColor(RGBColor::Yellow());

                    for (size_t i = 1; i < tokens.size(); i++)
                    {
                        switch (tokens[i].first) {
                            case 'i': center.x() += tokens[i].second; break;
                            case 'j': center.y() += tokens[i].second; break;
                            case 'k': center.z() += tokens[i].second; break;
                            default: break;
                        }
                    }

                    Vector3dd normal = (currentPosition - center) ^ (target - center);
                    normal.normalise();
                    Vector3dd axis1 = (currentPosition - center);
                    double r = (currentPosition - center).l2Metric();
                    axis1.normalise();;
                    Vector3dd axis2 = (axis1 ^ normal).normalised();
                    if (token.second == 3)
                        axis2 = -axis2;

                    Vector2dd proj;
                    proj.x() = (target-center) & axis1;
                    proj.y() = (target-center) & axis2;
                    double maxArg = proj.argument();

                    double arg = 0;
                    for (int i =0; i <= arcSteps; i++)
                    {
                        arg += maxArg / arcSteps;
                        Vector3dd subTarget = center + axis1 * r * cos(arg) + axis2 * r * sin(arg);
                        mesh.addLine(currentPosition, subTarget);
                        currentPosition = subTarget;
                    }

                    mesh.setColor(RGBColor::Pink());
                    mesh.addLine(currentPosition, target);

                } else {
                    mesh.setColor(RGBColor::Green());
                    mesh.addLine(currentPosition, target);
                }

                currentPosition = target;
                break;
            }
            case 'm': break;
        }

    }
    return 0;
}
#endif

int GCodeToMesh::renderToMeshOld(const GCodeProgram &program, Mesh3D &mesh)
{
    Vector3dd currentPosition = Vector3dd::Zero();
    mesh.switchColor(true);

    for (const GCodeProgram::Code &c: program.program)
    {
        if (c.area == ' ') {
            cout << ";" << c.comment << endl;
            continue;
        }

        if (c.area == 'g')
        {
            Vector3dd target = currentPosition;
            for (int i = 0; i < (int)c.parameters.size(); i++)
            {
                GCodeProgram::Record r = c.parameters[i];
                switch (r.address) {
                    case 'x': target.x() = r.value; break;
                    case 'y': target.y() = r.value; break;
                    case 'z': target.z() = r.value; break;
                    default: break;
                }
            }

            if (c.number == 0) {
//                cout << "G0 move" << endl;
                mesh.setColor(RGBColor::Gray());
                mesh.addLine(currentPosition, target);
            } else if (c.number == 1) {
//                cout << "G1 move" << endl;
                mesh.setColor(RGBColor::Blue());
                mesh.addLine(currentPosition, target);
            } else if (c.number == 2 || c.number == 3) {
                Vector3dd center = currentPosition;
                mesh.setColor(RGBColor::Yellow());

                for (int i = 0; i < (int)c.parameters.size(); i++)
                {
                    GCodeProgram::Record r = c.parameters[i];
                    switch (r.address) {
                        case 'i': center.x() += r.value; break;
                        case 'j': center.y() += r.value; break;
                        case 'k': center.z() += r.value; break;
                        default: break;
                    }
                }

                Vector3dd normal = (currentPosition - center) ^ (target - center);
                normal.normalise();
                Vector3dd axis1 = (currentPosition - center);
                double r = (currentPosition - center).l2Metric();
                axis1.normalise();;
                Vector3dd axis2 = (axis1 ^ normal).normalised();
                if (c.number == 3)
                    axis2 = -axis2;

                Vector2dd proj;
                proj.x() = (target-center) & axis1;
                proj.y() = (target-center) & axis2;
                double maxArg = proj.argument();

                double arg = 0;
                for (int i = 0; i <= arcSteps; i++)
                {
                    arg += maxArg / arcSteps;
                    Vector3dd subTarget = center + axis1 * r * cos(arg) + axis2 * r * sin(arg);
                    mesh.addLine(currentPosition, subTarget);
                    currentPosition = subTarget;
                }

                mesh.setColor(RGBColor::Pink());
                mesh.addLine(currentPosition, target);

            } else {
                mesh.setColor(RGBColor::Green());
                mesh.addLine(currentPosition, target);
            }

            currentPosition = target;

        }
    }
    return 0;
}


int GcodeLoader::loadGcode(std::istream &input, GCodeProgram &program)
{
    string line;

    program.program.clear();

    while (!input.eof())
    {
        HelperUtils::getlineSafe (input, line);

        if (line.empty())
            continue;

        if (trace) {
            cout << "Parsing line <" << line << ">" << endl;
        }

        GCodeProgram::Code code;
        /* Processing comments */
        line = HelperUtils::removeLeading(line, " \t");

        std::vector<string> parts;
        parts = HelperUtils::stringSplit(line, ';');
        line = parts.front();
        parts.erase(parts.begin() + 0, parts.begin() + 1); // Stupid C++;
        if (parts.size() > 0)
        {
            code.comment += HelperUtils::stringCombine(parts, ';');
        }

        if (line.empty())
        {
            program.program.push_back(code);
            continue;
        }

        parts = HelperUtils::stringSplit(line, '(');
        line = parts.front();
        parts.erase(parts.begin() + 0, parts.begin() + 1); // Stupid C++;
        if (parts.size() > 0)
        {
            code.comment += HelperUtils::stringCombine(parts, '(');
        }

        if (line.empty())
        {
            program.program.push_back(code);
            continue;
        }

        std::transform(line.begin(), line.end(), line.begin(), ::tolower);

        if (trace) {
            cout << "Without comments <" << line << ">" << endl;
        }

        vector<pair<char, double>> tokens = parseLine(line);
        /*for (auto token : tokens)
        {
            cout << " " << token.first << " -> " << token.second << endl;
        }*/
        if (tokens.empty())
            continue;

        const auto &token = tokens[0];
        code.area = token.first;
        code.number = token.second;

        for (int i = 1; i < (int)tokens.size(); i++)
        {
            GCodeProgram::Record r = {tokens[i].first, tokens[i].second};
            code.parameters.push_back(r);
        }

        program.program.push_back(code);
    }
    return 0;
}



int GcodeLoader::saveGcode(std::ostream &out, const GCodeProgram &program)
{
    for (const GCodeProgram::Code &c: program.program)
    {
        if (c.area != ' ')
        {
            out << (char)toupper(c.area) << c.number << " ";
            for (GCodeProgram::Record r : c.parameters) {
                out << (char)toupper(r.address) << r.value << " ";
            }
        }

        if (!c.comment.empty()) {
            out << "; " << c.comment;
        }

        out << endl;
    }
    return 0;
}

GcodeLoader::~GcodeLoader()
{}

/** Interpreter **/

GCodeInterpreter::MachineState *GCodeInterpreter::createState()
{
    return new GCodeInterpreter::MachineState;
}

void GCodeInterpreter::executeProgram(const GCodeProgram &program)
{
    if (state == NULL)
        state = createState();

    for (size_t num = 0; num < program.program.size(); num++)
    {
        const GCodeProgram::Code &c = program.program[num];
        if (this->gcodeHook(c)) {
            continue;
        }


        if (c.area == ' ') {
            if (traceComment) {
                cout << ";" << c.comment << endl;
            }
            continue;
        }

        if (c.area == 'm')
        {
            switch (c.number)
            {
            case 82:
                state->extruderRealtive = false;
                break;
            case 83:
                state->extruderRealtive = true;
                break;
            case 104:
                    if (c.hasParameter('s')) {
                        state->extruderTemperature = c.getParameter('s');
                        if (traceProcess) {
                            cout << "Setting extruder temperature " << state->extruderTemperature << endl;
                        }
                    }
                break;
            case 140:
                    if (c.hasParameter('s'))
                        state->bedTemperature = c.getParameter('s');
                        if (traceProcess) {
                            cout << "Setting bed temperature " << state->extruderTemperature << endl;
                        }
                break;
            default:
                break;
            }
        }

        if (c.area == 'g')
        {
            MachineState newState = *state;

            for (int i = 0; i < (int)c.parameters.size(); i++)
            {
                GCodeProgram::Record r = c.parameters[i];
                switch (r.address) {
                    case 'x': newState.position.x() = r.value; break;
                    case 'y': newState.position.y() = r.value; break;
                    case 'z': newState.position.z() = r.value; break;

                    case 'e' : newState.extruderPos = state->extruderRealtive ? (r.value + newState.extruderPos) : r.value; break;
                    case 'f' : newState.feedRate    = r.value; break;
                    default: break;
                }
            }

            if (c.number == 0) {
                cout << "G0 move" << endl;
                //mesh.setColor(RGBColor::Gray());
                //mesh.addLine(currentPosition, target);
                straightHook(0, *state, newState);
            }
            else if (c.number == 1) {
//                cout << "G1 move" << endl;
                //mesh.setColor(RGBColor::Blue());
                //mesh.addLine(currentPosition, target);
                straightHook(1, *state, newState);
            }
            else if (c.number == 2 || c.number == 3) {
                Vector3dd center = newState.position;
                //mesh.setColor(RGBColor::Yellow());

                for (int i = 0; i < (int)c.parameters.size(); i++)
                {
                    GCodeProgram::Record r = c.parameters[i];
                    switch (r.address) {
                        case 'i': center.x() += r.value; break;
                        case 'j': center.y() += r.value; break;
                        case 'k': center.z() += r.value; break;
                        default: break;
                    }
                }

                Vector3dd normal = (state->position - center) ^ (newState.position - center);
                normal.normalise();
                Vector3dd axis1 = (state->position - center);
                double r = axis1.l2Metric();
                axis1.normalise();
                Vector3dd axis2 = (axis1 ^ normal).normalised();
                if (c.number == 3)
                    axis2 = -axis2;

                Vector2dd proj;
                proj.x() = (newState.position - center) & axis1;
                proj.y() = (newState.position - center) & axis2;
                double maxArg = proj.argument();

                PlaneFrame frame(center, axis1 * r, axis2 *r);

                arkHook(*state, newState, frame, maxArg);

                /*
                double arg = 0;
                for (int i = 0; i <= arcSteps; i++)
                {
                    arg += maxArg / arcSteps;
                    Vector3dd subTarget = center + axis1 * r * cos(arg) + axis2 * r * sin(arg);
                    mesh.addLine(currentPosition, subTarget);
                    currentPosition = subTarget;
                }
                */
                /*mesh.setColor(RGBColor::Pink());
                mesh.addLine(currentPosition, target);*/
            }
            else {
                errorHook();
                /*mesh.setColor(RGBColor::Green());
                mesh.addLine(currentPosition, target);*/
            }

            *state = newState;
        }
    }
}


/**
 * These hooks could either
 *  return true - when this method has taken care of the gcode
 *  return flase - is you want default implemenation update the machine state
 **/
bool GCodeInterpreter::gcodeHook    ( const GCodeProgram::Code &/*code*/ )
{
    return false;
}

bool GCodeInterpreter::straightHook (int /*type*/, const MachineState &/*before*/, const MachineState &/*after*/)
{
    return true;
}

bool GCodeInterpreter::arkHook(const MachineState &/*before*/, const MachineState &/*after*/, const PlaneFrame &/*frame*/, double /*maxArg*/ )
{

    return true;
}

void GCodeInterpreter::errorHook(void)
{}

GCodeInterpreter::~GCodeInterpreter()
{
    delete_safe(state);
}

/*void GCodeToMesh::setColor(Mesh3D &mesh, const MachineState &before, const MachineState &after)
{

}*/


bool GCodeToMesh::MeshInterpreter::straightHook(int type, const GCodeInterpreter::MachineState &before, const GCodeInterpreter::MachineState &after)
{
    double len = (after.position - before.position).l2Metric();
    double erate = 0;
    if (len != 0) {
        erate = (after.extruderPos - before.extruderPos) / len;
    }

    DrawGCodeParameters &p = parent->mParameters;

    switch (p.scheme()) {
        default:
        case GCodeColoringSheme::COLOR_FROM_GCODE:          mesh->setColor(type ? RGBColor::Green() : RGBColor::Blue()); break;
        case GCodeColoringSheme::COLOR_FROM_EXTRUSION_RATE: mesh->setColor(RGBColor::rainbow(lerp(0.0,1.0,                   erate,   p.minExtrude(), p.maxExtrude()))); break;
        case GCodeColoringSheme::COLOR_FROM_TEMPERATURE:    mesh->setColor(RGBColor::rainbow(lerp(0.0,1.0,after.extruderTemperature, p.minTemp(), p.maxTemp()))); break;
        case GCodeColoringSheme::COLOR_FROM_SPEED:          mesh->setColor(RGBColor::rainbow(lerp(0.0,1.0,after.feedRate, p.minSpeed(), p.maxSpeed()))); break;
    }

    mesh->addLine(before.position, after.position);
    return true;
}

bool GCodeToMesh::MeshInterpreter::arkHook(const GCodeInterpreter::MachineState &before, const GCodeInterpreter::MachineState &after, const PlaneFrame &frame, double maxArg)
{
    Vector3dd currentPosition = before.position;

    DrawGCodeParameters &p = parent->mParameters;
    switch (p.scheme()) {
        default:
        case GCodeColoringSheme::COLOR_FROM_GCODE:          mesh->setColor(RGBColor::Yellow()); break;
        case GCodeColoringSheme::COLOR_FROM_EXTRUSION_RATE: mesh->setColor(RGBColor::rainbow(0.5)); break;
        case GCodeColoringSheme::COLOR_FROM_TEMPERATURE:    mesh->setColor(RGBColor::rainbow(lerp(0.0,1.0,after.extruderTemperature, p.minTemp(), p.maxTemp()))); break;
        case GCodeColoringSheme::COLOR_FROM_SPEED:          mesh->setColor(RGBColor::rainbow(lerp(0.0,1.0,after.feedRate, p.minSpeed(), p.maxSpeed()))); break;
    }

    double arg = 0;
    for (int i = 0; i <= parent->arcSteps; i++)
    {
        arg += maxArg / parent->arcSteps;
        Vector3dd subTarget = frame.p1 + frame.e1 * cos(arg) + frame.e2 * sin(arg);
        mesh->addLine(currentPosition, subTarget);
        currentPosition = subTarget;
    }

    mesh->setColor(RGBColor::Red());
    mesh->addLine(currentPosition, after.position);
    return true;
}

int GCodeToMesh::renderToMesh(const GCodeProgram &in, Mesh3D &mesh)
{
    SYNC_PRINT(("GCodeToMesh::renderToMesh(): called\n"));
    SYNC_PRINT(("GCodeToMesh::renderToMesh(): style %d\n", mParameters.scheme()));

    mesh.switchColor(true);
    MeshInterpreter interpreter(this, &mesh);
    interpreter.executeProgram(in);
    return 0;
}


} // namespace corecvs
