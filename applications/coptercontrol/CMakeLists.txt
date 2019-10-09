target_sources(corecvs
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/copterDialog.h
        ${CMAKE_CURRENT_LIST_DIR}/copterThread.h
        ${CMAKE_CURRENT_LIST_DIR}/copterControlWidget.h                             # control widgets
        ${CMAKE_CURRENT_LIST_DIR}/generatedParameters/copter.h                      # parameters for calculation threads, host dialogs etc.
        ${CMAKE_CURRENT_LIST_DIR}/parametersMapper/parametersMapperCopter.h         # parameters for params mapper
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/main_copter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/copterDialog.cpp
        ${CMAKE_CURRENT_LIST_DIR}/copterThread.cpp
        ${CMAKE_CURRENT_LIST_DIR}/copterControlWidget.cpp                           # control widgets
        ${CMAKE_CURRENT_LIST_DIR}/generatedParameters/copter.cpp                    # parameters for calculation threads, host dialogs etc.
        ${CMAKE_CURRENT_LIST_DIR}/parametersMapper/parametersMapperCopter.cpp       # parameters for params mapper
)