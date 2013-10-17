# Standard part for any project that tunes some specific parameters that depend of the set config before by the config.pri
#

#CONFIG += restricted_compile_dynamic
#CONFIG += restricted_use_dynamic

#QMAKE_AR = ar cq

# ------------------------------------------

BUILD_CFG_SFX = ""
build_pass:CONFIG(debug, debug|release) {
                DEFINES +=  DEBUG
    win32-msvc*:DEFINES += _DEBUG                   # for msvc such a define is commonly used
    BUILD_CFG_NAME = debug

   #win32:       BUILD_CFG_SFX = "d"                # append suffix to the target only for win32 config for a while...
    win32-msvc*: BUILD_CFG_SFX = "d"                # only for msvc as QtCreator doesn't understand the app with suffix
}
build_pass:CONFIG(release, debug|release) {
    BUILD_CFG_NAME = release
}

trace {
    DEFINES += TRACE
}

asserts {
    DEFINES += ASSERTS
}

with_avx {
    QMAKE_CFLAGS   += -mavx
    QMAKE_CXXFLAGS += -mavx
    DEFINES        += WITH_AVX
}

with_sse {
    DEFINES += WITH_SSE

    !win32-msvc* {
        QMAKE_CFLAGS   += -msse2
        QMAKE_CXXFLAGS += -msse2
    } else {
        QMAKE_CFLAGS   += /arch:SSE2
        QMAKE_CXXFLAGS += /arch:SSE2
    }
}
with_sse3 {
    DEFINES += WITH_SSE3

    !win32-msvc* {
        QMAKE_CFLAGS   += -msse3
        QMAKE_CXXFLAGS += -msse3
    } else !win32-msvc2008 {
        QMAKE_CFLAGS   += /arch:SSE3
        QMAKE_CXXFLAGS += /arch:SSE3
    } else {
        DEFINES -= WITH_SSE3
    }
}
with_sse4 {
    DEFINES += WITH_SSE4

    !win32-msvc* {
        QMAKE_CFLAGS   += -msse4.1
        QMAKE_CXXFLAGS += -msse4.1
    } else !win32-msvc2008 {
        QMAKE_CFLAGS   += /arch:SSE4.1
        QMAKE_CXXFLAGS += /arch:SSE4.1
    } else {
        DEFINES -= WITH_SSE4
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

gcc45_toolchain {
  QMAKE_CC = gcc-4.5
  QMAKE_CXX = g++-4.5
  QMAKE_LINK =  g++-4.5
  QMAKE_LINK_SHLIB =  g++-4.5
  QMAKE_LINK_C = gcc-4.5
  QMAKE_LINK_C_SHLIB = gcc-4.5
}

clang_toolchain {
  QMAKE_CC = clang
  QMAKE_CXX = clang
  QMAKE_LINK =  clang
  QMAKE_LINK_SHLIB =  clang
  QMAKE_LINK_C = clang
  QMAKE_LINK_C_SHLIB = clang
}

gcc47_toolchain {
  QMAKE_CC = gcc-4.7
  QMAKE_CXX = g++-4.7
  QMAKE_LINK =  g++-4.7
  QMAKE_LINK_SHLIB =  g++-4.7
  QMAKE_LINK_C = gcc-4.7
  QMAKE_LINK_C_SHLIB = gcc-4.7
  gcc_lto {
     QMAKE_CFLAGS_RELEASE += -flto
     QMAKE_CXXFLAGS_RELEASE += -flto
     QMAKE_LFLAGS += -flto
  }
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
  gcc_checker {
     QMAKE_CFLAGS += -fsanitize=address -fno-omit-frame-pointer
     QMAKE_CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
     QMAKE_LFLAGS += -fsanitize=address -fno-omit-frame-pointer
  }
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

    QMAKE_CFLAGS_RELEASE   += -O3 -g3 -mtune=native
    QMAKE_CXXFLAGS_RELEASE += -O3 -g3 -mtune=native
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

    # Support parallel compiling
    #
    QMAKE_CFLAGS   += /MP
    QMAKE_CXXFLAGS += /MP

    # We keep all pdb files at intermediate directories
    #
    gen_vsproj {
        QMAKE_CXXFLAGS += -Fd"$(IntDir)"
        QMAKE_LFLAGS   += /PDB:"$(IntDir)\\$(TargetName).pdb"
    } else {
        QMAKE_CXXFLAGS += -Fd"$(OBJECTS_DIR)"
        QMAKE_LFLAGS   += /PDB:"$(OBJECTS_DIR)\\$(QMAKE_TARGET).pdb"
    }
}


# Workaround for found bug of QMake:
# On the 'distclean' command tree recursively generated makefiles (and object_script as well) haven't been deleted.
# It happens when a project has the same name as its directory.
#
build_pass :                          # must clean only for the concrete configuration
!gen_vsproj {                                       # QMake doesn't support clean command for generated VS projects
    CONFIG(debug, debug|release) {
        QMAKE_DISTCLEAN += object_script.*        # delete these files always for each project even if they are absent
    }
    CONFIG(release, debug|release) {
        QMAKE_DISTCLEAN += object_script.*        # delete these files always for each project even if they are absent
    }
#   QMAKE_DISTCLEAN += object_script.*        # commented as it's repeated twice when open (common|release)

#   QMAKE_DISTCLEAN += Makefile.$(QMAKE_TARGET)*  # doesn't work as we make different target name with the project name
#   QMAKE_DISTCLEAN += Makefile.$(TARGET)*      # doesn't work as make's target has an output filename, not the project name!
#   QMAKE_DISTCLEAN += Makefile.*         # doesn't work as it kills both makefiles and the cleaning is stopped!

    # We have to check those projects, which have the same name as their folders!!!
    #
    contains(TARGET, directShow) {
        QMAKE_DISTCLEAN += Makefile.directShow*
    }
    contains(TARGET, core) {
        QMAKE_DISTCLEAN += Makefile.core*
    }
    contains(TARGET, utils) {
        QMAKE_DISTCLEAN += Makefile.utils*
    }
    contains(TARGET, recorder) {
        QMAKE_DISTCLEAN += Makefile.recorder*
    }
    contains(TARGET, opencl) {
        QMAKE_DISTCLEAN += Makefile.opencl*
    }
    contains(OBJ_TESTS_DIR, tests) {        # TARGET doesn't work as it has a name of each test!
       #QMAKE_DISTCLEAN += Makefile*        # doesn't work as it tries to delete Makefile.unitTests.Debug/Release that're really used on distclean cmd!
        QMAKE_DISTCLEAN += Makefile Makefile.Debug Makefile.Release     # these files are generated indeed!
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
            } else:win32-msvc* {
                TBB_LIBDIR = $(TBB_PATH)/lib/intel64/vc10
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
            contains(TARGET, core): !build_pass: message(Using <$$TBB_LIBDIR>)
        } else {
           !build_pass:message(TBB not found. Please set TBB_PATH system variable to a root folder of TBB)
        }
    } else:macx {
        #message (Using TBB at $$TBB_PATH)
        DEFINES     += WITH_TBB
        INCLUDEPATH += $$TBB_PATH/include
        LIBS        += -L$$TBB_PATH/lib -ltbb

        DEPENDPATH  += $$TBB_PATH/include
    } else {
        #message (Using TBB at $$TBB_PATH)
        DEFINES     += WITH_TBB
        INCLUDEPATH += $$TBB_PATH/include
        LIBS        += -ltbb
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

# Workaround for -fPIC bug
QMAKE_CFLAGS_STATIC_LIB=
QMAKE_CXXFLAGS_STATIC_LIB=

