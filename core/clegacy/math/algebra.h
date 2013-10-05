#ifndef ALGEBRA_H_
#define ALGEBRA_H_
#include <stdint.h>
namespace corecvs {
/**
 * \file algebra.h
 * \brief Header for algebra.c
 *
 * \ingroup corefiles
 * \date Apr 18, 2009
 * \author Alexander Pimenov
 */
#ifdef __cplusplus
    extern "C" {

#endif


typedef uint16_t fixed8in16;
typedef uint32_t fixed16in32;


uint8_t getDirection5bit(int16_t dx, int16_t dy);
uint8_t getMagnitude7bit(int16_t dx, int16_t dy);


double getLengthStable (double a, double b);

#ifdef __cplusplus
    } //     extern "C"
#endif
} //namespace corecvs
#endif /* ALGEBRA_H_ */

