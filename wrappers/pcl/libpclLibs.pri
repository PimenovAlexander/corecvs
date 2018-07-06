with_libpcl {
    PCL_ROOT = $$(PCL_ROOT)
    win32 {
        !isEmpty(PCL_ROOT) {
            exists($$PCL_ROOT/include/pcl-1.8/pcl/point_types.h) {
                INCLUDEPATH += $$PCL_ROOT/include/pcl-1.8
                LIBS        += -L$$PCL_ROOT/lib/
                DEFINES     += WITH_LIBPCL
                !build_pass: message(Using PCL from <$$PCL_ROOT>)
            } else {
                !build_pass:message(PCL not found.)
            }
        }
    } else {
        isEmpty(PCL_ROOT) {
            !build_pass: message(Linking with system PCL)
            exists(/usr/include/pcl-1.8/pcl/point_types.h) {
                INCLUDEPATH += /usr/include/pcl-1.8
#               INCLUDEPATH += /usr/include/eigen3
                DEFINES     += WITH_LIBPCL
            } else {
                !build_pass:message(PCL not found.)
            }
        } else {
            !build_pass: message(Linking with PCL from $$PCL_ROOT)
            DEPENDPATH  += $$PCL_ROOT/include/pcl-1.8
            INCLUDEPATH += $$PCL_ROOT/include/pcl-1.8
            LIBS        += -L$$PCL_ROOT/lib/
            DEFINES     += WITH_LIBPCL
        }
    }

    contains(DEFINES, WITH_LIBPCL) {                    # if it's installed properly with found path for inc
        win32 {
          build_pass:CONFIG(debug, debug|release) {
            LIBS += -lpcl_keypoints_debug    -lpcl_visualization_debug -lpcl_stereo_debug           -lpcl_people_debug -lpcl_recognition_debug -lpcl_io_ply_debug -lpcl_octree_debug
            LIBS += -lpcl_registration_debug -lpcl_filters_debug       -lpcl_search_debug           -lpcl_kdtree_debug -lpcl_io_debug          -lpcl_ml_debug     -lpcl_features_debug -lpcl_tracking_debug
            LIBS += -lpcl_segmentation_debug -lpcl_outofcore_debug     -lpcl_sample_consensus_debug -lpcl_common_debug -lpcl_surface_debug
          }
          build_pass:CONFIG(release, debug|release) {
            LIBS += -lpcl_keypoints_release    -lpcl_visualization_release -lpcl_stereo_release           -lpcl_people_release -lpcl_recognition_release -lpcl_io_ply_release -lpcl_octree_release
            LIBS += -lpcl_registration_release -lpcl_filters_release       -lpcl_search_release           -lpcl_kdtree_release -lpcl_io_release          -lpcl_ml_release     -lpcl_features_release -lpcl_tracking_release
            LIBS += -lpcl_segmentation_release -lpcl_outofcore_release     -lpcl_sample_consensus_release -lpcl_common_release -lpcl_surface_release
          }
        } else {
            LIBS += -lpcl_keypoints    -lpcl_visualization -lpcl_stereo           -lpcl_people -lpcl_recognition -lpcl_io_ply            -lpcl_apps      -lpcl_octree
            LIBS += -lpcl_registration -lpcl_filters       -lpcl_search           -lpcl_kdtree -lpcl_io          -lpcl_ml -lpcl_features -lpcl_tracking
            LIBS += -lpcl_segmentation -lpcl_outofcore     -lpcl_sample_consensus -lpcl_common -lpcl_surface

            LIBS += -lgmp
        }
    }
}
