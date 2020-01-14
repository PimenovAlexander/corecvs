#include "core/fileformats/gcodeLoader.h"
#include "core/utils/utils.h"

#include <sstream>
#include <cctype>

namespace corecvs {

using std::istringstream;
using std::string;
using std::vector;
using std::pair;
using std::istream;

GcodeLoader::GcodeLoader()
{}

vector<pair<char, double>> GcodeLoader::parseLine1(const string& gline)
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

vector<pair<char, double>> GcodeLoader::parseLine(const string& gline)
{
    // cout << "parseLine("<< gline << "):called" << endl;

    vector<pair<char, double>> result;
    size_t pos = 0;
    while (true)
    {
        pair<char, double> p;

        pos = gline.find_first_of("gxyzesfijm", pos);
        // cout << "pos:" << pos << endl;

        if (pos == string::npos)
            break;

        p.first = gline[pos];

        /*G CODE is locale independant so we need to parse double in C locale*/
        std::locale mylocale("C");
        istringstream ss(gline.substr(pos+1));
        ss.imbue(mylocale);
        ss >> p.second;
        result.push_back(p);
        pos += 1;
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

        if (!c.comment.empty()) {
            SYNC_PRINT(("calling commentHook (%s)\n", c.comment.c_str()));
            commentHook(c.comment);
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
                if (c.hasParameter('s')) {
                    state->bedTemperature = c.getParameter('s');
                    if (traceProcess) {
                        cout << "Setting bed temperature " << state->bedTemperature << endl;
                    }
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
                Vector3dd center = state->position;
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

bool GCodeInterpreter::commentHook(const std::string &text)
{
    SYNC_PRINT(("LabelGcodeInterpreter::commentHook (%s)\n", text.c_str()));
    return true;
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

int GCodeToMesh::renderToMesh(const GCodeProgram &in, Mesh3D &mesh,
                                         double offset) {
    SYNC_PRINT(("GCodeToMesh::renderToMesh(): called\n"));
    SYNC_PRINT(
        ("GCodeToMesh::renderToMesh(): style %d\n", mParameters.scheme()));

    mesh.switchColor(true);
    VinylCutterMeshInterpreter interpreter(this, &mesh, offset);
    interpreter.executeProgram(in);

    return 0;
}


bool GCodeToMesh::VinylCutterMeshInterpreter::straightHook(
    int type, const GCodeInterpreter::MachineState &before,
    const GCodeInterpreter::MachineState &after) {

    double len = (after.position - before.position).l2Metric();
    double erate = 0;
    if (len != 0) {
        erate = (after.extruderPos - before.extruderPos) / len;
    }

    DrawGCodeParameters &p = parent->mParameters;

    if (type == 0) {
        mesh->setColor(RGBColor::Gray());
        mesh->addLine(before.position, after.position);
        return true;
    }

    switch (p.scheme()) {
        default:
        case GCodeColoringSheme::COLOR_FROM_GCODE:
            mesh->setColor(type ? RGBColor::Green() : RGBColor::Blue());
            break;
        case GCodeColoringSheme::COLOR_FROM_EXTRUSION_RATE:
            mesh->setColor(RGBColor::rainbow(
                lerp(0.0, 1.0, erate, p.minExtrude(), p.maxExtrude())));
            break;
        case GCodeColoringSheme::COLOR_FROM_TEMPERATURE:
            mesh->setColor(
                RGBColor::rainbow(lerp(0.0, 1.0, after.extruderTemperature,
                                       p.minTemp(), p.maxTemp())));
            break;
        case GCodeColoringSheme::COLOR_FROM_SPEED:
            mesh->setColor(RGBColor::rainbow(
                lerp(0.0, 1.0, after.feedRate, p.minSpeed(), p.maxSpeed())));
            break;
    }

    // Initial knife position
    knifePos = before.position - tangent * offset;
    Vector3dd direction = (after.position - before.position).normalised();

    if (direction.angleTo(tangent) < 90) {
        SYNC_PRINT((
            "VinylCutterInterpreter: sharp angle detected! Not handled "
            "correctly, as it leads to unpredictable behavior of the blade\n"));
    }

    Vector3dd currentPosition = before.position;
    double step = len / parent->arcSteps;

    for (int i = 0; i <= parent->arcSteps; i++) {
        Vector3dd subTarget = knifePos + tangent * step;
        mesh->addLine(knifePos, subTarget);
        knifePos = subTarget;
        currentPosition += direction * step;
        tangent = (currentPosition - knifePos).normalised();
    }

    return true;
}

bool GCodeToMesh::VinylCutterMeshInterpreter::arkHook(
    const GCodeInterpreter::MachineState &before,
    const GCodeInterpreter::MachineState &after, const PlaneFrame &frame,
    double maxArg) {

    MachineState currentState = before;
    MachineState subTarget = before;

    DrawGCodeParameters &p = parent->mParameters;
    switch (p.scheme()) {
        default:
        case GCodeColoringSheme::COLOR_FROM_GCODE:
            mesh->setColor(RGBColor::Yellow());
            break;
        case GCodeColoringSheme::COLOR_FROM_EXTRUSION_RATE:
            mesh->setColor(RGBColor::rainbow(0.5));
            break;
        case GCodeColoringSheme::COLOR_FROM_TEMPERATURE:
            mesh->setColor(
                RGBColor::rainbow(lerp(0.0, 1.0, after.extruderTemperature,
                                       p.minTemp(), p.maxTemp())));
            break;
        case GCodeColoringSheme::COLOR_FROM_SPEED:
            mesh->setColor(RGBColor::rainbow(
                lerp(0.0, 1.0, after.feedRate, p.minSpeed(), p.maxSpeed())));
            break;
    }

    double arg = 0;

    // Approximates arc with line segments
    for (int i = 0; i <= parent->arcSteps; i++) {
        arg += maxArg / parent->arcSteps;
        subTarget.position =
            frame.p1 + frame.e1 * cos(arg) + frame.e2 * sin(arg);
        GCodeInterpreter::MachineState subState;
        straightHook(1, currentState, subTarget);
        currentState = subTarget;
    }

    // Final line segment
    mesh->setColor(RGBColor::Red());
    straightHook(1, currentState, after);

    return true;
}


bool GCodeCompensator::VinylCutterCodeInterpreter::gcodeHook(
    const corecvs::GCodeProgram::Code &c) {

  corecvs::GCodeProgram::Code copy = c;

  result->program.push_back(copy);

  step++;

  return false;
}

bool GCodeCompensator::VinylCutterCodeInterpreter::straightHook(
    int type, const GCodeInterpreter::MachineState &before,
    const GCodeInterpreter::MachineState &after) {

  if (((before.position.z() > touchZ && after.position.z() < touchZ) ||
       (before.position.z() < touchZ && after.position.z() > touchZ)) &&
      (before.position.x() != after.position.x() ||
       before.position.y() != after.position.y())) {
    SYNC_PRINT(("VinylCutterInterpreter: Warning! The knife touches/loses "
                "touch with non-zero horizontal or vertical movement.\n"));
  }

  if (type == 0) {
    if (before.position.z() < touchZ && after.position.z() < touchZ) {
      SYNC_PRINT(("VinylCutterInterpreter: Warning! Rapid positioning below "
                  "cutting position! Not calculating offset for that.\n"));
    }
    return true;
  }

  if (before.position.z() < touchZ && after.position.z() < touchZ) {

    if (reboundStep == step) {
      Vector2dd tangentVec =
          (after.position - before.position).normalised().xy() * bladeOffset;

      corecvs::GCodeProgram::Record rebound_x, rebound_y, rebound_z, rebound_i, rebound_j, rebound_k;

      rebound_x.address = 'x';
      rebound_x.value = before.position.x() + tangentVec.x();
      rebound_y.address = 'y';
      rebound_y.value = before.position.y() + tangentVec.y();
      rebound_z.address = 'z';
      rebound_z.value = after.position.z();
      rebound_i.address = 'i';
      rebound_i.value = reboundCenter.x();
      rebound_j.address = 'j';
      rebound_j.value = reboundCenter.y();
      rebound_k.address = 'k';
      rebound_k.value = reboundCenter.z();

      corecvs::GCodeProgram::Code rebound;
      rebound.area = 'g';
      rebound.number = tangentVec.azimuthTo(tangent) > 0 ? 2 : 3;
      rebound.parameters = {rebound_x, rebound_y, rebound_z, rebound_i, rebound_j, rebound_k};

      result->program.insert(result->program.end() - 1, rebound);
    }

    Vector2dd overcutVec =
        (after.position - before.position).normalised().xy() * bladeOffset;

    corecvs::GCodeProgram::Record overcut_x, overcut_y, overcut_z;

    overcut_x.address = 'x';
    overcut_x.value = after.position.x() + overcutVec.x();
    overcut_y.address = 'y';
    overcut_y.value = after.position.y() + overcutVec.y();
    overcut_z.address = 'z';
    overcut_z.value = after.position.z();

    corecvs::GCodeProgram::Code overcut;
    overcut.area = 'g';
    overcut.number = 1;
    overcut.parameters = {overcut_x, overcut_y, overcut_z };

    reboundCenter = - Vector3dd(overcutVec.x(), overcutVec.y(), 0);
    reboundStep = step + 1;
    tangent = overcutVec;

    result->program.push_back(overcut);
  }

  return true;
}

bool GCodeCompensator::VinylCutterCodeInterpreter::arkHook(
    const GCodeInterpreter::MachineState &before,
    const GCodeInterpreter::MachineState &after, const PlaneFrame &frame,
    double maxArg) {
  CORE_UNUSED(before);
  CORE_UNUSED(after);
  CORE_UNUSED(frame);
  CORE_UNUSED(maxArg);

  if (((before.position.z() > touchZ && after.position.z() < touchZ) ||
       (before.position.z() < touchZ && after.position.z() > touchZ)) &&
      (before.position.x() != after.position.x() ||
       before.position.y() != after.position.y())) {
    SYNC_PRINT(("VinylCutterInterpreter: Warning! The kinfe touches/loses "
                "touch with non-zero horizontal or vertical movement.\n"));
  }

  if (before.position.z() < touchZ && after.position.z() < touchZ) {
    if (reboundStep == step) {
      SYNC_PRINT(("Corners\n"));
      Vector2dd tangentVec =
          (after.position - before.position).normalised().xy() * bladeOffset;

      corecvs::GCodeProgram::Record rebound_x, rebound_y, rebound_z, rebound_i, rebound_j, rebound_k;

      rebound_x.address = 'x';
      rebound_x.value = before.position.x() + tangentVec.x();
      rebound_y.address = 'y';
      rebound_y.value = before.position.y() + tangentVec.y();
      rebound_z.address = 'z';
      rebound_z.value = after.position.z();
      rebound_i.address = 'i';
      rebound_i.value = reboundCenter.x();
      rebound_j.address = 'j';
      rebound_j.value = reboundCenter.y();
      rebound_k.address = 'k';
      rebound_k.value = reboundCenter.z();

      corecvs::GCodeProgram::Code rebound;
      rebound.area = 'g';
      rebound.number = tangentVec.azimuthTo(tangent) > 0 ? 2 : 3; // TODO:
      rebound.parameters = {rebound_x, rebound_y, rebound_z, rebound_i, rebound_j, rebound_k};

      result->program.insert(result->program.end() - 1, rebound);
    }
  }

  return true;
}

void GCodeCompensator::compensateDragKnife(const GCodeProgram &in,
                                           double offset, double touchZ) {
    SYNC_PRINT(("GCodeCompensator::compensateDragKnife(): called\n"));
    VinylCutterCodeInterpreter interpreter =
        VinylCutterCodeInterpreter(&result, offset, touchZ);
    interpreter.executeProgram(in);
};


GCodeCompensator::GCodeCompensator()
{}

GCodeCompensator::~GCodeCompensator()
{}

} // namespace corecvs
