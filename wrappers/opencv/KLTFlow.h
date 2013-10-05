/**
 * \file KLTFlow.h
 * \brief Add Comment Here
 *
 * \date Feb 23, 2011
 * \author alexander
 */

#ifndef KLTFLOW_H_
#define KLTFLOW_H_

#include <vector>

#include "flowBuffer.h"
#include "g12Buffer.h"
#include "flowVector.h"

using corecvs::FloatFlowVector;
using corecvs::G12Buffer;
using std::vector;

class KLTFlow
{
public:
    KLTFlow();
    virtual ~KLTFlow();

    static vector<FloatFlowVector> *getOpenCVKLT(
            G12Buffer *first,
            G12Buffer *second,
            double selectorQuality,
            double selectorDistance,
            int selectorSize,
            int useHarris,
            double harrisK,
            int kltSize);
};

#endif /* KLTFLOW_H_ */
