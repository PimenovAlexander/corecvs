GTEST_PATH = $$(GTEST_PATH)
isEmpty(GTEST_PATH) {
    win32 {
        message(GTEST_PATH not configured!)
    } else {
        LIBS += -lgtest
    }
} else {
    INCLUDEPATH += $$GTEST_PATH/include

    CONFIG(debug, debug|release) {
        LIBS += -L$$GTEST_PATH/build/Debug   -lgtest
       #LIBS += -L$$GTEST_PATH/build/Release -lgtest
    }
    CONFIG(release, debug|release) {
        LIBS += -L$$GTEST_PATH/build/Release -lgtest
    }
}
