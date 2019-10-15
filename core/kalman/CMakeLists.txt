target_sources(corecvs
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/cooSparseMatrix.h
        ${CMAKE_CURRENT_LIST_DIR}/cholesky.h
        ${CMAKE_CURRENT_LIST_DIR}/upperUnitaryMatrix.h
        ${CMAKE_CURRENT_LIST_DIR}/uduDecomposed.h
    #    ${CMAKE_CURRENT_LIST_DIR}/kalman.h
        ${CMAKE_CURRENT_LIST_DIR}/classicKalman.h

    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/cooSparseMatrix.cpp
        ${CMAKE_CURRENT_LIST_DIR}/cholesky.cpp
        ${CMAKE_CURRENT_LIST_DIR}/upperUnitaryMatrix.cpp
        ${CMAKE_CURRENT_LIST_DIR}/uduDecomposed.cpp
    #    ${CMAKE_CURRENT_LIST_DIR}/kalman.cpp
        ${CMAKE_CURRENT_LIST_DIR}/classicKalman.cpp
)

