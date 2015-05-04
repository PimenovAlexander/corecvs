# try use global config
exists(../../../config.pri) {
    #message(Using global config for tests)
    include(../../../config.pri)
} else {
    message(Using local config for tests)
    include(../config.pri)
}

TEMPLATE = subdirs

SUBDIRS +=       \
#    xmlserialize \
#    avigrab     \
    topcon      \
    visual      \
    nvmmerger   \

unix {
#    SUBDIRS += grab
#    SUBDIRS += adoptcolor
}


grab.file          = grab/grab.pro
xmlserialize.file  = xmlserialize/xmlserialize.pro
adoptcolor.file    = adoptcolor/adoptcolor.pro