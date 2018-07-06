isEmpty(RAPIDJSON_WRAPPER_DIR) {
    message(Incorrect usage of rapidjson.pri with empty RAPIDJSON_WRAPPER_DIR. Rapidjson is switched off!)
} else {
    !exists($$RAPIDJSON_WRAPPER_DIR/sources/readme.md) {
        message(Rapidjson is switched off! Not cloned to $$RAPIDJSON_WRAPPER_DIR)
    } else {
        message(Rapidjson found!)

        include(rapidjsonLibs.pri)
    }
}

contains(DEFINES, WITH_RAPIDJSON) {                    # if it's installed properly with found path for lib

#   INCLUDEPATH += $$RAPIDJSON_WRAPPER_DIR
    INCLUDEPATH += $$RAPIDJSON_WRAPPER_DIR/sources/include/


#    HEADERS +=  $$RAPIDJSON_WRAPPER_DIR/rapidJSONReader.h
#    SOURCES +=  $$RAPIDJSON_WRAPPER_DIR/rapidJSONReader.cpp

##    HEADERS +=  $$RAPIDJSON_WRAPPER_DIR/rapidJSONWriter.h
##    SOURCES +=  $$RAPIDJSON_WRAPPER_DIR/rapidJSONWriter.cpp

}

