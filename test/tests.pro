# try use global config 
exists(../../../config.pri) {
    #message(Using global config)
    include(../../../config.pri)
} else { 
    message(Using local config)
    include(../config.pri)
}
 
TEMPLATE = subdirs

SUBDIRS +=     \
    grab24     \
    adoptcolor \
    fileloader \
    softrender  \


softrender.file                     = softrender/softrender.pro