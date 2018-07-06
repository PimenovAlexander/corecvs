isEmpty(LIBPCL_WRAPPER_DIR) {
    message(Incorrect usage of libpcl.pri with empty LIBPCL_WRAPPER_DIR. Libpcl is switched off!)
} else {
    include(libpclLibs.pri)
}

contains(DEFINES, WITH_LIBPCL) {                    # if it's found

    include(../eigen/eigen.pri)                     # PCL requires Eigen

    INCLUDEPATH += $$LIBPCL_WRAPPER_DIR

    HEADERS += $$LIBPCL_WRAPPER_DIR/libpclWrapper.h
    SOURCES += $$LIBPCL_WRAPPER_DIR/libpclWrapper.cpp

    win32 {
        CONFIG += with_boost
        exists("$$PCL_ROOT/3rdParty/Boost/include/boost-1_64/boost/version.hpp") {
            BOOST_INCS_HINT = $$PCL_ROOT/3rdParty/Boost/include/boost-1_64
        }
        include(../boost/boost.pri)                 # PCL requires boost

        CONFIG += with_flann                        # PCL requires flann
        exists($$PCL_ROOT/3rdParty/FLANN/include/flann/flann.hpp) {
            INCLUDEPATH += $$PCL_ROOT/3rdParty/FLANN/include
#           LIBS        += -L$$PCL_ROOT/3rdParty/FLANN/lib/ -lflann -lflann_cpp     # not needed as PCL is prebuilt
        } else {
            !build_pass:message(PCL requires FLANN that is not found)
        }
    }
}
