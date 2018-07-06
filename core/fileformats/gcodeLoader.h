#ifndef GCODELOADER_H
#define GCODELOADER_H

#include <vector>
#include <iostream>
#include "core/xml/generated/drawGCodeParameters.h"


#include "core/utils/global.h"

#include "core/geometry/mesh3d.h"
#include "core/geometry/polygons.h"

#include "core/xml/generated/gCodeColoringSheme.h"

namespace corecvs {

class GCodeProgram
{
public:
    struct Record {
        char address;
        double value;
    };

    struct Code {
        std::string comment; /**< Actually should use union or common parent class. Too lazy for this*/
        char area = ' ';
        int number = 0;
        std::vector<Record> parameters;

        /*bool isComment() const {
            return !comment.empty();
        }*/

        bool hasParameter(char p) const {
            for (Record r : parameters)
                if (r.address == p)
                    return true;
            return false;
        }

        double getParameter(char p) const {
            for (Record r : parameters)
                if (r.address == p)
                    return r.value;
            return 0;
        }

        void setParameter(char p, double value)
        {
            for (Record &r : parameters)
                if (r.address == p)
                {
                    r.value = value;
                    return;
                }
            parameters.push_back({p, value});
        }
    };

    std::vector<Code> program;

    /* Common processors */

};

class GCodeInterpreter {
public:
    struct MachineState {
        Vector3dd position = Vector3dd::Zero();
        // Vector3dd angles

        /* 3D Printer related stuff need to be brought to child class */
        double feedRate = 100;
        double bedTemperature = 0;
        double extruderTemperature = 0;

        bool extruderRealtive = true;
        double extruderPos = 0;

        virtual ~MachineState() {}
    };

    MachineState* state = NULL;
    bool traceComment = false;
    bool traceProcess = false;

    virtual MachineState* createState(void);

    virtual void executeProgram(const GCodeProgram &program);

    /* These hooks could either return true - then implemenation update the machine state */
    virtual bool gcodeHook    ( const GCodeProgram::Code &code );

    virtual bool straightHook (int type, const MachineState &before, const MachineState &after) ;
    virtual bool arkHook      (const MachineState &before, const MachineState &after,
                               const PlaneFrame &frame, double maxArg);


    virtual void errorHook( void );

    virtual ~GCodeInterpreter();
};

class GCodeToMesh {
public:
    bool trace = true;
    int arcSteps = 10;


//    GCodeColoringSheme::GCodeColoringSheme coloring = GCodeColoringSheme::COLOR_FROM_SPEED;
    DrawGCodeParameters mParameters;

    int renderToMeshOld(const GCodeProgram &in, Mesh3D &mesh);

    /* New style controllable converter */
    class MeshInterpreter : public GCodeInterpreter {
    public:
        GCodeToMesh *parent = NULL;
        Mesh3D *mesh = NULL;


        MeshInterpreter(GCodeToMesh *parent, Mesh3D *mesh) :
            parent(parent),
            mesh(mesh)
        {}

        // GCodeInterpreter interface
    public:
        virtual bool straightHook(int type, const MachineState &before, const MachineState &after) override;
        virtual bool arkHook(const MachineState &before, const MachineState &after, const PlaneFrame &frame, double maxArg) override;
    };

    //void setColor(Mesh3D &mesh, const MachineState &before, const MachineState &after);
    int renderToMesh(const GCodeProgram &in, Mesh3D &mesh);


};


class GcodeLoader
{
public:
    GcodeLoader();
    bool trace = false;


    int loadGcode(istream &input, Mesh3D &mesh);

    int loadGcode(istream &input, GCodeProgram &program);
    int saveGcode(ostream &out, const GCodeProgram &program);

    virtual ~GcodeLoader();

public:
    static vector<std::pair<char, double> > parseLine(const string &gline);
};

} // namespace corecvs

#endif // GCODELOADER_H
