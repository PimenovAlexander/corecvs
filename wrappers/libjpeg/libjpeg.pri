isEmpty(LIBJPEG_WRAPPER_DIR) {
    message(Incorrect usage of libjpeg.pri with empty LIBJPEG_WRAPPER_DIR. Libjpeg is switched off!)
} else {
     include(libjpegLibs.pri)
}

contains(DEFINES, WITH_LIBJPEG) {                    # if it's installed properly with found path for lib

    INCLUDEPATH += $$LIBJPEG_WRAPPER_DIR

    HEADERS += \
                $$LIBJPEG_WRAPPER_DIR/libjpegFileReader.h \

		
    SOURCES += \
                $$LIBJPEG_WRAPPER_DIR/libjpegFileReader.cpp \

}

