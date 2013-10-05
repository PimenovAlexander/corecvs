with_opencv {
    OPENCV_PATH = $$(OPENCV_PATH)
    win32 {
        !isEmpty(OPENCV_PATH) {
            exists($$OPENCV_PATH/include/opencv2/core/version.hpp) {
                INCLUDEPATH += $$OPENCV_PATH/include
            }
            DEFINES += WITH_OPENCV          # we assume that we'll find OpenCV if it's installed; otherwise please adapt this file.

            exists($$OPENCV_PATH/bin/cv210.dll) {
                !build_pass:message(Using <$$OPENCV_PATH/bin>)
                LIBS += \
                    -L$$OPENCV_PATH/bin \
                    -lcv210 \
                    -lcxcore210 \
                    -lcvaux210 \
                    -lcxts210 \
                    -lhighgui210 \
                    -llibopencv_features2d210 \
                    -lopencv_flann210 \
                    -lml210 \
                    -lopencv_ffmpeg210
            } else:exists($$OPENCV_PATH/bin/libcv200.dll) {
                !build_pass:message(Using <$$OPENCV_PATH/bin>)
                LIBS += \
                    -L$$OPENCV_PATH/bin \
                    -llibcv200 \
                    -llibcxcore200 \
                    -llibcvaux200 \
                    -llibcxts200 \
                    -llibhighgui200 \
                    -llibopencv_features2d200 \
                    -llibopencv_flann200 \
                    -llibml200 \
                    -llibopencv_ffmpeg200
            } else:exists($$OPENCV_PATH/bin/libopencv_core220.dll) {
                !build_pass:message(Using <$$OPENCV_PATH/bin>)
                LIBS += \
                    -L$$OPENCV_PATH/bin/ \
                    -llibopencv_calib3d220 \
                    -llibopencv_video220 \
                    -llibopencv_core220 \
                    -llibopencv_highgui220 \
                    -llibopencv_features2d220 \
                    -llibopencv_flann220 \
                    -llibopencv_legacy220 \
                    -llibopencv_imgproc220
            } else:exists($$OPENCV_PATH/bin/libopencv_core249.dll) {
                !build_pass:message(Using <$$OPENCV_PATH/bin>)
                INCLUDEPATH += $$OPENCV_PATH/include \
                    $$OPENCV_PATH/modules/core/include \
                    $$OPENCV_PATH/modules/calib3d/include \
                    $$OPENCV_PATH/modules/video/include \
                    $$OPENCV_PATH/modules/highgui/include \
                    $$OPENCV_PATH/modules/imgproc/include \
                    $$OPENCV_PATH/modules/features2d/include \
                    $$OPENCV_PATH/modules/flann/include \
                    $$OPENCV_PATH/modules/objdetect/include \
                    $$OPENCV_PATH/modules/legacy/include

                LIBS += \
                    -L$$OPENCV_PATH/bin/ \
                    -llibopencv_calib3d249 \
                    -llibopencv_video249 \
                    -llibopencv_core249 \
                    -llibopencv_highgui249 \
                    -llibopencv_features2d249 \
                    -llibopencv_legacy249 \
                    -llibopencv_objdetect249 \
                    -llibopencv_imgproc249
            } else:exists($$OPENCV_PATH/build/lib/Release/opencv_core220.lib) {
                !build_pass:message(Using <$$OPENCV_PATH/build/lib/Release>)
                INCLUDEPATH += \
                    $$OPENCV_PATH/build/include
                LIBS += \
                    -L$$OPENCV_PATH/build/lib/Release/ \
                    -lopencv_calib3d220 \
                    -lopencv_video220 \
                    -lopencv_core220 \
                    -lopencv_highgui220 \
                    -lopencv_features2d220 \
                    -lopencv_flann220 \
                    -lopencv_legacy220 \
                    -lopencv_imgproc220 \
                    -lopencv_objdetect220
            } else:exists($$OPENCV_PATH/build/x64/mingw/bin/libopencv_core230.dll) {
                !build_pass:message(Using <$$OPENCV_PATH/build/x64/mingw/bin>)
                INCLUDEPATH += \
                    $$OPENCV_PATH/build/include
                LIBS += \
                    -L$$OPENCV_PATH/build/x64/mingw/lib/ \
                    -llibopencv_video230 \
                    -llibopencv_core230 \
                    -llibopencv_flann230 \
                    -llibopencv_highgui230
            } else:exists($$OPENCV_PATH/build/x64/mingw/bin/libopencv_core242.dll): !win32-msvc* {
                !build_pass:message(Using <$$OPENCV_PATH/build/x64/mingw/bin>)
                INCLUDEPATH += \
                    $$OPENCV_PATH/build/include
                LIBS += \
                    -L$$OPENCV_PATH/build/x64/mingw/lib/ \
                    -llibopencv_calib3d242 \
                    -llibopencv_video242 \
                    -llibopencv_core242 \
                    -llibopencv_highgui242 \
                    -llibopencv_features2d242 \
                    -llibopencv_flann242 \
                    -llibopencv_legacy242 \
                    -llibopencv_nonfree242 \
                    -llibopencv_imgproc242 \
                    -llibopencv_objdetect242
            } else:exists($$OPENCV_PATH/build/x64/vc10/bin/opencv_core242.dll): win32-msvc* {
                !build_pass:message(Using <$$OPENCV_PATH/build/x64/vc10/bin>)
                INCLUDEPATH += \
                    $$OPENCV_PATH/build/include
                LIBS += \
                    -L$$OPENCV_PATH/build/x64/vc10/lib/ \
                    -lopencv_calib3d242 \
                    -lopencv_video242 \
                    -lopencv_core242 \
                    -lopencv_highgui242 \
                    -lopencv_features2d242 \
                    -lopencv_flann242 \
                    -lopencv_legacy242 \
                    -lopencv_nonfree242 \
                    -lopencv_imgproc242 \
                    -lopencv_objdetect242
            } else {
                message(Unsupported OpenCV version)
            }
        } else {
            !build_pass:message(OpenCV not found)
        }

    } else:macx {
        !build_pass:message(Compiling with OpenCV from $$OPENCV_PATH)

        LIBS += -L$$OPENCV_PATH/lib/
        LIBS += -lopencv_highgui -lopencv_video -lopencv_core

        INCLUDEPATH += $$OPENCV_PATH/include
        DEPENDPATH  += $$OPENCV_PATH/include
        DEFINES     += WITH_OPENCV
    } else {
        isEmpty(OPENCV_PATH) {
            !build_pass:message(Compiling with system OpenCV)
#           LIBS += -lcv -lhighgui -lcxcore
            LIBS += -lopencv_highgui -lopencv_video -lopencv_core -lopencv_flann -lopencv_imgproc -lopencv_calib3d -lopencv_features2d -lopencv_objdetect # for opencv 2.3+
        } else {
            !build_pass:message(Compiling with OpenCV from $$OPENCV_PATH)
            LIBS += -L$$OPENCV_PATH/lib/

            LIBS += -lopencv_legacy
            LIBS += -lopencv_highgui
            LIBS += -lopencv_contrib
#           LIBS += -lopencv_ts
            LIBS += -lopencv_objdetect
            LIBS += -lopencv_calib3d
            LIBS += -lopencv_ml
            LIBS += -lopencv_flann
#           LIBS += -lopencv_videostab
            LIBS += -lopencv_nonfree
            LIBS += -lopencv_features2d
#           LIBS += -lopencv_photo
            LIBS += -lopencv_video
            LIBS += -lopencv_gpu
            LIBS += -lopencv_core
            LIBS += -lopencv_imgproc
#           LIBS += -lopencv_stitching

            INCLUDEPATH += $$OPENCV_PATH/include
            DEPENDPATH  += $$OPENCV_PATH/include
        }
        DEFINES += WITH_OPENCV
    }
}
