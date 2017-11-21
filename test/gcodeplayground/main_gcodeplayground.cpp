#include <fstream>

#include "core/fileformats/gcodeLoader.h"

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



int main (int argc, char **argv)
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
