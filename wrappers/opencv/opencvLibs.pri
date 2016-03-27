with_opencv {
    OPENCV_PATH = $$(OPENCV_PATH)
    win32 {
        !isEmpty(OPENCV_PATH) {
            exists($$OPENCV_PATH/include/opencv2/core/version.hpp) {
                INCLUDEPATH += $$OPENCV_PATH/include                            # when we made "make install" there
            }
            #OPENCV_INC_NOTINSTALLED = $$OPENCV_PATH/build/include              # it's strange path, usually empty, but distributive has all includes there?...

            # the following paths are taking includes directly from sources
            #
            exists($$OPENCV_PATH/sources/modules/core/include/opencv2/core/core.hpp) {  # since v.2.4.11 sources at their folder
                OPENCV_SRC_MODULES = $$OPENCV_PATH/sources/modules
            } else:exists($$OPENCV_PATH/modules/core/include/opencv2/core/core.hpp) {   # 2.4.9 has sources at the root folder
                OPENCV_SRC_MODULES = $$OPENCV_PATH/modules
            }
            exists($$OPENCV_SRC_MODULES/core/include/opencv2/core/core.hpp) {
                OPENCV_INC_NOTINSTALLED += \
                    $$OPENCV_SRC_MODULES/core/include \
                    $$OPENCV_SRC_MODULES/calib3d/include \
                    $$OPENCV_SRC_MODULES/video/include \
                    $$OPENCV_SRC_MODULES/highgui/include \
                    $$OPENCV_SRC_MODULES/imgproc/include \
                    $$OPENCV_SRC_MODULES/features2d/include \
                    $$OPENCV_SRC_MODULES/flann/include \
                    $$OPENCV_SRC_MODULES/objdetect/include \
                    $$OPENCV_SRC_MODULES/nonfree/include \      # our opencv wrapper for SIFT, SURF stuff needs this
                    $$OPENCV_SRC_MODULES/legacy/include \       # our featureDetectorCV uses "BruteForceMatcher"
                    $$OPENCV_SRC_MODULES/ml/include             # legacy.hpp includes ml.hpp
            }

            OPENCV_249_LIBS_R = -lopencv_calib3d249    -lopencv_video249   -lopencv_core249     -lopencv_highgui249   \
                                -lopencv_features2d249 -lopencv_flann249   -lopencv_imgproc249  -lopencv_objdetect249 \
                                -lopencv_nonfree249    -lopencv_legacy249 #-lopencv_ml249

            OPENCV_249_LIBS_D = -lopencv_calib3d249d    -lopencv_video249d   -lopencv_core249d    -lopencv_highgui249d   \
                                -lopencv_features2d249d -lopencv_flann249d   -lopencv_imgproc249d -lopencv_objdetect249d \
                                -lopencv_nonfree249d    -lopencv_legacy249d #-lopencv_ml249d
            CONFIG(debug, debug|release) {
                OPENCV_249_LIBS = $$OPENCV_249_LIBS_D
                OPENCV_249_LIBS_ADD_OWN_BUILT = -L$$OPENCV_PATH/build/lib/Debug/   $$OPENCV_249_LIBS
            }
            CONFIG(release, debug|release) {
                OPENCV_249_LIBS = $$OPENCV_249_LIBS_R
                OPENCV_249_LIBS_ADD_OWN_BUILT = -L$$OPENCV_PATH/build/lib/Release/ $$OPENCV_249_LIBS
            }

            exists($$OPENCV_PATH/build/bin/Release/opencv_core249.dll): win32-msvc* {               # git's OpenCV tag=2.4.9 built by any own MSVC with GPU
                !build_pass:message(Using <$$OPENCV_PATH/build/bin/Release|Debug>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS        += $$OPENCV_249_LIBS_ADD_OWN_BUILT
                DEFINES     += WITH_OPENCV
            } else:exists($$OPENCV_PATH/build/x64/vc10/bin/opencv_core249.dll): win32-msvc2010 {    # built OpenCV v.2.4.9 with msvc10 without GPU
                !build_pass:message(Using <$$OPENCV_PATH/build/x64/vc10/bin>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS        += -L$$OPENCV_PATH/build/x64/vc10/lib/ $$OPENCV_249_LIBS
                DEFINES     += WITH_OPENCV
            } else:exists($$OPENCV_PATH/build/x64/vc12/bin/opencv_core249.dll): win32-msvc2013 {    # built OpenCV v.2.4.9 with vc12 without GPU
                !build_pass:message(Using <$$OPENCV_PATH/build/x64/vc12/bin>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS        += -L$$OPENCV_PATH/build/x64/vc12/lib/ $$OPENCV_249_LIBS
                DEFINES     += WITH_OPENCV
            } else:exists($$OPENCV_PATH/x64/vc12/bin/opencv_core249.dll): win32-msvc* {             # installed OpenCV v.2.4.9 with msvc* without GPU (our integration server)
                !build_pass:message(Using <$$OPENCV_PATH/x64/vc12/bin>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS        += -L$$OPENCV_PATH/x64/vc12/lib/ $$OPENCV_249_LIBS
                DEFINES     += WITH_OPENCV
            } else:exists($$OPENCV_PATH/build.mg/bin/libopencv_core249.dll): !win32-msvc* {         # git's OpenCV tag=2.4.9 built by MINGW without GPU
                !build_pass:message(Using <$$OPENCV_PATH/build.mg/bin>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS += -L$$OPENCV_PATH/build.mg/bin/ \
                        -llibopencv_calib3d249    -llibopencv_video249   -llibopencv_core249     -llibopencv_highgui249   \
                        -llibopencv_features2d249 -llibopencv_flann249   -llibopencv_imgproc249  -llibopencv_objdetect249 \
                        -llibopencv_nonfree249    -llibopencv_legacy249 #-llibopencv_ml249
                DEFINES += WITH_OPENCV
            } else:exists($$OPENCV_PATH/build/x64/vc12/bin/opencv_core2411.dll) {                   # OpenCV tag=2.4.11 built by vc12 without GPU
                !build_pass:message(Using <$$OPENCV_PATH/build/x64/vc12/bin>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS += -L$$OPENCV_PATH/build/x64/vc12/lib/ \
                        -lopencv_calib3d2411    -lopencv_video2411   -lopencv_core2411    -lopencv_highgui2411   \
                        -lopencv_features2d2411 -lopencv_flann2411   -lopencv_imgproc2411 -lopencv_objdetect2411 \
                        -lopencv_nonfree2411 -lopencv_legacy2411 #-llibopencv_ml2411
                DEFINES += WITH_OPENCV
            } else {
                message(Using <$$OPENCV_PATH>)
                message(Unsupported OpenCV version - please adapt the opencvLibs.pri for other versions)
            }
        } else {
            !build_pass:message(OpenCV not found)
        }
    } else:macx {
        !build_pass:message(Compiling with OpenCV from $$OPENCV_PATH)

        LIBS += -L$$OPENCV_PATH/lib/ \
                -lopencv_calib3d    -lopencv_video   -lopencv_core    -lopencv_highgui   \
                -lopencv_features2d -lopencv_flann   -lopencv_imgproc -lopencv_objdetect \
                -llibopencv_nonfree -lopencv_legacy #-llibopencv_ml

        INCLUDEPATH += $$OPENCV_PATH/include
        DEPENDPATH  += $$OPENCV_PATH/include
        DEFINES     += WITH_OPENCV
    } else {
        isEmpty(OPENCV_PATH) {
            !build_pass:message(Compiling with system OpenCV)
        } else {
            !build_pass:message(Compiling with OpenCV from $$OPENCV_PATH)
            DEPENDPATH  += $$OPENCV_PATH/include
            INCLUDEPATH += $$OPENCV_PATH/include
            LIBS        += -L$$OPENCV_PATH/lib/
        }
        LIBS += -lopencv_calib3d    -lopencv_video   -lopencv_core    -lopencv_highgui   \
                -lopencv_features2d -lopencv_flann   -lopencv_imgproc -lopencv_objdetect \
                -lopencv_nonfree    -lopencv_legacy #-llibopencv_ml

       #LIBS += -lopencv_contrib -lopencv_ts -lopencv_videostab -lopencv_photo -lopencv_gpu -lopencv_ocl -lopencv_stitching -lopencv_superres

        DEFINES += WITH_OPENCV
    }
}
