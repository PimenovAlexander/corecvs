#ifndef HPGL_LOADER_H
#define HPGL_LOADER_H

#include <string>
#include <vector>
#include <iostream>

namespace corecvs {

class HPGLProgram
{
public:
    struct Record {
        std::string command;
        std::vector<double> params;
    };

    std::vector<Record>  program;

    friend std::ostream& operator << (std::ostream &out, Record &toSave)
    {
        out << toSave.command;
        for (size_t i=0; i < toSave.params.size(); i++)
        {
            out << toSave.params[i] << ((i != toSave.params.size() - 1) ? "," : "");
        }
        out << ";" << std::endl;
        return out;
    }

    friend std::ostream& operator << (std::ostream &out, HPGLProgram &program)
    {
        for (size_t i=0; i < program.program.size(); i++)
        {
            out << program.program[i];
        }
        return out;
    }
};


class HPGLLoader
{
public:
    bool trace = true;


    HPGLLoader();
    int loadHPGLcode(std::istream &input, HPGLProgram &program);
private:
    HPGLProgram::Record parseLine(const std::string &gline);
};

} // namespace corecvs

#endif // HPGL_LOADER_H
