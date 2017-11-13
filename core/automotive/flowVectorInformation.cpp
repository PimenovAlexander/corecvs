/**
 * \file flowVectorInformation.cpp
 * \brief This file holds the data structures that help classification of flow vectors.
 *
 * \date Feb 18, 2011
 * \author alexander
 */

#include "core/automotive/flowVectorInformation.h"
namespace corecvs {

const char *FlowVectorInformation::REASON_NAMES[] =
{
   "NOT_FILTERED",
   "FLOW_TOO_SHORT",
   "FLOW_V_TOO_SHORT",
   "FLOW_FU_TOO_LARGE",
   "FLOW_FU_TOO_SUBJECT_TO_NOISE",
   "OBJECT_INSIDE_CORRIDOR",
   "OBJECT_NEGATIVE",
   "OBJECT_UNDERGROUND"
};

} //namespace corecvs

