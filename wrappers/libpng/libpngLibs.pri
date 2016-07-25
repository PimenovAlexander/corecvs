with_libpng {
    LIBPNG_PATH = $$(LIBPNG_PATH)
    win32 {
        !isEmpty(LIBPNG_PATH) {
            exists($$LIBPNG_PATH/include/) {
                INCLUDEPATH += $$LIBPNG_PATH/include                            # when we made "make install" there
            }
        }
    } else {
        isEmpty(LIBPNG_PATH) {
            !build_pass:message(Compiling with system Libpng)
        } else {
            !build_pass:message(Compiling with libPng from $$LIBPNG_PATH)
            DEPENDPATH  += $$LIBPNG_PATH/include
            INCLUDEPATH += $$LIBPNG_PATH/include
            LIBS        += -$$LIBPNG_PATH/lib/
        }
        LIBS += -lpng
        DEFINES += WITH_LIBPNG
    }
}
