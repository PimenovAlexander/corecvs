#ifndef LEVMAR_H_
#define LEVMAR_H_
/**
 * \file levmar.h
 *
 * \brief This file is a header for the levmar.c
 *
 *
 * \ingroup corefiles
 * \date May 22, 2009
 * \author Alexander Pimenov
 *
 */
#ifdef __cplusplus
    extern "C" {

#endif

/**
 * A functor to describe a function to be modeled.
 */
typedef double   (*LMfunction)(int num, double input[]);

/**
 * A functor to describe a derivative of the function to be modeled.
 */
typedef double   (*LMJ)(int num, int component, double input[]);
/**
 * A functor to describe a result of the measurement.
 */
typedef double (*LMY)(int num);

double doLMFit(int num, int dimention, LMfunction ff, LMY yf, LMJ Jf, double startGuess[], double *lastGuess, int iterations);

#ifdef __cplusplus
    } //     extern "C"
#endif
#endif /* LEVMAR_H_ */

