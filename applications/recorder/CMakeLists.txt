target_sources(corecvs
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/recorderDialog.h
        ${CMAKE_CURRENT_LIST_DIR}/recorderThread.h
        ${CMAKE_CURRENT_LIST_DIR}/recorderControlWidget.h                             # control widgets
        ${CMAKE_CURRENT_LIST_DIR}/generatedParameters/recorder.h                      # parameters for calculation threads, host dialogs etc.
        ${CMAKE_CURRENT_LIST_DIR}/parametersMapper/parametersMapperRecorder.h         # parameters for params mapper
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/main_recorder.cpp
        ${CMAKE_CURRENT_LIST_DIR}/recorderDialog.cpp
        ${CMAKE_CURRENT_LIST_DIR}/recorderThread.cpp
        ${CMAKE_CURRENT_LIST_DIR}/recorderControlWidget.cpp                           # control widgets
        ${CMAKE_CURRENT_LIST_DIR}/generatedParameters/recorder.cpp                    # parameters for calculation threads, host dialogs etc.
        ${CMAKE_CURRENT_LIST_DIR}/parametersMapper/parametersMapperRecorder.cpp       # parameters for params mapper
)
