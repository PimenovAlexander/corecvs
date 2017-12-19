HEADERS += \
    math/eulerAngles.h \
    math/matrix/matrix.h \
    math/matrix/matrix22.h \
    math/matrix/matrix33.h \
    math/matrix/matrix44.h \
    math/matrix/diagonalMatrix.h \
    math/matrix/blasReplacement.h \
    math/sparseMatrix.h \
    math/matrix/homographyReconstructor.h \
    math/matrix/matrixOperations.h \
    math/vector/vectorOperations.h \
    math/vector/fixedArray.h \
    math/vector/fixedVector.h \
    math/vector/vector2d.h \
    math/vector/vector3d.h \
    math/vector/vector4d.h \
    math/vector/vector.h \
    math/fixed/fixedPoint24p8.h \
    math/lutAlgebra.h \
    math/projectiveTransform.h \
    math/quaternion.h \
    math/affine.h \
    math/levenmarq.h \    
    math/gradientDescent.h \
    math/helperFunctions.h \
    math/generic/genericMath.h \
    \
    math/sse/sseWrapper.h \
    math/sse/sseInteger.h \
    math/sse/int64x2.h \
    math/sse/int32x4.h \
    math/sse/int32x8v.h \
    math/sse/float32x4.h \
    math/sse/float32x8.h \
    math/sse/floatT8.h \
    math/sse/sseMath.h \
    \
    math/sse/intBase16x8.h \
    math/sse/int16x8.h \
    math/sse/uInt16x8.h \
    \
    math/sse/intBase8x16.h \
    math/sse/int8x16.h \
    math/sse/uInt8x16.h \
    math/sse/doublex2.h \
    math/sse/doublex4.h \
    math/sse/doublex8.h \
    \
    math/avx/avxInteger.h \
    math/avx/int16x16.h \
    math/avx/int32x16v.h \
    math/avx/int32x8.h \
    math/avx/int64x4.h \
    \
    math/mathUtils.h \
    math/eulerAngles.h \
    math/puzzleBlock.h \
    math/matrix/similarityReconstructor.h \
    math/sse/doublexT4.h \
    math/extensiveCoding.h \
    math/wisdom.h

SOURCES += \
    math/matrix/matrix.cpp \
    math/matrix/matrix22.cpp \
    math/matrix/matrix33.cpp \
    math/matrix/matrix44.cpp \
    math/matrix/diagonalMatrix.cpp \
    math/sparseMatrix.cpp \
    math/matrix/homographyReconstructor.cpp \
    math/vector/vector2d.cpp \
    math/lutAlgebra.cpp \
    math/affine.cpp \
    math/projectiveTransform.cpp \
    math/quaternion.cpp \
    math/gradientDescent.cpp \
    math/helperFunctions.cpp \
    math/generic/genericMath.cpp \
    math/sse/sseWrapper.cpp \
    math/matrix/similarityReconstructor.cpp \
    math/wisdom.cpp \

contains(DEFINES, "WITH_FFTW") {
    !build_pass: message(Adding core submodule math : fftw wrapper)

HEADERS += \
    math/fftw/fftwWrapper.h \

SOURCES += \
    math/fftw/fftwWrapper.cpp \

}
