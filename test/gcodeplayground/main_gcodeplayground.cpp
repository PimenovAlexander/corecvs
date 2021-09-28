#include <fstream>


#include "core/reflection/commandLineSetter.h"

#include "core/buffers/bufferFactory.h"

#include "core/fileformats/gcodeLoader.h"
#include "core/fileformats/pltLoader.h"
#include "core/filesystem/folderScanner.h"

#include "labelGcodeInterpreter.h"

#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

using namespace std;
using namespace corecvs;

int vynilCutter (double offset)
{
    CORE_UNUSED(offset);

    std::string input = "in.gcode";
    std::string output = "out.gcode";



    GCodeProgram program;
    GcodeLoader loader;

    ifstream file;
    file.open(input, ios::in);
    if (file.fail())
    {
        SYNC_PRINT(("g code playground::load(): Can't open mesh file <%s>\n", input.c_str()));
        return false;
    }

    loader.loadGcode(file, program);

    GCodeProgram programR;





    ofstream outfile;
    outfile.open(output, std::ostream::out);
    loader.saveGcode(outfile, programR);
    outfile.close();
    return 0;
}



int main1 (int argc, char **argv)
{
    std::string input = "";
    std::string output = "a.gcode";


    if (argc == 2)
    {
        input = argv[1];
    } else if (argc == 3)
    {
        input  = argv[1];
        output = argv[2];
    } else {
        SYNC_PRINT(("Usage rawconverter <input.gcode> [<output.gcode>]\n"));
        SYNC_PRINT((" default output is a.gcode \n"));
        return 1;
    }

    GCodeProgram program;
    GcodeLoader loader;

    ifstream file;
    file.open(input, ios::in);
    if (file.fail())
    {
        SYNC_PRINT(("g code playground::load(): Can't open mesh file <%s>\n", input.c_str()));
        return false;
    }

    loader.loadGcode(file, program);

    double z = 0;
    double percentAdd   = (250.0-40.0) / 6.0;
    double percentStart = 40.0;

    for (size_t i = 0; i < program.program.size(); i++)
    {
        GCodeProgram::Code &c = program.program[i];
        if (c.hasParameter('z')) {
            z = c.getParameter('z');
        }

        int part = 0;
        if (z > 2) part = (z - 2) / 10;

        double factor = percentStart + part * percentAdd;

        if (c.hasParameter('f')) {
            c.setParameter('f', c.getParameter('f') * (factor / 100.0));
        }

    }

    ofstream outfile;
    outfile.open(output, std::ostream::out);
    loader.saveGcode(outfile, program);
    outfile.close();
    return 0;
}


int main2 (int argc, char **argv)
{
    HPGLLoader loader;
    HPGLProgram program;

    if (argc != 2) {
        return 0;
    }


    ifstream file;
    std::string input = argv[1];

    file.open(input, ios::in);
    if (file.fail())
    {
        SYNC_PRINT(("plt playground::load(): Can't open plt file <%s>\n", input.c_str()));
        return false;
    }

    loader.loadHPGLcode(file, program);

    /*Prepare output*/
    ofstream ofile;
    ofile.open(input + ".gcode", ios::out);

    ofile <<
             "G21         ; Set units to mm\n"
             "G90         ; Absolute positioning\n"
             "M221 S100   ; Power on\n"
             "G1 F500     ; Feed rate\n"
             "\n";



    /* Mesh output */


    Mesh3D mesh;
    mesh.switchColor();

    Vector3dd pos(0.0);
    mesh.setColor(RGBColor::Green());
    bool on = false;

    bool moved = true;

    for (size_t t = 0; t < program.program.size(); t++)
    {
        HPGLProgram::Record &r = program.program[t];

        if (r.command == "PA") {
            Vector2dd p1(r.params[0], r.params[1]);

            Vector2dd gout = (p1 / 40.0) - Vector2dd(0, 100);

            Vector3dd meshout(gout, 0.0);

            mesh.addLine(pos, meshout);
            pos = meshout;


            if (on) {
                ofile << "G1 X" << gout.x() << " Y" << gout.y() << " S1.0" << endl;
            } else {
                ofile << "G0 X" << gout.x() << " Y" << gout.y() << endl;
            }

            moved = true;
        }
        if (r.command == "PU") {
            if (!moved)
            {
                 ofile << "G1 X" << pos.x() - 0.1 << " Y" << pos.y()       << " S1.0" << endl;
                 ofile << "G1 X" << pos.x() + 0.1 << " Y" << pos.y()       << " S1.0" << endl;
                 ofile << "G1 X" << pos.x()       << " Y" << pos.y()       << " S1.0" << endl;
                 ofile << "G1 X" << pos.x() - 0.1 << " Y" << pos.y() - 0.1 << " S1.0" << endl;
                 ofile << "G1 X" << pos.x() + 0.1 << " Y" << pos.y() + 0.1 << " S1.0" << endl;
                 ofile << "G1 X" << pos.x()       << " Y" << pos.y()       << " S1.0" << endl;
            }
            on = false;
            mesh.setColor(RGBColor::Red());
        }

        if (r.command == "PD") {
            on = true;
            mesh.setColor(RGBColor::Green());
            moved = false;
        }

    }

    mesh.dumpPLY(input + ".ply");

    ofile << "M5          ; Switch tool offEnd" << endl;
    ofile << "M400" << endl;
    ofile << "G0 X0 Y0" << endl;
    ofile << "M221   S0" << endl;
    ofile.close();
    return 0;
}

int annotate (CommandLineSetter &s)
{
    SYNC_PRINT(("GCode annotate called\n"));
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    LibpngFileSaver::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif

    std::string input  = s.getString("input" , "input.gcode");
    std::string output = s.getString("output", "out.png");

    GCodeProgram program;
    GcodeLoader loader;

    ifstream file;
    file.open(input, ios::in);
    if (file.fail())
    {
        SYNC_PRINT(("g code playground::load(): Can't open gcode file <%s>\n", input.c_str()));
        return false;
    }

    loader.loadGcode(file, program);

    std::string basename = FolderScanner::getBaseName(input);
    double scale= 0.25;
    int header = 40;
    RGB24Buffer *canvas = new RGB24Buffer(1500 * scale + header, 6000 * scale, RGBColor::White());
    canvas->drawRectangle(0, header, canvas->w - 1, canvas->w - 1 - header, RGBColor::Gray());
    AbstractPainter<RGB24Buffer> p(canvas);
    p.drawFormatU8(4, 4, RGBColor::Black(), 4, "Left: %s", basename.c_str());
    p.drawFormatU8(4 + canvas->w/2, 4, RGBColor::Black(), 4, "Right: %s", basename.c_str());

/*    for (int c = 0; c < basename.length(); c++)
    {
        printf("%02X\n",(int)basename[c]);
    }*/

    SvgFile svgFile;

    LabelGcodeInterpreter interperter;
    interperter.canvas = canvas;
    interperter.scale  = scale;

    interperter.executeProgram(program);

    BufferFactory::getInstance()->saveRGB24Bitmap(canvas, output);

    RGB24Buffer *canvas1 = canvas->createViewPtr<RGB24Buffer>(0,           0, canvas->h, canvas->w/2);
    RGB24Buffer *canvas2 = canvas->createViewPtr<RGB24Buffer>(0, canvas->w/2, canvas->h, canvas->w/2);

    BufferFactory::getInstance()->saveRGB24Bitmap(canvas1, std::string("1_") + output);
    BufferFactory::getInstance()->saveRGB24Bitmap(canvas2, std::string("2_") + output);

    ofstream outfile;
    outfile.open("02.re.gcode", std::ostream::out);
    loader.saveGcode(outfile, program);
    outfile.close();
    return 0;
}

void usage(void) {
      cout << "Usage:\n"
           << "\thelp\n"
           << "\tcompensate [-i|--input filename] [-o|--output filename] [-f|--offset offset] [-t|--touchZ touchZ]\n"
           << "\trender [-i|--input filename] [-o|--output filename] [-v|--vinyl]\n"
           << "\tannotate [-i|--input filename] [-o|--output filename]\n";

}

int main(int argc, char **argv)
{
  CommandLineSetter s(argc, argv);

  vector<string> nonPrefix = s.nonPrefix();

  if (nonPrefix.size() <= 1) {
     SYNC_PRINT(("Provided %d argumets\n", nonPrefix.size()));
     usage();
     return 0;
  }

  bool vinyl = false;
  double offset = 0.5;
  double touchZ = -0.1;
  std::string infile = "in.gcode";
  std::string outfile;

  std::string mode = nonPrefix[1];
  SYNC_PRINT(("Mode :%s\n", mode.c_str()));

  if (mode == "help" || s.hasOption("help"))
  {
      usage();
      return  0;
  }

  if (mode == "annotate") {
    annotate(s);

    return  0;
  }

  if (mode == "compensate") {
    outfile = "out.gcode";
  } else if (mode == "render") {
    outfile = "out.ply";
  } else {
    usage();
    return -1;
  }

  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];

    if (arg == "-i" || arg == "--input") {

      // You will see this piece of code a couple more times.
      // I hate macros with a burning passion and this test util doesn't deserve
      // it's own parser class
      if (i++ == argc) {
        usage();
        return -1;
      }
      infile = argv[i];

    } else if (arg == "-o" || arg == "--output") {

      if (i++ == argc) {
        usage();
        return -1;
      }
      outfile = argv[i];

    } else if (arg == "-t" || arg == "--touchZ") {
      
      if (i++ == argc) {
        usage();
        return -1;
      }
      touchZ = stod(argv[i]);

    } else if (arg == "-f" || arg == "--offset") {

      if (i++ == argc) {
        usage();
        return -1;
      }
      offset = stod(argv[i]);

    } else if (arg == "-v" || arg == "--vinyl") {

      vinyl = true;

    }
  }

  if (mode == "compensate") {
    GCodeCompensator compensator;
    GCodeProgram program;
    GcodeLoader loader;
    ifstream ifile;
    ofstream ofile;

    ifile.open(infile, ios::in);
    if (ifile.fail())
    {
      SYNC_PRINT(("Can't open input gcode file <%s>\n", infile.c_str()));
        return 1;
    }

    ofile.open(outfile, ios::out);
    if (ofile.fail())
    {
      SYNC_PRINT(("Can't open output gcode file <%s>\n", outfile.c_str()));
        return 1;
    }

    loader.loadGcode(ifile, program);
    compensator.compensateDragKnife(program, offset, touchZ);
    loader.saveGcode(ofile, compensator.result);

    ofile.close();

  }

  if (mode == "render") {
    GCodeProgram program;
    GcodeLoader loader;
    GCodeToMesh renderer;
    Mesh3D mesh;
    ifstream file;

    file.open(infile, ios::in);
    if (file.fail())
    {
      SYNC_PRINT(("Can't open input gcode file <%s>\n", infile.c_str()));
        return 1;
    }

    loader.loadGcode(file, program);

    if (vinyl) {
      renderer.renderToMesh(program, mesh, offset);
    } else {
      renderer.renderToMesh(program, mesh);
    }

    if (mesh.dumpPLY(outfile)) {
        SYNC_PRINT(("Can't open output mesh file <%s>\n", outfile.c_str()));
        return 1;
    }
  }

  return 0;
}
