# try use global config
exists(../../../../config.pri) {
    ROOT_DIR=../../../..
    #message(Using global config)
    include($$ROOT_DIR/config.pri)
} else { 
    #message(Using local config)
    ROOT_DIR=../..
    include($$ROOT_DIR/cvs-config.pri)
}
ROOT_DIR=$$PWD/$$ROOT_DIR

TEMPLATE = app
TARGET   = test_serialize1
QT 	+= xml

include(../../utils/utils.pri)                      # it uses TARGET, ROOT_DIR and detects UTILS_BINDIR, OBJECTS_DIR, DESTDIR, ...!
include(../../wrappers/gtest/gtest.pri)

SOURCES += main_serialize1.cpp
HEADERS += main_serialize1.h


#with_rapidjson {
#    message(We have rapidjson)
#    RAPIDJSON_WRAPPER_DIR = ../../wrappers/rapidjson
#    include($$RAPIDJSON_WRAPPER_DIR/rapidjson.pri)
#
#    contains(DEFINES, WITH_RAPIDJSON) {
#        INCLUDEPATH += $$RAPIDJSON_WRAPPER_DIR
#    }
#}

#with_jsonmodern {
#    message(We have Json for Modern C++ )
#    JSONMODERN_WRAPPER_DIR = ../../wrappers/jsonmodern
#    include($$JSONMODERN_WRAPPER_DIR/jsonmodern.pri)
#
#    contains(DEFINES, WITH_JSONMODERN) {
#        INCLUDEPATH += $$JSONMODERN_WRAPPER_DIR
#    }
#}

