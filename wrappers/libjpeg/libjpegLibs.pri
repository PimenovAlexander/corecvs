with_libjpeg {
    LIBJPEG_PATH = $$(LIBJPEG_PATH)
    win32 {
        !isEmpty(LIBJPEG_PATH) {
            exists($$LIBJPEG_PATH/include/) {
                INCLUDEPATH += $$LIBJPEG_PATH/include                            # when we made "make install" there
            }
        }
    } else {
        isEmpty(LIBJPEG_PATH) {
            !build_pass:message(Compiling with system Libjpeg)
        } else {
            !build_pass:message(Compiling with Libjpeg from $$LIBJPEG_PATH)
            DEPENDPATH  += $$LIBJPEG_PATH/include
            INCLUDEPATH += $$LIBJPEG_PATH/include
            LIBS        += -$$LIBJPEG_PATH/lib/
        }
        LIBS += -ljpeg
        DEFINES += WITH_LIBJPEG
    }
}
