isEmpty(LIBPNG_WRAPPER_DIR) {
    message(Incorrect usage of libpng.pri with empty LIBPNG_WRAPPER_DIR. Libpng is switched off!)
} else {
    include(libpngLibs.pri)
}

contains(DEFINES, WITH_LIBPNG) {                    # if it's installed properly with found path for lib

    INCLUDEPATH += $$LIBPNG_WRAPPER_DIR

    HEADERS += \
                $$LIBPNG_WRAPPER_DIR/libpngFileReader.h \

		
    SOURCES += \
                $$LIBPNG_WRAPPER_DIR/libpngFileReader.cpp \

}

