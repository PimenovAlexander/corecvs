# Configuration of a build is determined by CONFIG variable, comment and uncomment
# entries as needed. Configurations can be added from command line, like
# qmake "CONFIG+=with_neon" cvs.pro

# This is a common section, anyway everybody is changing config.pri after checkout

# Open both lines to work with only one configuration that is chosen below.
# Such a config would have alone makefile at each working src dir.
#
#CONFIG -= debug_and_release
#CONFIG -= debug_and_release_target

# By generating VS projects only, neither debug nor release must be chosen!
# For makefiles we ask to generate both config makefiles and build only one chosen below config.
#
!gen_vsproj {
    CONFIG +=   \
                \
        debug \
#        release \

}

CONFIG +=       \
#               \
#   trace       \
#   asserts     \
                \
   with_sse     \
   with_sse3    \
   with_sse4    \
   with_tbb     \


!win32:!macx {
    CONFIG +=             \
                          \
    #   pedantic_build    \
    #   gcc_env_toolchain \
    #  gcc48_toolchain   \
    #   gcc_lto           \
    #   gcc_checker       \
       gcc47_toolchain   \
    #   gcc45_toolchain   \
    #   clang_toolchain   \
    #   clang_analyser    \
    #   ccache_toolchain  \
    #   icc_toolchain     \
    #   arm_toolchain     \
    #    with_neon        \
    #\
    #    profile_alignment \
    #    profile_prepare   \
    #    profile_use       \

}

CONFIG +=                   \
#        with_ueye           \
        with_httpserver     \

win32 {
    CONFIG +=               \
        with_directshow     \
}

win32 {
#    CONFIG += with_opencv
} else {
 #   CONFIG += with_opencv
}

win32 {
   #CONFIG += with_hardware     # win32: is disabled for a while
} else {
#   CONFIG += with_hardware     # enabled only for some linux PCs for debugging
}

win32 {
   #CONFIG += with_openglext	# is disabled as it's not yet implemented
} else {
    CONFIG += with_openglext
}

win32-msvc* {
    CONFIG += with_opencl       # delivered OpenCL.lib is compatible with msvc build tools
} else:win32 {
                                # it's not supported as delivered OpenCL.lib isn't compatible with the win32-mingw linker tool
    CONFIG += with_opencl       # opened as it's managed more carefully by the opencl project
} else {
#    CONFIG += with_opencl       # linux: opened for analysis on different GPUs (only nVidia) and CPUs
}

# include standard part for any project that tunes some specific parameters that depend of the config been set above
#
ROOT_PATH=$$PWD
include(common.pri)
