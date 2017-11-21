# try use global config
exists(../../../../config.pri) {
    #message(Using global config)
    ROOT_DIR=../../../..
    include($$ROOT_DIR/config.pri)
} else {
    message(Using local config)
    ROOT_DIR=../..
    include($$ROOT_DIR/cvs-config.pri)
}
ROOT_DIR=$$PWD/$$ROOT_DIR

message(Adding gtest. Root dir is $$ROOT_DIR)

exists($$ROOT_DIR/siblings/gtest) {
    !build_pass: message(Using local gtest from siblings/gtest)

    INCLUDEPATH += $$ROOT_DIR/siblings/gtest/
    INCLUDEPATH += $$ROOT_DIR/siblings/gtest/include

    SOURCES += $$ROOT_DIR/siblings/gtest/src/gtest-all.cc
    HEADERS += $$ROOT_DIR/siblings/gtest/include/gtest/*.h

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
    }
}
