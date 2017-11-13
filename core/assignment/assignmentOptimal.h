#ifndef ASSIGNMENTOPTIMAL_H_
#define ASSIGNMENTOPTIMAL_H_
/**
 * \file assignmentOptimal.h
 * \brief a header for assignmentOptimal.c
 *
 * \ingroup cppcorefiles
 * \date Oct 17, 2010
 * \author alexander
 */
#include <stdint.h>

#include "core/utils/global.h"

#include "core/buffers/abstractBuffer.h"
namespace corecvs {


void assignOptimal(AbstractBuffer<int, int> *input, AbstractBuffer<int, int> *output);
void assignOptimal(AbstractBuffer<uint16_t, int> *input, AbstractBuffer<int, int> *output);
void assignOptimal(AbstractBuffer<double, int> *input, AbstractBuffer<int, int> *output);
void assignOptimal(AbstractBuffer<float, int> *input, AbstractBuffer<int, int> *output);




} //namespace corecvs
#endif  //ASSIGNMENTOPTIMAL_H_

