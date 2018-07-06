#include "pltLoader.h"
#include "core/utils/utils.h"

#include <sstream>

namespace corecvs {

using namespace std;

/* Rewrite this with regex */
HPGLProgram::Record HPGLLoader::parseLine(const string& gline)
{
    std::string line = gline;

    cout << "parseLine("<< line << "):called" << endl;

    HPGLProgram::Record result;

    result.command = line.substr(0, 2);

    if (line.back() == ';') line = line.substr(0, line.length() - 1);

    vector<string> split = HelperUtils::stringSplit(line.substr(2), ',');


    for (string s: split)
    {
        if (!s.empty()) {
            std::locale mylocale("C");
            std::istringstream ss(s);
            ss.imbue(mylocale);
            double v;
            ss >> v;
            result.params.push_back(v);
        }
    }

    return result;
}

HPGLLoader::HPGLLoader()
{}


int HPGLLoader::loadHPGLcode(std::istream &input, HPGLProgram &program)
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

        HPGLProgram::Record r = parseLine(line);

        program.program.push_back(r);
    }
    return 0;
}

} // namespace corecvs
