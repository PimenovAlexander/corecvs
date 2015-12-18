# try use global config 
exists(../../../../config.pri) {
    ROOT_DIR=../../../..
    #message(Using global config)
} else { 
    message(Using local config)
    ROOT_DIR=../..
}
ROOT_DIR=$$PWD/$$ROOT_DIR
include($$ROOT_DIR/config.pri)

CONFIG  += staticlib
TARGET   = cvs_application_base
TEMPLATE = lib

HOSTBASE_DIR=.
include ($$HOSTBASE_DIR/baseApplication.pri)                   # it uses HOSTBASE_DIR, detects HOSTBASE_BINDIR, OBJECTS_DIR, ...

HEADERS += \
    generatedParameters/baseParameters.h \
    generatedParameters/presentationParameters.h \
    generatedParameters/rotationPresets.h \
    generatedParameters/flowStyle.h \
    generatedParameters/stereoStyle.h \
    generatedParameters/outputStyle.h \    
    layers/resultImage.h \
    layers/imageResultLayer.h \
    memoryUsageCalculator/memoryUsageCalculator.h \
    parametersMapper/parametersMapperBase.h \
    statistics/statisticsDialog.h \
    statistics/qtStatisticsCollector.h \
    abstractCalculationThread.h \
    baseCalculationThread.h \
    baseOutputData.h \
    baseHostDialog.h \
    mainWindow.h \
    baseParametersControlWidget.h \
    presentationParametersControlWidget.h \
    camerasConfigParameters.h \
    

SOURCES += \
    generatedParameters/baseParameters.cpp \
    generatedParameters/presentationParameters.cpp \
    layers/resultImage.cpp \
    layers/imageResultLayer.cpp \
    memoryUsageCalculator/memoryUsageCalculator.cpp \
    parametersMapper/parametersMapperBase.cpp \
    statistics/statisticsDialog.cpp \
    abstractCalculationThread.cpp \
    baseCalculationThread.cpp \
    baseOutputData.cpp \
    baseHostDialog.cpp \
    mainWindow.cpp \
    baseParametersControlWidget.cpp \
    presentationParametersControlWidget.cpp \
    camerasConfigParameters.cpp \
    

FORMS += \
    statistics/statisticsDialog.ui \
    ui/hostDialogStub.ui \
    ui/baseParametersControlWidget.ui \
    ui/presentationParametersControlWidget.ui \
    ui/mainWindow.ui \

with_opengl {
    HEADERS += scene3DMouse.h
    SOURCES += scene3DMouse.cpp
}

win32 {
   HEADERS += memoryUsageCalculator/windowsMemoryUsageCalculator.h
   SOURCES += memoryUsageCalculator/windowsMemoryUsageCalculator.cpp
} else:macx {
   HEADERS += memoryUsageCalculator/macMemoryUsageCalculator.h
   SOURCES += memoryUsageCalculator/macMemoryUsageCalculator.cpp
} else {
   HEADERS += memoryUsageCalculator/linuxMemoryUsageCalculator.h
   SOURCES += memoryUsageCalculator/linuxMemoryUsageCalculator.cpp
}

