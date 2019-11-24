SET(APRILTAG_INCLUDE_SEARCH_PATHS
        ${CMAKE_CURRENT_LIST_DIR}/../../siblings/apriltag
        /usr/include/
        /usr/local/include/
        /usr/local/include/apriltag/
        /usr/local/include/apriltag/common/)

SET(APRILTAG_LIB_SEARCH_PATHS
       ${CMAKE_CURRENT_LIST_DIR}/../../siblings/apriltag
        /lib/
        /lib64/
        /usr/lib/
        /usr/lib64/
        /usr/local/lib/
        /usr/local/lib64/
        )

FIND_PATH(APRILTAG_INCLUDE_DIR NAMES
        apriltag.h
        apriltag_math.h
        apriltag_pose.h
        tag16h5.h
        tag25h9.h
        tag36h11.h
        tagCircle21h7.h
        tagCircle49h12.h
        tagCustom48h12.h
        tagStandard41h12.h
        tagStandard52h13.h

#        common headers
#        is it really neccecery?
        doubles.h
        doubles_floats_impl.h
        floats.h
        g2d.h
        getopt.h
        homography.h
        image_types.h
        image_u8.h
        image_u8x3.h
        image_u8x4.h
        matd.h
        math_util.h
        pam.h
        pjpeg.h
        pnm.h
        postscript_utils.h
        string_util.h
        svd22.h
        time_util.h
        time_profile.h
        unionfind.h
        workerpool.h
        zarray.h
        zhash.h
        zmaxheap.h
        PATHS ${Apriltag_INCLUDE_SEARCH_PATHS}
        )

FIND_LIBRARY(APRILTAG_LIB NAMES apriltag PATHS ${APRILTAG_LIB_SEARCH_PATHS})

if (APRILTAG_LIB)
  message("Apriltag has been found!")
  SET(APRILTAG_FOUND ON)
else()
  SET(APRILTAG_FOUND OFF)
endif()
