# Standard part for any project that tunes some specific parameters that depend of the set config before by the config.pri
#

#CONFIG += restricted_compile_dynamic
#CONFIG += restricted_use_dynamic

#QMAKE_AR = ar cq

# ------------------------------------------

BUILD_CFG_SFX = ""                                  # debug and release libs/exes will be overwritten on !win32-msvc* config as we use one output folder
build_pass:CONFIG(debug, debug|release) {
                 DEFINES +=  DEBUG
    win32-msvc*: DEFINES += _DEBUG                  # for msvc such a define is commonly used
    win32-msvc*: BUILD_CFG_SFX = "d"                # only for msvc as QtCreator doesn't understand the app with suffix
    win32-msvc* {
        BUILD_CFG_NAME = /debug
    } else:win32 {
        BUILD_CFG_NAME = /debug-mg
    } else {
        BUILD_CFG_NAME = ""                         # debug and release objs will be overwritten on !win32 config!
    }
}
build_pass:CONFIG(release, debug|release) {
                 DEFINES +=  NDEBUG
    win32-msvc* {
        BUILD_CFG_NAME = /release
    } else:win32 {
        BUILD_CFG_NAME = /release-mg
    } else {
        BUILD_CFG_NAME = ""                         # debug and release objs will be overwritten on !win32 config!
    }
}

CONFIG += c++11

# TODO: this info is needed before - at config.pri!
#contains(QMAKE_HOST.arch, "armv7l") {
#    message(Odroid platform with ARM detected)
#    CONFIG += odroid
#} else {
#    #message(standard x86/64 platform detected)
#}

trace {
    DEFINES += TRACE
}

asserts {
    DEFINES += ASSERTS
}

with_avx {
    DEFINES += WITH_AVX
    !win32-msvc* {
        QMAKE_CFLAGS   += -mavx
        QMAKE_CXXFLAGS += -mavx
    } else {
        QMAKE_CFLAGS   += $$QMAKE_CFLAGS_AVX        # Qmake uses it as "-arch:AVX" for msvc >= VS-2010
        QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_AVX

        #!build_pass: message(DEFINES = $$DEFINES)
    }
}

with_avx2 {
    DEFINES += WITH_AVX2
    !win32-msvc* {
        QMAKE_CFLAGS   += -mavx2
        QMAKE_CXXFLAGS += -mavx2
    } else {
        QMAKE_CFLAGS   += $$QMAKE_CFLAGS_AVX2        # Qmake uses it as "-arch:AVX2" for msvc >= VS-2015
        QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_AVX2
    }
}

with_fma {
    DEFINES += WITH_FMA
    !win32-msvc* {
        QMAKE_CFLAGS   += -mfma
        QMAKE_CXXFLAGS += -mfma
    } else {
        QMAKE_CFLAGS   += /arch:FMA
        QMAKE_CXXFLAGS += /arch:FMA
    }
}

with_sse {
    DEFINES += WITH_SSE
    !win32-msvc* {
        QMAKE_CFLAGS   += -msse2
        QMAKE_CXXFLAGS += -msse2
    } else {
#       QMAKE_CFLAGS   += -arch:SSE2     # actual only for x86 mode
#       QMAKE_CXXFLAGS += -arch:SSE2
    }
}
with_sse3 {
    DEFINES += WITH_SSE3
    !win32-msvc* {
        QMAKE_CFLAGS   += -msse3 -mssse3
        QMAKE_CXXFLAGS += -msse3 -mssse3
    } else {
#       DEFINES -= WITH_SSE3
    }
}

with_sse4 {
    DEFINES += WITH_SSE4
    !win32-msvc* {
        QMAKE_CFLAGS   += -msse4.1
        QMAKE_CXXFLAGS += -msse4.1
    } else {
#       DEFINES -= WITH_SSE4
    }
}

with_neon {
    QMAKE_CXXFLAGS += -mfloat-abi=softfp -mfpu=neon
    DEFINES += WITH_NEON
}

win32 {
    DEFINES += WIN32
    DEFINES += USE_GTS
}
macx {
    DEFINES += MACX
}

gcc_env_toolchain {
  GCC_POSTFIX = $$(GCC_POSTFIX)
  message(Compiling with gcc-$$GCC_POSTFIX)

  QMAKE_CC = gcc-$$GCC_POSTFIX
  QMAKE_CXX = g++-$$GCC_POSTFIX
  QMAKE_LINK =  g++-$$GCC_POSTFIX
  QMAKE_LINK_SHLIB =  g++-$$GCC_POSTFIX
  QMAKE_LINK_C = gcc-$$GCC_POSTFIX
  QMAKE_LINK_C_SHLIB = gcc-$$GCC_POSTFIX
}

clang_toolchain {
  CLANG_POSTFIX="-3.6"

  CONFIG -= warn_on

  QMAKE_CC = clang$$CLANG_POSTFIX
  QMAKE_CXX = clang++$$CLANG_POSTFIX
  QMAKE_LINK =  clang++$$CLANG_POSTFIX
  QMAKE_LINK_SHLIB =  clang
  QMAKE_LINK_C = clang
  QMAKE_LINK_C_SHLIB = clang

#  QMAKE_CFLAGS += -stdlib=libc++
#  QMAKE_CXXFLAGS += -stdlib=libc++


   QMAKE_CFLAGS += -Wall -Wno-inconsistent-missing-override
   QMAKE_CFLAGS += -Wall -Wno-overloaded-virtual
   QMAKE_CXXFLAGS += -Wall -Wno-inconsistent-missing-override
   QMAKE_CXXFLAGS += -Wall -Wno-overloaded-virtual

#   QMAKE_CFLAGS_WARN_OFF   -=  -Wall
#   QMAKE_CXXFLAGS_WARN_OFF -=  -Wall
}
gcc48_toolchain {
  QMAKE_CC = gcc-4.8
  QMAKE_CXX = g++-4.8
  QMAKE_LINK =  g++-4.8
  QMAKE_LINK_SHLIB =  g++-4.8
  QMAKE_LINK_C = gcc-4.8
  QMAKE_LINK_C_SHLIB = gcc-4.8

  # Uncomment this when qt headers are fixed
  QMAKE_CFLAGS += -Wno-unused-local-typedefs
  QMAKE_CXXFLAGS += -Wno-unused-local-typedefs

  gcc_lto {
     QMAKE_CFLAGS_RELEASE += -flto
     QMAKE_CXXFLAGS_RELEASE += -flto
     QMAKE_LFLAGS += -flto
  }
}

gcc_checker {
     QMAKE_CFLAGS   += -fsanitize=address -fno-omit-frame-pointer
     QMAKE_CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
     QMAKE_LFLAGS   += -fsanitize=address -fno-omit-frame-pointer

# QMAKE_CFLAGS   += -fsanitize=undefined
# QMAKE_CXXFLAGS += -fsanitize=undefined
# QMAKE_LFLAGS   += -fsanitize=undefined
}


arm_toolchain {
    QMAKE_CC = arm-none-linux-gnueabi-gcc
    QMAKE_CXX = arm-none-linux-gnueabi-g++
    QMAKE_LINK = arm-none-linux-gnueabi-g++
    QMAKE_LINK_SHLIB = arm-none-linux-gnueabi-g++
    QMAKE_LINK_C = arm-none-linux-gnueabi-gcc
    QMAKE_LINK_C_SHLIB = arm-none-linux-gnueabi-gcc
    QMAKE_INCDIR_QT =
    QMAKE_LIBDIR_QT =
}

icc_toolchain {
    SUPRESSED_REMERKS = -wd981 -wd383 -wd82 -wd1782

#  -gcc-version=default
    QMAKE_CFLAGS_DEBUG = $$SUPRESSED_REMERKS
    QMAKE_CXXFLAGS_DEBUG = $$SUPRESSED_REMERKS
    QMAKE_CFLAGS_RELEASE = $$SUPRESSED_REMERKS
    QMAKE_CXXFLAGS_RELEASE = $$SUPRESSED_REMERKS
    QMAKE_LFLAGS = ""

    QMAKE_CC = icc
    QMAKE_CXX = icc
    QMAKE_LINK = icc
    QMAKE_LINK_SHLIB = icc
    QMAKE_LINK_C = icc
    QMAKE_LINK_C_SHLIB = icc

}

isEmpty(CCACHE_TOOLCHAIN_ON) {
  ccache_toolchain {
    QMAKE_CC = ccache $$QMAKE_CC
    QMAKE_CXX = ccache $$QMAKE_CXX
    QMAKE_LINK = ccache $$QMAKE_LINK
    QMAKE_LINK_SHLIB = ccache $$QMAKE_SHLIB
    QMAKE_LINK_C = ccache $$QMAKE_LINK_C
    QMAKE_LINK_C_SHLIB = ccache $$QMAKE_LINK_SHLIB
    CCACHE_TOOLCHAIN_ON = true
  }
}

!win32-msvc* {
    QMAKE_CFLAGS_DEBUG     -= -g
    QMAKE_CXXFLAGS_DEBUG   -= -g
    QMAKE_LFLAGS           -= -g
    QMAKE_CFLAGS_DEBUG     += -O0 -g3
    QMAKE_CXXFLAGS_DEBUG   += -O0 -g3
    QMAKE_LFLAGS           +=     -g3

    QMAKE_CFLAGS_RELEASE   += -O3
    QMAKE_CXXFLAGS_RELEASE += -O3
   QMAKE_CFLAGS_RELEASE   += -g3
   QMAKE_CXXFLAGS_RELEASE += -g3
#   QMAKE_CFLAGS_RELEASE   += -mtune=native     # TODO: native doesn't work while we could use (SSE & !AVX)
#   QMAKE_CXXFLAGS_RELEASE += -mtune=native     # TODO: native doesn't work while we could use (SSE & !AVX)

    # Workaround for -fPIC bug
    QMAKE_CFLAGS_STATIC_LIB=
    QMAKE_CXXFLAGS_STATIC_LIB=

    !win32 {
        QMAKE_CFLAGS   +=-fPIC
        QMAKE_CXXFLAGS +=-fPIC
    }
} else {
   #QMAKE_CXXFLAGS_DEBUG   += /showIncludes

    QMAKE_CFLAGS_DEBUG     += -Od
    QMAKE_CXXFLAGS_DEBUG   += -Od
    QMAKE_CFLAGS_RELEASE   += /Ox
    QMAKE_CXXFLAGS_RELEASE += /Ox

    # Microsoft is too restrictive
    #
    DEFINES        += _CRT_SECURE_NO_WARNINGS
    QMAKE_CFLAGS   += /wd4800 /wd4244
    QMAKE_CXXFLAGS += /wd4800 /wd4244
    QMAKE_LFLAGS   += /ignore:4221
    QMAKE_LIBFLAGS += /ignore:4221

    # Support parallel compiling
    #
    QMAKE_CFLAGS   += /MP
    QMAKE_CXXFLAGS += /MP

    # We keep all pdb files at intermediate directories
    #
    win32-msvc2013 {
        # Since [Qt5.5.0 + msvc2013] pdb management is added automatically into bin folder
    } else {
        gen_vsproj {
            QMAKE_CXXFLAGS += -Fd"$(IntDir)"
            QMAKE_LFLAGS   += /PDB:"$(IntDir)\\$(TargetName).pdb"
        } else {
            QMAKE_CXXFLAGS += -Fd"$(OBJECTS_DIR)"
            QMAKE_LFLAGS   += /PDB:"$(OBJECTS_DIR)\\$(QMAKE_TARGET).pdb"
        }
    }
}


# Workaround for found bug of QMake:
# On the 'distclean' command tree recursively generated makefiles (and object_script as well) haven't been deleted.
# It happens when a project has the same name as its directory.
#
build_pass {                                      # must clean only for the concrete configuration
  !gen_vsproj {                                   # QMake doesn't support clean command for generated VS projects
    CONFIG(debug, debug|release) {
        QMAKE_DISTCLEAN += object_script.*        # delete these files always for each project even if they are absent
    }
    CONFIG(release, debug|release) {
        QMAKE_DISTCLEAN += object_script.*        # delete these files always for each project even if they are absent
    }
#   QMAKE_DISTCLEAN += object_script.*            # commented as it's repeated twice when open (common|release)

#   QMAKE_DISTCLEAN += Makefile.$(QMAKE_TARGET)*  # doesn't work as we make different target name with the project name
#   QMAKE_DISTCLEAN += Makefile.$(TARGET)*        # doesn't work as make's target has an output filename, not the project name!
#   QMAKE_DISTCLEAN += Makefile.*                 # doesn't work as it kills both makefiles and the cleaning is stopped!

    # We have to check those projects, which have the same name as their folders!!!
    #
    contains(TARGET, directShow) {
        QMAKE_DISTCLEAN += Makefile.directShow*
    }
    contains(TARGET, cvs_core) {
        QMAKE_DISTCLEAN += Makefile.core*
    }
    contains(TARGET, cvs_utils) {
        QMAKE_DISTCLEAN += Makefile.utils*
    }
    contains(TARGET, recorder) {
        QMAKE_DISTCLEAN += Makefile.recorder*
    }
    contains(TARGET, test_opencl) {
        QMAKE_DISTCLEAN += Makefile.opencl*
    }
    contains(OBJ_TESTS_DIRNAME, tests) {    # TARGET doesn't work as it has a name of each test!
       #QMAKE_DISTCLEAN += Makefile*        # doesn't work as it tries to delete Makefile.unitTests.Debug/Release that are really used on distclean cmd!
        QMAKE_DISTCLEAN += Makefile Makefile.Debug Makefile.Release     # these files are generated indeed!
    }
  } else {
    # unfortunately QMake doesn't support clean command inside generated VS projects
  }
}

msvc_analyser {
}


arm_toolchain {
    QMAKE_CFLAGS_RELEASE += -O3
    QMAKE_CXXFLAGS_RELEASE += -O3
    QT -= core gui
}

clang_toolchain {
#    QMAKE_CFLAGS_RELEASE += -march=native
#    QMAKE_CXXFLAGS_RELEASE += -march=native
    QMAKE_CFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE -= -O2
}



clang_analyser {
   QMAKE_CFLAGS_RELEASE += --analyze
   QMAKE_CXXFLAGS_RELEASE += --analyze
}

#!win32:!clang_toolchain {
#    QMAKE_CFLAGS += -rdynamic
#    QMAKE_CXXFLAGS += -rdynamic
#    QMAKE_LFLAGS += -rdynamic
#}

profile_prepare {
    QMAKE_CFLAGS_RELEASE += -pg -fprofile-generate
    QMAKE_CXXFLAGS_RELEASE += -pg -fprofile-generate
    QMAKE_LFLAGS += -fprofile-generate
}

profile_use {
    QMAKE_CFLAGS_RELEASE += -fprofile-use
    QMAKE_CXXFLAGS_RELEASE += -fprofile-use
    QMAKE_LFLAGS += -fprofile-use

    with_tbb {
        QMAKE_CFLAGS_RELEASE += -fprofile-correction
        QMAKE_CXXFLAGS_RELEASE += -fprofile-correction
        QMAKE_LFLAGS += -fprofile-correction
    }
}

pedantic_build {
    QMAKE_CFLAGS += -pedantic
    QMAKE_CXXFLAGS += -pedantic
}

gcov {
    QMAKE_CFLAGS   += -fprofile-arcs -ftest-coverage
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS   += -fprofile-arcs -ftest-coverage
}

with_tbb:!contains(DEFINES, WITH_TBB) {
    TBB_PATH = $$(TBB_PATH)
    win32 {
        !isEmpty(TBB_PATH) {
            DEFINES += WITH_TBB

            win32-msvc*:!contains(QMAKE_HOST.arch, x86_64) {
                TBB_LIBDIR = $(TBB_PATH)/lib/ia32/vc10
            } else:win32-msvc2010 {
                TBB_LIBDIR = $(TBB_PATH)/lib/intel64/vc10
            } else:win32-msvc* {
                TBB_LIBDIR = $(TBB_PATH)/lib/intel64/vc12
            } else:exists($(TBB_PATH)/lib/tbb.dll) {
                # old config when TBB's bins&libs were placed at TBB's lib dir
                TBB_LIBDIR = $(TBB_PATH)/lib
            } else {
                GCC_VER    = $$system(gcc -dumpversion)
                TBB_LIBDIR = $(TBB_PATH)/lib/intel64/mingw$$GCC_VER
                # the "script/windows/.tbb_build_mingw.bat" places libs there
            }
            INCLUDEPATH += $(TBB_PATH)/include
            LIBS        += -L"$$TBB_LIBDIR" -ltbb
            !build_pass: contains(TARGET, cvs_core): message(Using <$$TBB_LIBDIR>)
        } else {
           !build_pass: message(TBB not found. Please set TBB_PATH system variable to a root folder of TBB to use it)
        }
    } else:macx {
        #message (Using TBB at $$TBB_PATH)
        DEFINES     += WITH_TBB
        INCLUDEPATH += $$TBB_PATH/include
        LIBS        += -L$$TBB_PATH/lib -ltbb

        DEPENDPATH  += $$TBB_PATH/include
    } else {
        !isEmpty(TBB_PATH) {
            #message (Using TBB at $$TBB_PATH)
            INCLUDEPATH += $$TBB_PATH/include
            LIBS        += -L$$TBB_PATH/lib/
        }
        else {
            !build_pass: message (Using System TBB)
        }
        DEFINES     += WITH_TBB
        LIBS        += -ltbb
    }
}

#
# MKL is more preferable as it uses "tbb" internally, which we use too anyway.
# But openBLAS uses an "OpenMP" that is bad to use with tbb simultaneously, nevertheless you can switch off tbb as well.
# Therefore we support MKL with tbb threading and also MKL with openMP threading model on Windows/Linux platforms.
# For more detailed MKL's linker options, see "https://software.intel.com/en-us/articles/intel-mkl-link-line-advisor".
#
with_mkl {
    MKLROOT = $$(MKLROOT)
    !win32: isEmpty(MKLROOT) {
        MKLROOT = /opt/intel/mkl
    }
    exists("$$MKLROOT"/include/mkl.h) {
        !win32 {
            LIBS        += -L"$$MKLROOT"/lib/intel64 -lmkl_intel_lp64 -lmkl_core
            with_tbb {
                LIBS    += -lmkl_tbb_thread -lstdc++ -lpthread -lm      # -ltbb was already included above
            } else {
                LIBS    += -lmkl_gnu_thread -ldl -lpthread -lm          # with OpenMP's threading layer, GNU's OpenMP library (libgomp)
            }
        } else {
            LIBS        += -L"$$MKLROOT"/lib/intel64 -lmkl_intel_lp64_dll -lmkl_core_dll
            with_tbb {
                LIBS    += -lmkl_tbb_thread_dll                         # -ltbb was already included above
            } else {
                LIBS    += -lmkl_intel_thread_dll -llibiomp5md          # with OpenMP's threading layer, Intel's OpenMP library (libiomp5)
            }
        }
        INCLUDEPATH += "$$MKLROOT"/include
        DEFINES     += WITH_BLAS
        DEFINES     += WITH_MKL
    }
    else {
        !build_pass: message (requested MKL is not installed and is deactivated)
    }
    CONFIG += with_blas
}

with_openblas {
    contains(DEFINES, WITH_MKL) {
        !build_pass: contains(TARGET, core): message(openBLAS is deactivated as detected MKL was activated)
    }
    else:!win32 {
        BLAS_PATH = $$(BLAS_PATH)
        !isEmpty(BLAS_PATH) {
            exists("$$BLAS_PATH"/include/cblas.h) {
                !build_pass: message(Using BLAS from <$$BLAS_PATH>)
                INCLUDEPATH += $(BLAS_PATH)/include
                LIBS        += -lopenblas
                DEFINES     += WITH_BLAS
            }
            else {
                !build_pass: message(requested openBLAS via BLAS_PATH is not found and is deactivated)
            }
        } else {
            exists(/usr/include/cblas.h) {
                !build_pass: message (Using System BLAS)
                LIBS        += -lopenblas -llapacke
                DEFINES     += WITH_BLAS
            }
            else {
                !build_pass: message(requested system BLAS is not found and is deactivated)
            }
        }
    } else {
        !build_pass: message(requested openBLAS is not supported for Win and is deactivated)
    }
    CONFIG += with_blas
}

with_fftw {
    contains(DEFINES, WITH_MKL) {
        !build_pass: contains(TARGET, core): message(Using FFTW from MKL)
        INCLUDEPATH += "$$MKLROOT"/include/fftw
        DEFINES     += WITH_FFTW
    }
    else:!win32 {
        FFTW_PATH = $$(FFTW_PATH)
        isEmpty(FFTW_PATH) {
            FFTW_PREFIX=/usr
        } else {
            FFTW_PREFIX=$(FFTW_PATH)
        }
        exists("$$FFTW_PREFIX"/include/fftw3.h) {
            !build_pass: message(Using FFTW from <$$FFTW_PREFIX>)
            INCLUDEPATH += $(FFTW_PREFIX)/include
            LIBS        += -lfftw3
            DEFINES     += WITH_FFTW
        }
        else {
            !build_pass: message(requested FFTW is not found and is deactivated)
        }
    }
    else {
        !build_pass: message(requested separated FFTW is not supported for Win and is deactivated)
    }
}

# More static analysis warnings
# QMAKE_CXXFLAGS += -Wextra
# QMAKE_CXXFLAGS += -Woverloaded-virtual
# QMAKE_CXXFLAGS += -Wnon-virtual-dtor
# QMAKE_CXXFLAGS += -Wold-style-cast
# QMAKE_CXXFLAGS += -Wconversion
# QMAKE_CXXFLAGS += -Wsign-conversion
# QMAKE_CXXFLAGS += -Winit-self
# QMAKE_CXXFLAGS += -Wunreachable-code

#OPEN_ROOT_DIRECTORY = $$PWD

DISTFILES += \
    $$PWD/arduinoscancontrol.ino
