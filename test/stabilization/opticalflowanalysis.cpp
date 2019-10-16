#include "opticalflowanalysis.h"

#include "core/math/vector/vector2d.h"
#include "core/buffers/flow/flowVector.h"

#include <vector>

using namespace corecvs;

OpticalFlowAnalysis::OpticalFlowAnalysis(QObject *parent) : QObject(parent)
{

}

void OpticalFlowAnalysis::analyzeFlow()
{
    FlowBuffer *flow = this->opticalFlowProcessor->getFlow();
    if (flow == NULL) {
        return;
    }

    Vector2dd commonDiff(0.0, 0.0);

    std::vector<FloatFlowVector> *vectors = flow->toVectorForm();
    int size = vectors->size();
    for (int i = 0; i < size; i++) {
        FloatFlowVector curVector = vectors->at(i);
        Vector2dd diff = curVector.end - curVector.start;
        commonDiff += diff;
    }
    commonDiff = commonDiff / size;
//    SYNC_PRINT(("Amount points: %d, common diff vector: (%f, %f) \n", size, commonDiff.x(), commonDiff.y()));
    double xDiff = commonDiff.x();
    double yDiff = commonDiff.y();
    if (xDiff < -5) {
        SYNC_PRINT(("move Left\n"));
    } else if (xDiff > 5) {
        SYNC_PRINT(("move right\n"));
    }
    if (yDiff < -5) {
        SYNC_PRINT(("move up\n"));
    } else if (yDiff > 5) {
        SYNC_PRINT(("move down\n"));
    }

}
