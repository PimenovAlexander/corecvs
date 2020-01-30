#ifndef SIMULATIONWORLD_H
#define SIMULATIONWORLD_H

#include <string>
#include <core/geometry/mesh/mesh3DDecorated.h>
#include <core/math/mathUtils.h>


class UserTransformBase {
public:
    virtual corecvs::Affine3DQ transformAffine()
    {
        return corecvs::Affine3DQ::Identity();
    }

    virtual corecvs::Matrix44 transformMatrix()
    {
        return corecvs::Matrix44::Identity();
    }

    virtual ~UserTransformBase() {};
};


class UserTransformAffine3DQ : public UserTransformBase {
public:
    corecvs::Affine3DQ transform;

    virtual corecvs::Affine3DQ transformAffine() override
    {
        return transform;
    }

    virtual corecvs::Matrix44 transformMatrix() override
    {
        return (corecvs::Matrix44)transform;
    }

    virtual ~UserTransformAffine3DQ(){};
};

class UserTransformScale : public UserTransformBase {
public:
    double scaler;

    virtual corecvs::Affine3DQ transformAffine() override
    {
        return corecvs::Affine3DQ::Identity();
    }

    virtual corecvs::Matrix44 transformMatrix() override
    {
        return corecvs::Matrix44(scaler);
    }

    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(scaler, 1.0, "scaler");
    }

    virtual ~UserTransformScale(){};
};

class UserTransformShift : public UserTransformBase {
public:
    corecvs::Vector3dd shifter;

    virtual corecvs::Affine3DQ transformAffine() override
    {
        return corecvs::Affine3DQ::Shift(shifter);
    }

    virtual corecvs::Matrix44 transformMatrix() override
    {
        return corecvs::Matrix44::Shift(shifter);
    }

    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(shifter, corecvs::Vector3dd::Zero(), "shifter");
    }

    virtual ~UserTransformShift(){};
};

class UserTransformMainAxis : public UserTransformBase {
public:
    enum {
        X = 0,
        Y = 1,
        Z = 2
    } axis;

    double angle = 0.0;

    virtual corecvs::Affine3DQ transformAffine() override
    {
        switch (axis) {
            case 0: return corecvs::Affine3DQ::RotationX(angle);
            case 1: return corecvs::Affine3DQ::RotationY(angle);
            case 2: return corecvs::Affine3DQ::RotationZ(angle);
        }
        return corecvs::Affine3DQ::Identity();
    }

    virtual corecvs::Matrix44 transformMatrix() override
    {
        return (corecvs::Matrix44) transformAffine();
    }

    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit((int &)axis, 0, "axis" );

        double degAngle = corecvs::radToDeg(angle);
        visitor.visit(degAngle, 0.0, "angle");
        angle = corecvs::degToRad(degAngle);
    }

    virtual ~UserTransformMainAxis(){};
};

class UserTransformMainRotation : public UserTransformBase {
public:
    corecvs::Vector3dd axis = corecvs::Vector3dd::Zero();
    double angle = 0.0;

    virtual corecvs::Affine3DQ transformAffine() override
    {
        return corecvs::Affine3DQ(corecvs::Quaternion::Rotation(axis, angle));
    }

    virtual corecvs::Matrix44 transformMatrix() override
    {
        return (corecvs::Matrix44) transformAffine();
    }

    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(axis, "axis");

        double degAngle = corecvs::radToDeg(angle);
        visitor.visit(degAngle, 0.0, "angle");
        angle = corecvs::degToRad(degAngle);
    }

    virtual ~UserTransformMainRotation(){};
};



class SimulationTransform
{
public:
    corecvs::Matrix44 transform;

    SimulationTransform() :
        transform(corecvs::Matrix44::Identity())
    {
    }

    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        if (visitor.isLoader())
        {
            std::string type;
            visitor.visit(type, std::string("affine3d"), "type");

            if (type == "affine3d")
            {
                transform.accept(visitor);
                return;
            }

            if (type == "scale") {
                SYNC_PRINT(("Loading scaler\n"));
                UserTransformScale s;
                s.accept(visitor);
                transform = s.transformMatrix();
            }

            if (type == "shift") {
                SYNC_PRINT(("Loading shift\n"));
                UserTransformShift s;
                s.accept(visitor);
                transform = s.transformMatrix();
            }

            if (type == "axisr") {
                SYNC_PRINT(("Loading axisr\n"));
                UserTransformMainAxis s;
                s.accept(visitor);
                transform = s.transformMatrix();
            }

            if (type == "rotate") {
                SYNC_PRINT(("Loading rotation\n"));
                UserTransformMainRotation s;
                s.accept(visitor);
                transform = s.transformMatrix();
            }

        }
    }
};

class SimulationMesh
{
public:
    std::string meshName;
    bool show;

    corecvs::Mesh3DDecorated mesh;
    corecvs::Matrix44 transform;

    SimulationMesh() :
        transform(corecvs::Matrix44::Identity())
    {

    }

    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(meshName, std::string(""), "mesh");
        visitor.visit(show, true, "show");

        if (visitor.isLoader())
        {
            int transformSize = 0;
            visitor.visit(transformSize, 0, "transform.size");

            SimulationTransform tstack;
            for (size_t i = 0; i < (size_t)transformSize; i++)
            {
                char buffer[100];
                snprintf2buf(buffer, "transform[%d]", i);
                visitor.visit(tstack, buffer);
                transform = tstack.transform * transform;
            }

        }
        if (visitor.isSaver())
        {
            int transformSize = 1;
            visitor.visit(transformSize, 0, "transform.size");
            visitor.visit(transform, "transform[0]");
        }

    }

};

class SimulationWorld
{
public:
    SimulationWorld();

    void load(const std::string &filename);

    std::string name;
    std::vector<SimulationMesh> meshes;


    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(name       , std::string(""),      "world");

        int meshesSize = (int)meshes.size();
        visitor.visit(meshesSize, 0, "meshes.size");
        meshes.resize(meshesSize);

        for (size_t i = 0; i < (size_t)meshesSize; i++)
        {
            char buffer[100];
            snprintf2buf(buffer, "meshes[%d]", i);
            visitor.visit(meshes[i], buffer);
        }
    }

};

#endif // SIMULATIONWORLD_H
