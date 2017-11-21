isEmpty(JSONMODERN_WRAPPER_DIR) {
    !build_pass: message(Json for Modern C++ is switched off! Empty JSONMODERN_WRAPPER_DIR in jsonmodern.pri)
} else {
    !exists($$JSONMODERN_WRAPPER_DIR/sources/src/json.hpp) {
        !build_pass: message(Json for Modern C++ is switched off! Not cloned)
    } else {
        include(jsonmodernLibs.pri)
    }
}

contains(DEFINES, WITH_JSONMODERN) {                    # if it's installed properly with found path for lib

#   INCLUDEPATH += $$JSONMODERN_WRAPPER_DIR
    INCLUDEPATH += $$JSONMODERN_WRAPPER_DIR/sources/src/

#   HEADERS +=  $$JSONMODERN_WRAPPER_DIR/jsonModernReader.h
#   SOURCES +=  $$JSONMODERN_WRAPPER_DIR/jsonModernReader.cpp
}
