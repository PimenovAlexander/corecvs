# try use global config
exists(../../../../config.pri) {
    #message(Using global config)
    ROOT_DIR=../../../..
    include($$ROOT_DIR/config.pri)
} else {
    #message(Using local config)
    ROOT_DIR=../..
    include($$ROOT_DIR/cvs-config.pri)
}
ROOT_DIR=$$PWD/$$ROOT_DIR

GTEST_SRC_PATH = "$$(GTEST_SRC_PATH)"
isEmpty(GTEST_SRC_PATH) {
    GTEST_SRC_PATH="$$ROOT_DIR/siblings/gtest"

    !exists($$GTEST_SRC_PATH) {
        GTEST_SRC_PATH="$$ROOT_DIR/3dparty/gtest/googletest"
    }
}

exists($$GTEST_SRC_PATH) {
    !build_pass: message(Using local gtest source from $$GTEST_SRC_PATH)

    INCLUDEPATH += $$GTEST_SRC_PATH		# is needed to compile the native "gtest-all.cc" src file
    INCLUDEPATH += $$GTEST_SRC_PATH/include

    SOURCES += $$GTEST_SRC_PATH/src/gtest-all.cc
    HEADERS += $$GTEST_SRC_PATH/include/gtest/*.h

} else {

    GTEST_PATH = "$$(GTEST_PATH)"
    isEmpty(GTEST_PATH) {
        win32 {
            !build_pass: message(GTEST_PATH not configured!)
        } else {
            LIBS += -lgtest
        }
    } else {
        INCLUDEPATH += "$$GTEST_PATH"/include
        win32 {
            equals(QMAKE_TARGET.arch, "x86") {
                GTEST_PATH_BUILD = "$$GTEST_PATH"/build_x86
            } else {
                GTEST_PATH_BUILD = "$$GTEST_PATH"/build
            }

            CONFIG(debug, debug|release) {
                LIBS += -L"$$GTEST_PATH_BUILD/Debug"   -lgtest
            }
            CONFIG(release, debug|release) {
                LIBS += -L"$$GTEST_PATH_BUILD/Release" -lgtest
            }

            !build_pass: message(Using gtest from <$$GTEST_PATH/build/Release|Debug>)
        } else {
            !build_pass: message(Using gtest from <$$GTEST_PATH_BUILD/lib>)
            LIBS += -L"$$GTEST_PATH_BUILD/lib" -lgtest
        }
    }
}
