with_opencv {
    OPENCV_PATH = $$(OPENCV_PATH)
    OPENCV_CONTRIB_PATH = $$(OPENCV_CONTRIB_PATH) # 3.x nonfree repository path
    win32 {
        !isEmpty(OPENCV_PATH) {
            exists($$OPENCV_PATH/include/opencv2/core/version.hpp) {
                INCLUDEPATH += $$OPENCV_PATH/include                            # when we made "make install" there
            }

            exists($$OPENCV_PATH/modules/core/include/opencv2/core/version.hpp) {
                INCLUDEPATH += $$OPENCV_PATH/include                            # when we made "make install" there in 3.1.x
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
                    $$OPENCV_SRC_MODULES/ml/include \           # legacy.hpp includes ml.hpp
                    $$OPENCV_SRC_MODULES/gpu/include \          # 2.4.x cuda ORB and Bruteforce matcher
                    $$OPENCV_SRC_MODULES/ocl/include \          # 2.4.x opencl Bruteforce matcher implementation
            }

            exists($$OPENCV_SRC_MODULES/cudafeatures2d/include/opencv2/cudafeatures2d.hpp) {
                OPENCV_INC_NOTINSTALLED += \
	                $$(OPENCV_PATH)/build \                        # 3.x generated opencv2\opencv_modules.hpp
                    $$OPENCV_SRC_MODULES/imgcodecs/include \
                    $$OPENCV_SRC_MODULES/videoio/include \
                    $$OPENCV_SRC_MODULES/cudafeatures2d/include \  # 3.x cuda ORB and Bruteforce matcher
                    $$OPENCV_SRC_MODULES/cudafilters/include
            }	

            !isEmpty(OPENCV_CONTRIB_PATH) {
                OPENCV_CONTRIB_SRC_MODULES = $$OPENCV_CONTRIB_PATH/modules
                OPENCV_INC_NOTINSTALLED += \
                    $$OPENCV_CONTRIB_SRC_MODULES/xfeatures2d/include \  # 3.x nonfree features
            }

            OPENCV_249_LIBS_R  = -lopencv_calib3d249      -lopencv_video249     -lopencv_core249      -lopencv_highgui249   \
                                 -lopencv_features2d249   -lopencv_flann249     -lopencv_imgproc249   -lopencv_objdetect249 \
                                 -lopencv_nonfree249      -lopencv_legacy249   #-lopencv_ml249

            OPENCV_249_LIBS_D  = -lopencv_calib3d249d     -lopencv_video249d    -lopencv_core249d     -lopencv_highgui249d   \
                                 -lopencv_features2d249d  -lopencv_flann249d    -lopencv_imgproc249d  -lopencv_objdetect249d \
                                 -lopencv_nonfree249d     -lopencv_legacy249d  #-lopencv_ml249d

            OPENCV_2411_LIBS_R = -lopencv_calib3d2411     -lopencv_video2411     -lopencv_core2411     -lopencv_highgui2411   \
                                 -lopencv_features2d2411  -lopencv_flann2411     -lopencv_imgproc2411  -lopencv_objdetect2411 \
                                 -lopencv_nonfree2411     -lopencv_legacy2411   #-lopencv_ml2411

            OPENCV_2411_LIBS_D = -lopencv_calib3d2411d    -lopencv_video2411d    -lopencv_core2411d    -lopencv_highgui2411d   \
                                 -lopencv_features2d2411d -lopencv_flann2411d    -lopencv_imgproc2411d -lopencv_objdetect2411d \
                                 -lopencv_nonfree2411d    -lopencv_legacy2411d  #-lopencv_ml2411d

            OPENCV_2413_LIBS_R = -lopencv_calib3d2413     -lopencv_video2413     -lopencv_core2413     -lopencv_highgui2413   \
                                 -lopencv_features2d2413  -lopencv_flann2413     -lopencv_imgproc2413  -lopencv_objdetect2413 \
                                 -lopencv_nonfree2413     -lopencv_legacy2413   #-lopencv_ml2413

            OPENCV_2413_LIBS_D = -lopencv_calib3d2413d    -lopencv_video2413d    -lopencv_core2413d    -lopencv_highgui2413d   \
                                 -lopencv_features2d2413d -lopencv_flann2413d    -lopencv_imgproc2413d -lopencv_objdetect2413d \
                                 -lopencv_nonfree2413d    -lopencv_legacy2413d  #-lopencv_ml2413d

            OPENCV_310_LIBS_R  = -lopencv_calib3d310      -lopencv_video310      -lopencv_core310      -lopencv_highgui310   \
                                 -lopencv_features2d310   -lopencv_flann310      -lopencv_imgproc310   -lopencv_objdetect310 \
                                 -lopencv_imgcodecs310    -lopencv_videoio310   #-lopencv_ml310

            OPENCV_310_LIBS_D  = -lopencv_calib3d310d     -lopencv_video310d     -lopencv_core310d      -lopencv_highgui310d   \
                                 -lopencv_features2d310d  -lopencv_flann310d     -lopencv_imgproc310d   -lopencv_objdetect310d \
                                 -lopencv_imgcodecs310d   -lopencv_videoio310d  #-lopencv_ml310d


            exists($$OPENCV_PATH/build/bin/Release/opencv_gpu249.dll) { # add 2.4.9 cuda opencv libraries
                OPENCV_249_LIBS_R += -lopencv_gpu249
            }
            exists($$OPENCV_PATH/build/bin/Debug/opencv_gpu249d.dll) { # add 2.4.9 cuda opencv debug libraries
                OPENCV_249_LIBS_D += -lopencv_gpu249d
            }
            exists($$OPENCV_PATH/build/bin/Release/opencv_ocl249.dll) { # add 2.4.9 ocl opencv libraries
                OPENCV_249_LIBS_R += -lopencv_ocl249
            }
            exists($$OPENCV_PATH/build/bin/Debug/opencv_ocl249d.dll) { # add 2.4.9 ocl opencv debug libraries
                OPENCV_249_LIBS_D += -lopencv_ocl249d
            }

            exists($$OPENCV_PATH/build/bin/Release/opencv_gpu2411.dll) { # add 2.4.11 cuda opencv libraries
                OPENCV_2411_LIBS_R += -lopencv_gpu2411
            }
            exists($$OPENCV_PATH/build/bin/Debug/opencv_gpu2411d.dll) { # add 2.4.11 cuda opencv debug libraries
                OPENCV_2411_LIBS_D += -lopencv_gpu2411d
            }
            exists($$OPENCV_PATH/build/bin/Release/opencv_ocl2411.dll) { # add 2.4.11 ocl opencv libraries
                OPENCV_2411_LIBS_R += -lopencv_ocl2411
            }
            exists($$OPENCV_PATH/build/bin/Debug/opencv_ocl2411d.dll) { # add 2.4.11 ocl opencv debug libraries
                OPENCV_2411_LIBS_D += -lopencv_ocl2411d
            }

            exists($$OPENCV_PATH/build/bin/Release/opencv_gpu2413.dll) { # add 2.4.13 cuda opencv libraries
                OPENCV_2413_LIBS_R += -lopencv_gpu2413
            }
            exists($$OPENCV_PATH/build/bin/Debug/opencv_gpu2413d.dll) { # add 2.4.13 cuda opencv debug libraries
                OPENCV_2413_LIBS_D += -lopencv_gpu2413d
            }
            exists($$OPENCV_PATH/build/bin/Release/opencv_ocl2413.dll) { # add 2.4.13 ocl opencv libraries
                OPENCV_2413_LIBS_R += -lopencv_ocl2413
            }
            exists($$OPENCV_PATH/build/bin/Debug/opencv_ocl2413d.dll) { # add 2.4.13 ocl opencv debug libraries
                OPENCV_2413_LIBS_D += -lopencv_ocl2413d
            }


            exists($$OPENCV_PATH/build/bin/Release/opencv_cudafeatures2d310.dll) { # add 3.x cuda opencv libraries
                OPENCV_310_LIBS_R += -lopencv_cudafeatures2d310 -lopencv_cudafilters310   
            }
            exists($$OPENCV_PATH/build/bin/Debug/opencv_cudafeatures2d310d.dll) { # add 3.x cuda opencv debug libraries
                OPENCV_310_LIBS_D += -lopencv_cudafeatures2d310d -lopencv_cudafilters310d  
            }

            !isEmpty(OPENCV_CONTRIB_PATH) {
                OPENCV_310_LIBS_R += -lopencv_xfeatures2d310
                OPENCV_310_LIBS_D += -lopencv_xfeatures2d310d
            }


            CONFIG(debug, debug|release) {
                OPENCV_249_LIBS                = $$OPENCV_249_LIBS_D
                OPENCV_249_LIBS_ADD_OWN_BUILT  = -L$$OPENCV_PATH/build/lib/Debug/   $$OPENCV_249_LIBS

                OPENCV_2411_LIBS               = $$OPENCV_2411_LIBS_D
                OPENCV_2411_LIBS_ADD_OWN_BUILT = -L$$OPENCV_PATH/build/lib/Debug/   $$OPENCV_2411_LIBS

                OPENCV_2413_LIBS               = $$OPENCV_2413_LIBS_D
                OPENCV_2413_LIBS_ADD_OWN_BUILT = -L$$OPENCV_PATH/build/lib/Debug/   $$OPENCV_2413_LIBS

                OPENCV_310_LIBS                = $$OPENCV_310_LIBS_D
                OPENCV_310_LIBS_ADD_OWN_BUILT  = -L$$OPENCV_PATH/build/lib/Debug/   $$OPENCV_310_LIBS
            }
            CONFIG(release, debug|release) {
                OPENCV_249_LIBS                = $$OPENCV_249_LIBS_R
                OPENCV_249_LIBS_ADD_OWN_BUILT  = -L$$OPENCV_PATH/build/lib/Release/ $$OPENCV_249_LIBS

                OPENCV_2411_LIBS               = $$OPENCV_2411_LIBS_R
                OPENCV_2411_LIBS_ADD_OWN_BUILT = -L$$OPENCV_PATH/build/lib/Release/ $$OPENCV_2411_LIBS

                OPENCV_2413_LIBS               = $$OPENCV_2413_LIBS_R
                OPENCV_2413_LIBS_ADD_OWN_BUILT = -L$$OPENCV_PATH/build/lib/Release/ $$OPENCV_2413_LIBS

                OPENCV_310_LIBS                = $$OPENCV_310_LIBS_R
                OPENCV_310_LIBS_ADD_OWN_BUILT  = -L$$OPENCV_PATH/build/lib/Release/ $$OPENCV_310_LIBS
            }


            exists($$OPENCV_PATH/build/bin/Release/opencv_core249.dll): win32-msvc* {               # git's OpenCV tag=2.4.9 built by any own MSVC
                !build_pass:message(Using <$$OPENCV_PATH/build/bin/Release|Debug>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS        += $$OPENCV_249_LIBS_ADD_OWN_BUILT
                DEFINES     += WITH_OPENCV
                exists($$OPENCV_PATH/build/bin/Release/opencv_gpu249.dll) {
                    DEFINES     += WITH_OPENCV_GPU
                }
            } else:exists($$OPENCV_PATH/build/bin/Release/opencv_core2413.dll): win32-msvc* {    # git's OpenCV tag=2.4.13 built by any own MSVC
                !build_pass:message(Using <$$OPENCV_PATH/build/bin/Release|Debug>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS        += $$OPENCV_2413_LIBS_ADD_OWN_BUILT
                DEFINES     += WITH_OPENCV
                exists($$OPENCV_PATH/build/bin/Release/opencv_gpu2413.dll) {
                    DEFINES     += WITH_OPENCV_GPU
                }
            } else:exists($$OPENCV_PATH/build/bin/Release/opencv_core310.dll): win32-msvc* {    # git's OpenCV tag=3.1.0 built by any own MSVC
                !build_pass:message(Using <$$OPENCV_PATH/build/bin/Release|Debug>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS        += $$OPENCV_310_LIBS_ADD_OWN_BUILT
                DEFINES     += WITH_OPENCV WITH_OPENCV_3x
                exists($$OPENCV_PATH/build/bin/Release/opencv_cudafeatures2d310.dll) {
                    DEFINES     += WITH_OPENCV_GPU
                }
            } else:exists($$OPENCV_PATH/build/x86/vc10/bin/opencv_core249.dll):equals(QMAKE_TARGET.arch, "x86"): win32-msvc2010 {    # built OpenCV v.2.4.9 with msvc10 without GPU
                !build_pass:message(Using <$$OPENCV_PATH/build/x86/vc10/bin>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS        += -L$$OPENCV_PATH/build/x86/vc10/lib/ $$OPENCV_249_LIBS
                DEFINES     += WITH_OPENCV
            } else:exists($$OPENCV_PATH/build/x64/vc10/bin/opencv_core249.dll): win32-msvc2010 {    # built OpenCV v.2.4.9 with msvc10 without GPU
                !build_pass:message(Using <$$OPENCV_PATH/build/x64/vc10/bin>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS        += -L$$OPENCV_PATH/build/x64/vc10/lib/ $$OPENCV_249_LIBS
                DEFINES     += WITH_OPENCV
            } else:exists($$OPENCV_PATH/build/x86/vc12/bin/opencv_core249.dll):equals(QMAKE_TARGET.arch, "x86"): win32-msvc2013 {    # built OpenCV v.2.4.9 with vc12 without GPU
                !build_pass:message(Using <$$OPENCV_PATH/build/x86/vc12/bin>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS        += -L$$OPENCV_PATH/build/x86/vc12/lib/ $$OPENCV_249_LIBS
                DEFINES     += WITH_OPENCV
            } else:exists($$OPENCV_PATH/build/x64/vc12/bin/opencv_core249.dll): win32-msvc2013 {    # built OpenCV v.2.4.9 with vc12 without GPU
                !build_pass:message(Using <$$OPENCV_PATH/build/x64/vc12/bin>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS        += -L$$OPENCV_PATH/build/x64/vc12/lib/ $$OPENCV_249_LIBS
                DEFINES     += WITH_OPENCV
            } else:exists($$OPENCV_PATH/build/x86/vc12/bin/opencv_core249.dll):equals(QMAKE_TARGET.arch, "x86"): win32-msvc2015 {    # built OpenCV v.2.4.9 with vc12 without GPU
                !build_pass:message(Using <$$OPENCV_PATH/build/x86/vc12/bin>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS        += -L$$OPENCV_PATH/build/x86/vc12/lib/ $$OPENCV_249_LIBS
                DEFINES     += WITH_OPENCV
            } else:exists($$OPENCV_PATH/build/x64/vc12/bin/opencv_core249.dll): win32-msvc2015 {    # built OpenCV v.2.4.9 with vc12 without GPU
                !build_pass:message(Using <$$OPENCV_PATH/build/x64/vc12/bin>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS        += -L$$OPENCV_PATH/build/x64/vc12/lib/ $$OPENCV_249_LIBS
                DEFINES     += WITH_OPENCV
            } else:exists($$OPENCV_PATH/x86/vc12/bin/opencv_core249.dll):equals(QMAKE_TARGET.arch, "x86"): win32-msvc* {             # installed OpenCV v.2.4.9 with msvc* without GPU (our integration server)
                !build_pass:message(Using <$$OPENCV_PATH/x86/vc12/bin>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS        += -L$$OPENCV_PATH/x86/vc12/lib/ $$OPENCV_249_LIBS
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
            } else:exists($$OPENCV_PATH/build/x86/vc12/bin/opencv_core2411.dll):equals(QMAKE_TARGET.arch, "x86"): win32-msvc2013 {   # OpenCV tag=2.4.11 built by vc12 without GPU
                !build_pass:message(Using <$$OPENCV_PATH/build/x86/vc12/bin>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS += -L$$OPENCV_PATH/build/x86/vc12/lib/ $$OPENCV_2411_LIBS
                DEFINES += WITH_OPENCV
            } else:exists($$OPENCV_PATH/build/x64/vc12/bin/opencv_core2411.dll): win32-msvc2013 {   # OpenCV tag=2.4.11 built by vc12 without GPU
                !build_pass:message(Using <$$OPENCV_PATH/build/x64/vc12/bin>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS += -L$$OPENCV_PATH/build/x64/vc12/lib/ $$OPENCV_2411_LIBS
                DEFINES += WITH_OPENCV
            } else:exists($$OPENCV_PATH/build/x86/vc14/bin/opencv_core2411.dll):equals(QMAKE_TARGET.arch, "x86"): win32-msvc2015 {   # OpenCV tag=2.4.11 built by vc14 without GPU
                !build_pass:message(Using <$$OPENCV_PATH/build/x86/vc14/bin>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS += -L$$OPENCV_PATH/build/x86/vc14/lib/ $$OPENCV_2411_LIBS
                DEFINES += WITH_OPENCV
            } else:exists($$OPENCV_PATH/build/x64/vc14/bin/opencv_core2411.dll): win32-msvc2015 {   # OpenCV tag=2.4.11 built by vc14 without GPU
                !build_pass:message(Using <$$OPENCV_PATH/build/x64/vc14/bin>)
                INCLUDEPATH += $$OPENCV_INC_NOTINSTALLED
                LIBS += -L$$OPENCV_PATH/build/x64/vc14/lib/ $$OPENCV_2411_LIBS
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

        with_nopkgconfig {

            LIBS += -lopencv_calib3d    -lopencv_video   -lopencv_core    -lopencv_highgui   \
                    -lopencv_features2d -lopencv_flann   -lopencv_imgproc -lopencv_objdetect \
                    -lopencv_legacy #-llibopencv_ml

           # LIBS += -lopencv_nonfree

            message(Forcing OpenCV 3.x)
            DEFINES += WITH_OPENCV_3x
        } else {

            CONFIG += link_pkgconfig
            PKGCONFIG += opencv
            OPENCV_LIBS = $$system(pkg-config --libs opencv)
            system(pkg-config --atleast-version=3.0 opencv) {
                message(Detected OpenCV 3.x)
                DEFINES += WITH_OPENCV_3x
                OPENCV_CONTRIB_LIBS = $$find(OPENCV_LIBS, opencv_xfeatures2d)
                OPENCV_GPU_LIBS     = $$find(OPENCV_LIBS, opencv_cuda)
                !isEmpty(OPENCV_CONTRIB_LIBS) {
                    message(Detected OpenCV contrib module)
                }
            } else {
                OPENCV_GPU_LIBS     = $$find(OPENCV_LIBS, opencv_gpu)
            }
            !isEmpty(OPENCV_GPU_LIBS) {
                message(Detected OpenCV GPU module)
                DEFINES += WITH_OPENCV_GPU
            }
	}
 
        #LIBS += -lopencv_contrib -lopencv_ts -lopencv_videostab -lopencv_photo -lopencv_gpu -lopencv_ocl -lopencv_stitching -lopencv_superres
 
        DEFINES += WITH_OPENCV
    }
}
