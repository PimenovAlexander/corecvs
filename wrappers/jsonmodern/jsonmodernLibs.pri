with_jsonmodern {

    !c++11:!c++14 {
        !build_pass: message(Json for Modern C++ is switched off! Requires c++11/c++14 standard)
    } else : win32-msvc2010 {
        !build_pass: message(Json for Modern C++ is switched off! Not supported MSVC<2015)
    } else : win32-msvc2013 {
        !build_pass: message(Json for Modern C++ is switched off! Not supported MSVC<2015)
    } else {
        !build_pass: message(Json for Modern C++ module is ready for use)

        #JSONMODERN_PATH = $$(JSONMODERN_PATH)
        INCLUDEPATH += $$JSONMODERN_WRAPPER_DIR/sources/src/
        DEFINES += WITH_JSONMODERN
    }
}
