target_sources(corecvs
    PUBLIC
        meta/astNode.h
        meta/packedDerivative.h
        meta/floatJIT.h
        meta/astOptimize.h
    PRIVATE
        meta/astNode.cpp
        meta/packedDerivative.cpp
        meta/floatJIT.cpp
        meta/astOptimize.cpp
)

