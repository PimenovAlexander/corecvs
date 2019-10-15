target_sources(corecvs
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/eulerAngles.h
        ${CMAKE_CURRENT_LIST_DIR}/matrix/matrix.h
        ${CMAKE_CURRENT_LIST_DIR}/matrix/matrix22.h
        ${CMAKE_CURRENT_LIST_DIR}/matrix/matrix33.h
        ${CMAKE_CURRENT_LIST_DIR}/matrix/matrix44.h
        ${CMAKE_CURRENT_LIST_DIR}/matrix/diagonalMatrix.h
        ${CMAKE_CURRENT_LIST_DIR}/matrix/blasReplacement.h
        ${CMAKE_CURRENT_LIST_DIR}/sparseMatrix.h
        ${CMAKE_CURRENT_LIST_DIR}/matrix/homographyReconstructor.h
        ${CMAKE_CURRENT_LIST_DIR}/matrix/matrixOperations.h
        ${CMAKE_CURRENT_LIST_DIR}/vector/vectorOperations.h
        ${CMAKE_CURRENT_LIST_DIR}/vector/fixedArray.h
        ${CMAKE_CURRENT_LIST_DIR}/vector/fixedVector.h
        ${CMAKE_CURRENT_LIST_DIR}/vector/vector2d.h
        ${CMAKE_CURRENT_LIST_DIR}/vector/vector3d.h
        ${CMAKE_CURRENT_LIST_DIR}/vector/vector4d.h
        ${CMAKE_CURRENT_LIST_DIR}/vector/vector.h
        ${CMAKE_CURRENT_LIST_DIR}/fixed/fixedPoint24p8.h
        ${CMAKE_CURRENT_LIST_DIR}/lutAlgebra.h
        ${CMAKE_CURRENT_LIST_DIR}/projectiveTransform.h
        ${CMAKE_CURRENT_LIST_DIR}/quaternion.h
        ${CMAKE_CURRENT_LIST_DIR}/affine.h
        ${CMAKE_CURRENT_LIST_DIR}/levenmarq.h
        ${CMAKE_CURRENT_LIST_DIR}/gradientDescent.h
        ${CMAKE_CURRENT_LIST_DIR}/helperFunctions.h
        ${CMAKE_CURRENT_LIST_DIR}/generic/genericMath.h

        ${CMAKE_CURRENT_LIST_DIR}/sse/sseWrapper.h
        ${CMAKE_CURRENT_LIST_DIR}/sse/sseInteger.h
        ${CMAKE_CURRENT_LIST_DIR}/sse/int64x2.h
        ${CMAKE_CURRENT_LIST_DIR}/sse/int32x4.h
        ${CMAKE_CURRENT_LIST_DIR}/sse/int32x8v.h
        ${CMAKE_CURRENT_LIST_DIR}/sse/float32x4.h
        ${CMAKE_CURRENT_LIST_DIR}/sse/float32x8.h
        ${CMAKE_CURRENT_LIST_DIR}/sse/floatT8.h
        ${CMAKE_CURRENT_LIST_DIR}/sse/sseMath.h

        ${CMAKE_CURRENT_LIST_DIR}/sse/intBase16x8.h
        ${CMAKE_CURRENT_LIST_DIR}/sse/int16x8.h
        ${CMAKE_CURRENT_LIST_DIR}/sse/uInt16x8.h

        ${CMAKE_CURRENT_LIST_DIR}/sse/intBase8x16.h
        ${CMAKE_CURRENT_LIST_DIR}/sse/int8x16.h
        ${CMAKE_CURRENT_LIST_DIR}/sse/uInt8x16.h
        ${CMAKE_CURRENT_LIST_DIR}/sse/doublex2.h
        ${CMAKE_CURRENT_LIST_DIR}/sse/doublex4.h
        ${CMAKE_CURRENT_LIST_DIR}/sse/doublex8.h

        ${CMAKE_CURRENT_LIST_DIR}/avx/avxInteger.h
        ${CMAKE_CURRENT_LIST_DIR}/avx/int16x16.h
        ${CMAKE_CURRENT_LIST_DIR}/avx/int32x16v.h
        ${CMAKE_CURRENT_LIST_DIR}/avx/int32x8.h
        ${CMAKE_CURRENT_LIST_DIR}/avx/int64x4.h

        ${CMAKE_CURRENT_LIST_DIR}/mathUtils.h
        ${CMAKE_CURRENT_LIST_DIR}/eulerAngles.h
        ${CMAKE_CURRENT_LIST_DIR}/puzzleBlock.h
        ${CMAKE_CURRENT_LIST_DIR}/matrix/similarityReconstructor.h
        ${CMAKE_CURRENT_LIST_DIR}/sse/doublexT4.h
    #    ${CMAKE_CURRENT_LIST_DIR}/extensiveCoding.h
        ${CMAKE_CURRENT_LIST_DIR}/wisdom.h
        ${CMAKE_CURRENT_LIST_DIR}/rotate.h

    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/matrix/matrix.cpp
        ${CMAKE_CURRENT_LIST_DIR}/matrix/matrix22.cpp
        ${CMAKE_CURRENT_LIST_DIR}/matrix/matrix33.cpp
        ${CMAKE_CURRENT_LIST_DIR}/matrix/matrix44.cpp
        ${CMAKE_CURRENT_LIST_DIR}/matrix/diagonalMatrix.cpp
        ${CMAKE_CURRENT_LIST_DIR}/sparseMatrix.cpp
        ${CMAKE_CURRENT_LIST_DIR}/matrix/homographyReconstructor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/vector/vector2d.cpp
        ${CMAKE_CURRENT_LIST_DIR}/lutAlgebra.cpp
        ${CMAKE_CURRENT_LIST_DIR}/affine.cpp
        ${CMAKE_CURRENT_LIST_DIR}/projectiveTransform.cpp
        ${CMAKE_CURRENT_LIST_DIR}/quaternion.cpp
        ${CMAKE_CURRENT_LIST_DIR}/gradientDescent.cpp
        ${CMAKE_CURRENT_LIST_DIR}/helperFunctions.cpp
        ${CMAKE_CURRENT_LIST_DIR}/generic/genericMath.cpp
        ${CMAKE_CURRENT_LIST_DIR}/sse/sseWrapper.cpp
        ${CMAKE_CURRENT_LIST_DIR}/matrix/similarityReconstructor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/wisdom.cpp
        ${CMAKE_CURRENT_LIST_DIR}/rotate.cpp
)

#contains(DEFINES, "WITH_FFTW") {
#    !build_pass: message(Adding core submodule math : fftw wrapper)
#    ${CMAKE_CURRENT_LIST_DIR}/fftw/fftwWrapper.h
#    ${CMAKE_CURRENT_LIST_DIR}/fftw/fftwWrapper.cpp
#}
