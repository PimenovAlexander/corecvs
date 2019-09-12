# Configuration of a build is determined by CONFIG variable, comment and uncomment
# entries as needed. Configurations can be added from command line, like
# qmake "CONFIG+=with_neon" cvs.pro


# This is a common section, anyway everybody is changing config.pri after checkout

# Open both lines to work with only one configuration that is chosen below.
# Such a config would have alone makefile at each working src dir.
#
#CONFIG -= debug_and_release
#CONFIG -= debug_and_release_target

!contains(CORECVS_INCLUDED, "config.pri") {
CORECVS_INCLUDED +=  config.pri

# By generating VS projects only, neither debug nor release must be chosen!
# For makefiles we ask to generate both config makefiles and build only one chosen below config.
#
!gen_vsproj {
    CONFIG +=   \
                \
        debug   \
        release \

}

CONFIG +=       \
#               \
#   trace       \
   asserts     \
                \
   with_sse     \    # all present CPUs support SSE* instructions
   with_sse3    \
   with_sse4_1  \    # all the CPUs Core2 and above support the set of instructions above
#      with_sse4_2 \    # implemented in the Nehalem-based Intel Core i7 product line CPUs; "popcnt" instruction beginning with Nehalem
#      with_avx    \
#      with_avx2   \
#      with_fma    \
   with_native     \
   \
   with_tbb        \
   with_openblas   \
   with_fastbuild  \
   with_unorthodox \   # allow use an experimental filesystem
#  with_qscript    \   # experimental...

include(config-cpu-features.pri)

!win32:!macx {
    CONFIG +=             \
                          \
    #   pedantic_build    \
    #   gcc_env_toolchain \
    #   gcc48_toolchain   \
    #   gcc_lto           \
    #   gcc_checker       \
    #   gcc47_toolchain   \
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
#        with_httpserver     \
        with_avcodec        \
        with_libjpeg        \
        with_libpng         \


win32 {
    CONFIG +=               \
        with_directshow     \
}

!win32 {
     CONFIG += with_opencv
     CONFIG += with_nopkgconfig
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
 #   CONFIG += with_opencl       # delivered OpenCL.lib is compatible with msvc build tools
} else:win32 {
                                # it's not supported as delivered OpenCL.lib isn't compatible with the win32-mingw linker tool
#    CONFIG += with_opencl       # opened as it's managed more carefully by the opencl project
} else {
#    CONFIG += with_opencl       # Linux: opened for analysis on different GPUs (only nVidia) and CPUs
}

# include standard part for any project that tunes some specific parameters that depend of the config been set above
#
include(common.pri)

} # !contains(CORECVS_INCLUDED, "config.pri")

