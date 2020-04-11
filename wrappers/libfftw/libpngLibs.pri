with_libpng {
    !build_pass:message(Including libpng lib to link list)

    LIBPNG_PATH = $$(LIBPNG_PATH)
    win32 {
        !isEmpty(LIBPNG_PATH) {
            exists($$LIBPNG_PATH/include/) {
                INCLUDEPATH += $$LIBPNG_PATH/include                            # when we made "make install" there
            }
        }
    } else {
        isEmpty(LIBPNG_PATH) {
            !build_pass:message(Linking with system Libpng)
            exists(/usr/include/libpng/png.h) {
                LIBS += -lpng
                DEFINES += WITH_LIBPNG
            } else {
                !build_pass:message(libpng not found.)
            }

        } else {
            !build_pass:message(Linking with libPng from $$LIBPNG_PATH)
            DEPENDPATH  += $$LIBPNG_PATH/include
            INCLUDEPATH += $$LIBPNG_PATH/include
            LIBS        += -L$$LIBPNG_PATH/lib/
            LIBS += -lpng
            DEFINES += WITH_LIBPNG
        }
    }
}
