#ifndef _SGM_H_
#define _SGM_H_
/**
 * \file sgm.h
 * \brief This is a header file for the sgm.c
 *
 *
 * \ingroup corefiles
 * \date Apr 14, 2009
 * \author Alexander Pimenov
 */
#ifdef __cplusplus
    extern "C" {
#endif

#include "g12buffer.h"

#ifdef DEPRICATED
#include "disparity.h"
namespace corecvs {

#endif

#define PATH_LIMIT 12

/**
 * A list of defines that classify the path.
 *
 * \@{
 **/
#define PATH_TOP_BIT             (0)
#define PATH_TOP_RIGHT_BIT         (1)
#define PATH_RIGHT_BIT            (2)
#define PATH_BOTTOM_RIGHT_BIT     (3)
#define PATH_BOTTOM_BIT             (4)
#define PATH_BOTTOM_LEFT_BIT     (5)
#define PATH_LEFT_BIT            (6)
#define PATH_TOP_LEFT_BIT        (7)

#define PATH_TOP             (0x1 << PATH_TOP_BIT)
#define PATH_TOP_RIGHT         (0x1 << PATH_TOP_RIGHT_BIT)
#define PATH_RIGHT            (0x1 << PATH_RIGHT_BIT)
#define PATH_BOTTOM_RIGHT     (0x1 << PATH_BOTTOM_RIGHT_BIT)
#define PATH_BOTTOM             (0x1 << PATH_BOTTOM_BIT)
#define PATH_BOTTOM_LEFT     (0x1 << PATH_BOTTOM_LEFT_BIT)
#define PATH_LEFT            (0x1 << PATH_LEFT_BIT)
#define PATH_TOP_LEFT        (0x1 << PATH_TOP_LEFT_BIT)
/*\@}*/


typedef void  (*SGM_Progress_Callback)(int pathNum, int persent);

G12Buffer* sgm_PC16 (G12Buffer* I, G12Buffer* J, int dMax, int closePenlty, int longPenalty, uint32_t **sArrayTest, uint32_t **cArrayTest, uint32_t **pArrayTest, uint32_t mask, SGM_Progress_Callback callback);
G12Buffer* sgmFastx86 (G12Buffer* left, G12Buffer* right, int dMax, int closePenlty, int longPenalty);
G12Buffer* sgmFastx86v1 (G12Buffer* left, G12Buffer* right, int dMax, int closePenlty, int longPenalty);

#ifdef DEPRICATED
DisparityBuffer *countDisparity( G12Buffer* I, G12Buffer* J, int dMax, int closePenlty, int longPenalty );
#endif

#ifdef __cplusplus
    } //     extern "C"
#endif
} //namespace corecvs
#endif

