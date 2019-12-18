target_sources(corecvs
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/polynomialSolver.h
        ${CMAKE_CURRENT_LIST_DIR}/polynomial.h
        ${CMAKE_CURRENT_LIST_DIR}/basis.h
        ${CMAKE_CURRENT_LIST_DIR}/monom.h
        ${CMAKE_CURRENT_LIST_DIR}/polynom.h
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/polynomialSolver.cpp
        ${CMAKE_CURRENT_LIST_DIR}/polynomial.cpp
#        ${CMAKE_CURRENT_LIST_DIR}/basis.cpp
#        ${CMAKE_CURRENT_LIST_DIR}/monom.cpp
#        ${CMAKE_CURRENT_LIST_DIR}/polynom.cpp
)
