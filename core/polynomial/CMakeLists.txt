set(POLYNOMIAL_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/polynomialSolver.h
    ${CMAKE_CURRENT_LIST_DIR}/polynomial.h
    ${CMAKE_CURRENT_LIST_DIR}/basis.h
    ${CMAKE_CURRENT_LIST_DIR}/monom.h
    ${CMAKE_CURRENT_LIST_DIR}/polynom.h
    ${CMAKE_CURRENT_LIST_DIR}/finiteField.h
    PARENT_SCOPE
    )

set(POLYNOMIAL_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/polynomialSolver.cpp
    ${CMAKE_CURRENT_LIST_DIR}/polynomial.cpp
    PARENT_SCOPE
    )
