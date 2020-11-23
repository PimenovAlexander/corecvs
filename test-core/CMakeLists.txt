project (CoreCVSCoreTests)

set(MODULE_NAME corecvs)
set(MODULE_NAME_TEST corecvs_core_tests)


MESSAGE( STATUS "Including GTest on Tests build" )
#include_directories (${GTest_INCLUDE_DIR}/..)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/multifractalSpectrum/2.bmp
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
set (CORE_TEST_SOURCES
#    eigen/main_test_eigen_integration.cpp
#
#    ultrasound/main_test_ultrasound_reconstruction.cpp
#    ultrasound/model.cpp
#    ultrasound/imgreader.cpp
#    ultrasound/model.h
#    ultrasound/imgreader.h
#
#    generated/testEnum.h
#    generated/testSubClass.h
#    generated/testClass.h
#    generated/testBlock.h
#
#    generated/testSubClass.cpp
#    generated/testClass.cpp
#    generated/testBlock.cpp
#
##    eigen/main_test_eigen.cpp
#    affine/main_test_affine.cpp
##    aLowCodec/main_test_aLowCodec.cpp
#    arithmetics/main_test_arithmetics.cpp
#    assignment/main_test_assignment.cpp
#    automotive/main_test_automotive.cpp
#    buffer/main_test_buffer.cpp
#    cameramodel/main_test_cameramodel.cpp
#    cholesky/main_test_cholesky.cpp
#    cloud/main_test_cloud.cpp
#    color/main_test_color.cpp
#    commandline/main_test_commandline.cpp
#    convolve/main_test_convolve.cpp
#    deform/main_test_deform.cpp
#    derivative/main_test_derivative.cpp
#    draw/main_test_draw.cpp
#    fastkernel/main_test_fastkernel.cpp
#    fileformats/main_test_fileformats.cpp
#    filesystem/main_test_filesystem.cpp
##    gaussianSolution/main_test_gaussianSolution.cpp      # TODO: check it...
#    geometry/main_test_geometry.cpp
#    gradient/main_test_gradient.cpp
#    histogram/main_test_histogram.cpp
#    homography/main_test_homography.cpp
#    integral/main_test_integral.cpp
#    inset_outset/main_test_inset_outset.cpp
#    kalman/main_test_kalman.cpp
#    klt_cycle/main_test_klt_cycle.cpp
#    levenberg/main_test_levenberg.cpp
#    linear/main_test_linear.cpp
#    matrix/main_test_matrix.cpp                        # TODO: Windows: assert at matrix\main_test_matrix.cpp:385 - Internal problem with double and stdout
#    midmap_pyramid/main_test_midmap_pyramid.cpp
#    moments/main_test_moments.cpp
#    morphologic/main_test_morphologic.cpp
##    polynomDistortion/main_test_polynomDistortion.cpp  # TODO: check it...
#    ransac/main_test_ransac.cpp
#    readers/main_test_readers.cpp
#    rectificator/main_test_rectificator.cpp
#    rectificator1/main_test_rectificator1.cpp
#    rgb24buffer/main_test_rgb24buffer.cpp
#    rotation/main_test_rotation_lanzcos.cpp
#    serializer/main_test_serializer.cpp
#    similarity/main_test_similarity.cpp
#    sphericdist/main_test_sphericdist.cpp
#    ssewrappers/main_test_ssewrappers.cpp
#    tbb_wrapper/main_test_tbb_wrapper.cpp
#    triangulator/main_test_triangulator.cpp
#    vector/main_test_vector.cpp
##    yuv/main_test_yuv.cpp
##    cameracalibration/main_test_camera_structs.cpp
#    conic/main_test_conic.cpp
##    calstructs/main_test_calstructs.cpp
#    polynomial/main_test_polynomial.cpp
#    polynomial/main_test_basis.cpp
#    polynomial/main_test_monom.cpp
#    polynomial/main_test_polynom.cpp
##    reconstruction/main_test_reconstruction.cpp
#    meta/main_test_meta.cpp
#    function/main_test_function.cpp
#    deform/test_deform.cpp
#    camerafixture/main_test_camerafixture.cpp
#    renderer/main_test_renderer.cpp
#    raytrace/main_test_raytrace.cpp
#    json/main_test_json.cpp
#    utils/main_test_utils.cpp
#    quadric/main_test_quadric.cpp
#    planefit/main_test_planefit.cpp
#    noise/main_test_noise.cpp
#    projection/main_test_projection.cpp
#    vptree/main_test_vptree.cpp
#    meshfilter/main_test_meshfilter.cpp
#    meshdraw/main_test_meshdraw.cpp
#    meshcache/main_test_meshcache.cpp
#
#    convexpolygon/convexDebug.cpp
#    convexpolygon/main_test_convexpolygon.cpp
#    convexhull/main_test_convexhull.cpp
#    convexHull2d/main_test_convexHull2d.cpp
#
#    wuRasterizer/main_test_wu_rasterizer.cpp
#    bsptree/main_test_bsptree.cpp
#    halfspace/main_test_halfspace.cpp
#    orientedbox/main_test_orientedbox.cpp
#    processor6d/main_test_processor6d.cpp
#    bezierRasterizer/main_test_bezier_rasterizer.cpp
##     delaunay/main_test_delaunay.cpp
#    bspRenderer/bspRenderTest.cpp
#    bspRenderer/bspRenderer.cpp
#
#    cppunit_test/MatcherTest.h
#    snooker/commonTypes.h
#    snooker/errors.h
#    snooker/reflectionSegmentator.h
#    snooker/snookerSegmentator.h
#    convexpolygon/convexDebug.h
#    stateMachineTest/test.h
#    stateMachineTest/test.h
        multifractalSpectrum/main_test_multifractalSpectrum.cpp)

message(STATUS "SOURCE_DIR " ${corecvs_SOURCE_DIR})
message(STATUS "TEST_SOURCE_DIR " ${PROJECT_SOURCE_DIR})


add_executable(${MODULE_NAME_TEST} ${CORE_TEST_SOURCES})

add_custom_command(TARGET ${MODULE_NAME_TEST} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${MODULE_NAME_TEST}  ${CMAKE_BINARY_DIR}/bin/${MODULE_NAME_TEST}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${MODULE_NAME_TEST} to binary directory"
        )

set_property(TARGET ${MODULE_NAME_TEST} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${MODULE_NAME_TEST} PROPERTY CXX_STANDARD_REQUIRED ON)


target_link_libraries(${MODULE_NAME_TEST} gtest gtest_main stdc++fs corecvs)
target_include_directories(${MODULE_NAME_TEST} PUBLIC ${corecvs_SOURCE_DIR})

# For ctest support. Not necessary to use but nice move for automated testing.
add_test(
        NAME
            core-test
        COMMAND
            ./${MODULE_NAME_TEST}
)

