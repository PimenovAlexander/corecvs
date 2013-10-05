#ifndef FASTKERNELDOC_H_
#define FASTKERNELDOC_H_
/**
 * \file fastKernelDoc.h
 * \brief a documentation for fastkernels.
 *
 * \date May 7, 2011
 * \author alexander
 *
 * <h1>FastKernels introduction</h1>
 *
 * This file describes the nature of fastKernels initiative and the
 * ways it should apply to CVS core.
 *
 * FastKernels is the attempt to abstract the semantics of computation in image processing form the
 * the actual implementation.
 *   Both - the types that will hold the operands and the computation primitives
 *   that on which it will all execute.
 *
 * For example single code
 *  \f[
 *       \frac { F(x+1,y)-F(x-1,y) } {2}
 *  \f]
 *
 * could be used for integers and doubles. Moreover, automatic vectorization for
 * number of platforms can be done.
 *
 * Most part of the functionality above can be implemented using C++ templates.
 * But writing own meta-language can allow to use the same code for
 *  <ul>
 *   <li>FPGA</li>
 *   <li>DSP</li>
 *   <li>OpenCL</li>
 *   <li>HASCOL</li>
 *   <li>VHDL</li>
 *  </ul>
 *
 *
 * Still with C++ template programming following features can be reached.
 *  <ul>
 *   <li>Seamless work with different types</li>
 *   <li>Work with SSE2</li>
 *   <li>For SSE it is possible to automatically support streaming</li>
 *  </ul>
 *
 *
 * <h1>FastKernels notions</h1>
 *
 * FastKernels work with input and output buffers that are expected to be rectangular
 * Generally we don't want to limit number of inputs and outputs, but as you will
 * see, C++ templates allow then to work only with
 *
 *
 * <h1>Wrappers around SSE</h1>
 *  Wrappers around SSE allow to use SSE vectors as if they are real types. For details
 *  please have a look at SSEInteger, Float32x4, Int16x8, Int8x16, Int32x8, Int64x2
 *
 */

namespace corecvs {
} //namespace corecvs
#endif  //FASTKERNELDOC_H_

