#ifndef CORNERMARKER_H_
#define CORNERMARKER_H_
/**
 *  \file cornermarker.h
 *  \brief a header file for cornermarker.c
 *
 *  \ingroup corefiles
 *  \date Jun 3, 2009
 *  \author Alexander Pimenov
 */
#ifdef __cplusplus
    extern "C" {
#endif
#include "g12buffer.h"
namespace corecvs {

G12Buffer* findCornerPoints(G12Buffer *input);


#ifdef __cplusplus
    } //     extern "C"

#endif
} //namespace corecvs
#endif /* CORNERMARKER_H_ */

