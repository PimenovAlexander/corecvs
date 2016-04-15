#ifndef CBLASLAPACKEWRAPPER
#ifdef WITH_BLAS
#ifdef WITH_MKL
    #include <mkl.h>
#elif defined(WITH_BLAS)
    #include <complex>
    #define lapack_complex_float std::complex<float>
    #define lapack_complex_double std::complex<double>
    #include <cblas.h>
    #include <lapacke.h>
#endif
#endif
#endif
