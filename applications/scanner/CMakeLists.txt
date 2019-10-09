target_sources(corecvs
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/scannerDialog.h
        ${CMAKE_CURRENT_LIST_DIR}/scannerThread.h
        ${CMAKE_CURRENT_LIST_DIR}/scannerParametersControlWidget.h                           # control widgets
        ${CMAKE_CURRENT_LIST_DIR}/generatedParameters/scannerParameters.h                    # parameters for calculation threads, host dialogs etc.
        ${CMAKE_CURRENT_LIST_DIR}/generatedParameters/redRemovalType.h
        ${CMAKE_CURRENT_LIST_DIR}/parametersMapper/parametersMapperScanner.h                 # parameters for params mapper
        ${CMAKE_CURRENT_LIST_DIR}/laserlinedetection.h
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/main_scanner.cpp
        ${CMAKE_CURRENT_LIST_DIR}/scannerDialog.cpp
        ${CMAKE_CURRENT_LIST_DIR}/scannerThread.cpp
        ${CMAKE_CURRENT_LIST_DIR}/scannerParametersControlWidget.cpp                         # control widgets
        ${CMAKE_CURRENT_LIST_DIR}/generatedParameters/scannerParameters.cpp                  # parameters for calculation threads, host dialogs etc.
        ${CMAKE_CURRENT_LIST_DIR}/parametersMapper/parametersMapperScanner.cpp               # parameters for params mapper
        ${CMAKE_CURRENT_LIST_DIR}/laserlinedetection.cpp
)
