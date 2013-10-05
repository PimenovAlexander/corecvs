/**
 * \file flowVectorInformation.h
 * \brief This header holds the data structures that hold classification of flow vectors.
 *
 * \date Feb 18, 2011
 * \author alexander
 */

#ifndef FLOWVECTORINFORMATION_H_
#define FLOWVECTORINFORMATION_H_
#include "floatFlowBuffer.h"
#include "FCostFunction.h"
#include "vector3d.h"
namespace corecvs {

class FlowVectorInformation
{
public:
    enum FilteredType {
        NOT_FILTERED = 0,
        FLOW_TOO_SHORT,
        FLOW_V_TOO_SHORT,
        FLOW_FU_TOO_LARGE,
        FLOW_FU_TOO_SUBJECT_TO_NOISE,
        OBJECT_INSIDE_CORRIDOR,
        OBJECT_NEGATIVE,
        OBJECT_UNDERGROUND
    };

    static const char *REASON_NAMES[];

    FloatFlowVector vector;
    double Fu;
    double minFu;
    double maxFu;
    FilteredType isFiltered;
    bool isSelected;
    bool isAlien;
    FCostFunction::CostFunctionType type;
    Vector3dd pos;


    FlowVectorInformation(
        FloatFlowVector _vector,
        double _Fu,
        bool _isAlien
    ) :
        vector(_vector),
        Fu(_Fu),
        isFiltered(NOT_FILTERED),
        isSelected(false),
        isAlien(_isAlien),
        type(FCostFunction::AUTO),
        pos(0.0)
    {};
};


} //namespace corecvs
#endif /* FLOWVECTORINFORMATION_H_ */

