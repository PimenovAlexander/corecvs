isEmpty(LIBCGAL_WRAPPER_DIR) {
    message(Incorrect usage of libcgal.pri with empty LIBCGAL_WRAPPER_DIR. Libcgal is switched off!)
} else {
    include(libcgalLibs.pri)
}

contains(DEFINES, WITH_LIBCGAL) {                    # if it's installed properly with found path for lib

    INCLUDEPATH += $$LIBCGAL_WRAPPER_DIR

    HEADERS += \
                $$LIBCGAL_WRAPPER_DIR/libcgalWrapper.h \

		
    SOURCES += \
                $$LIBCGAL_WRAPPER_DIR/libcgalWrapper.cpp \

}

