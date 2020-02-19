cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME corecvs)
init_project(PROJECT_NAME ${MODULE_NAME}_core_tests)

MESSAGE( STATUS "Including GTest on Tests build" )

set(DIR_NAME
    test-core
    )

set(CONVEXPOLYGON_HEADER_FILE
    convexpolygon/convexDebug.h
    )

set(CPPUNIT_TEST_HEADER_FILES
    cppunit_test/MatcherTest.h
    )

set(GENERATED_HEADER_FILES
    generated/testEnum.h
    generated/testSubClass.h
    generated/testClass.h
    generated/testBlock.h
    )

set(SNOOKER_HEADER_FILES
    snooker/commonTypes.h
    snooker/errors.h
    snooker/reflectionSegmentator.h
    snooker/snookerSegmentator.h
    )

set(STATEMACHINETEST_HEADER_FILES
    stateMachineTest/test.h
    stateMachineTest/test.h
    )

set(ULTRASOUND_HEADER_FILES
    ultrasound/model.h
    ultrasound/imgreader.h
    )

set(HEADERS
    ${CONVEXPOLYGON_HEADER_FILE}
    ${CPPUNIT_TEST_HEADER_FILES}
    ${GENERATED_HEADER_FILES}
    ${SNOOKER_HEADER_FILES}
    ${STATEMACHINETEST_HEADER_FILES}
    ${ULTRASOUND_HEADER_FILES}
    )

set(AFFINE_SOURCE_FILE
    affine/main_test_affine.cpp
    )

set(ALOWCODEC_SOURCE_FILE
#    aLowCodec/main_test_aLowCodec.cpp
    )

set(ARITHMETICS_SOURCE_FILE
    arithmetics/main_test_arithmetics.cpp
    )

set(ASSIGNMENT_SOURCE_FILE
    assignment/main_test_assignment.cpp
    )

set(AUTOMOTIVE_SOURCE_FILE
    automotive/main_test_automotive.cpp
    )

set(BEZIERRASTERIZER_SOURCE_FILE
    bezierRasterizer/main_test_bezier_rasterizer.cpp
    )

set(BSPRENDERER_SOURCE_FILE
    bspRenderer/bspRenderTest.cpp
    bspRenderer/bspRenderer.cpp
    )

set(BSPTREE_SOURCE_FILE
    bsptree/main_test_bsptree.cpp
    )

set(BUFFER_SOURCE_FILE
    buffer/main_test_buffer.cpp
    )

set(CALSTRUCTS_SOURCE_FILE
#    calstructs/main_test_calstructs.cpp
    )

set(CAMERACALIBRATION_SOURCE_FILE
#    cameracalibration/main_test_camera_structs.cpp
    )

set(CAMERAMODEL_SOURCE_FILE
    cameramodel/main_test_cameramodel.cpp
    )

set(CAMERAFIXTURE_SOURCE_FILE
    camerafixture/main_test_camerafixture.cpp
    )

set(CHOLESKY_SOURCE_FILE?
    cholesky/main_test_cholesky.cpp
    )

set(CLOUD_SOURCE_FILE
    cloud/main_test_cloud.cpp
    )

set(COLOR_SOURCE_FILE
    color/main_test_color.cpp
    )

set(COMMANDLINE_SOURCE_FILE
    commandline/main_test_commandline.cpp
    )

set(CONIC_SOURCE_FILE
    conic/main_test_conic.cpp
    )

set(CONVEXHULL_SOURCE_FILE
    convexhull/main_test_convexhull.cpp
    )

set(CONVEXHULL2D_SOURCE_FILE
    convexHull2d/main_test_convexHull2d.cpp
    )

set(CONVEXPOLYGON_SOURCE_FILES
    convexpolygon/convexDebug.cpp
    convexpolygon/main_test_convexpolygon.cpp
    )

set(CONVOLVE_SOURCE_FILE
    convolve/main_test_convolve.cpp
    )

set(DEFORM_SOURCE_FILES
    deform/main_test_deform.cpp
    deform/test_deform.cpp
    )

set(DELAUNAY_SOURCE_FILE
#     delaunay/main_test_delaunay.cpp
    )

set(DERIVATIVE_SOURCE_FILE
    derivative/main_test_derivative.cpp
    )

set(DRAW_SOURCE_FILE
    draw/main_test_draw.cpp
    )

set (EIGEN_SOURCE_FILES
    eigen/main_test_eigen_integration.cpp
#   eigen/main_test_eigen.cpp
    )

set(FASTKERNEL_SOURCE_FILE
    fastkernel/main_test_fastkernel.cpp
    )

set(FILEFORMATS_SOURCE_FILE
    fileformats/main_test_fileformats.cpp
    )

set(FILESYSTEM_SOURCE_FILE
    filesystem/main_test_filesystem.cpp
    )

set(FUNCTION_SOURCE_FILE
    function/main_test_function.cpp
    )

set(GAUSSIANSOLUTION_SOURCE_FILE
#    gaussianSolution/main_test_gaussianSolution.cpp      # TODO: check it...
    )

set(GEOMETRY_SOURCE_FILE
    geometry/main_test_geometry.cpp
    )

set(GENERATED_SOURCE_FILES
    generated/testSubClass.cpp
    generated/testClass.cpp
    generated/testBlock.cpp
    )

set(GRADIENT_SOURCE_FILE
    gradient/main_test_gradient.cpp
    )

set(HALFSPACE_SOURCE_FILE
    halfspace/main_test_halfspace.cpp
    )

set(HISTOGRAM_SOURCE_FILE
    histogram/main_test_histogram.cpp
    )

set(HOMOGRAPHY_SOURCE_FILE
    homography/main_test_homography.cpp
    )

set(INTEGRAL_SOURCE_FILE
    integral/main_test_integral.cpp
    )

set(INSET_OUTSET_SOURCE_FILE
    inset_outset/main_test_inset_outset.cpp
    )

set(JSON_OUTSET_SOURCE_FILE
    json/main_test_json.cpp
    )

set(KALMAN_SOURCE_FILE
    kalman/main_test_kalman.cpp
    )

set(KLT_CYCLE_SOURCE_FILE
    klt_cycle/main_test_klt_cycle.cpp
    )

set(LEVENBERG_SOURCE_FILE
    levenberg/main_test_levenberg.cpp
    )

set(LINEAR_SOURCE_FILE
    linear/main_test_linear.cpp
    )

set(MATRIX_SOURCE_FILE
    matrix/main_test_matrix.cpp                        # TODO: Windows: assert at matrix\main_test_matrix.cpp:385 - Internal problem with double and stdout
    )

set(MESHDRAW_SOURCE_FILE
    meshdraw/main_test_meshdraw.cpp
    )

set(MESHFILTER_SOURCE_FILE
    meshfilter/main_test_meshfilter.cpp
    )

set(META_SOURCE_FILE
    meta/main_test_meta.cpp
    )

set(MIDMAP_PYRAMID_SOURCE_FILE
    midmap_pyramid/main_test_midmap_pyramid.cpp
    )

set(MOMENTS_SOURCE_FILE
    moments/main_test_moments.cpp
    )

set(MORPHOLOGIC_SOURCE_FILE
    morphologic/main_test_morphologic.cpp
    )

set(NOISE_SOURCE_FILE
    noise/main_test_noise.cpp
    )

set(ORIENTEDBOX_SOURCE_FILE
    orientedbox/main_test_orientedbox.cpp
    )

set(PLANEFIT_SOURCE_FILE
    planefit/main_test_planefit.cpp
    )

set(POLYNOMDISTORTION_SOURCE_FILE
#    polynomDistortion/main_test_polynomDistortion.cpp  # TODO: check it...
    )

set(POLYNOMIAL_SOURCE_FILES
    polynomial/main_test_polynomial.cpp
    polynomial/main_test_basis.cpp
    polynomial/main_test_monom.cpp
    polynomial/main_test_polynom.cpp
    )

set(PROCESSOR6D_SOURCE_FILE
    processor6d/main_test_processor6d.cpp
    )

set(PROJECTION_SOURCE_FILE
    projection/main_test_projection.cpp
    )

set(QUADRIC_SOURCE_FILE
    quadric/main_test_quadric.cpp
    )

set(RANSAC_SOURCE_FILE
    ransac/main_test_ransac.cpp
    )

set(RAYTRACE_SOURCE_FILE
    raytrace/main_test_raytrace.cpp
    )

set(READERS_SOURCE_FILE
    readers/main_test_readers.cpp
    )

set(RECONSTRUCTION_SOURCE_FILE
#    reconstruction/main_test_reconstruction.cpp
    )

set(RECTIFICATOR_SOURCE_FILES
    rectificator/main_test_rectificator.cpp
    rectificator1/main_test_rectificator1.cpp
    )

set(RENDERER_SOURCE_FILE
    renderer/main_test_renderer.cpp
    )

set(RGB24BUFFER_SOURCE_FILE
    rgb24buffer/main_test_rgb24buffer.cpp
    )

set(ROTATION_SOURCE_FILE
    rotation/main_test_rotation_lanzcos.cpp
    )

set(SERIALIZER_SOURCE_FILE
    serializer/main_test_serializer.cpp
    )

set(SIMILARITY_SOURCE_FILE
    similarity/main_test_similarity.cpp
    )

set(SPHERICDIST_SOURCE_FILE
    sphericdist/main_test_sphericdist.cpp
    )

set(SSEWRAPPERS_SOURCE_FILE
    ssewrappers/main_test_ssewrappers.cpp
    )

set(TBB_WRAPPER_SOURCE_FILE
    tbb_wrapper/main_test_tbb_wrapper.cpp
    )

set(TRIANGULATOR_SOURCE_FILE
    triangulator/main_test_triangulator.cpp
    )

set (ULTRASOUND_SOURCE_FILES    
    ultrasound/main_test_ultrasound_reconstruction.cpp
    ultrasound/model.cpp
    ultrasound/imgreader.cpp
    )

set(UTILS_SOURCE_FILE
    utils/main_test_utils.cpp
    )

set(VECTOR_SOURCE_FILE
    vector/main_test_vector.cpp
    )

set(VPTREE_SOURCE_FILE
    vptree/main_test_vptree.cpp
    )

set(WURASTERIZER_SOURCE_FILE
    wuRasterizer/main_test_wu_rasterizer.cpp
    )

set(YUV_SOURCE_FILE
#    yuv/main_test_yuv.cpp
    )

set(SOURCES
    ${AFFINE_SOURCE_FILE}
    ${ALOWCODEC_SOURCE_FILE}
    ${ARITHMETICS_SOURCE_FILE}
    ${ASSIGNMENT_SOURCE_FILE}
    ${AUTOMOTIVE_SOURCE_FILE}
    ${BEZIERRASTERIZER_SOURCE_FILE}
    ${BSPRENDERER_SOURCE_FILE}
    ${BSPTREE_SOURCE_FILE}
    ${BUFFER_SOURCE_FILE}
    ${CALSTRUCTS_SOURCE_FILE}
    ${CAMERACALIBRATION_SOURCE_FILE}
    ${CAMERAMODEL_SOURCE_FILE}
    ${CAMERAFIXTURE_SOURCE_FILE}
    ${CHOLESKY_SOURCE_FILE}
    ${CLOUD_SOURCE_FILE}
    ${COLOR_SOURCE_FILE}
    ${COMMANDLINE_SOURCE_FILE}
    ${CONIC_SOURCE_FILE}
    ${CONVEXHULL_SOURCE_FILE}
    ${CONVEXHULL2D_SOURCE_FILE}
    ${CONVEXPOLYGON_SOURCE_FILES}
    ${CONVOLVE_SOURCE_FILE}
    ${DEFORM_SOURCE_FILES}
    ${DELAUNAY_SOURCE_FILE}
    ${DERIVATIVE_SOURCE_FILE}
    ${DRAW_SOURCE_FILE}
    ${EIGEN_SOURCE_FILES}
    ${FASTKERNEL_SOURCE_FILE}
    ${FILEFORMATS_SOURCE_FILE}
    ${FILESYSTEM_SOURCE_FILE}
    ${FUNCTION_SOURCE_FILE}
    ${GAUSSIANSOLUTION_SOURCE_FILE}
    ${GEOMETRY_SOURCE_FILE}
    ${GENERATED_SOURCE_FILES}
    ${GRADIENT_SOURCE_FILE}
    ${HALFSPACE_SOURCE_FILE}
    ${HISTOGRAM_SOURCE_FILE}
    ${HOMOGRAPHY_SOURCE_FILE}
    ${INTEGRAL_SOURCE_FILE}
    ${INSET_OUTSET_SOURCE_FILE}
    ${JSON_OUTSET_SOURCE_FILE}
    ${KALMAN_SOURCE_FILE}
    ${KLT_CYCLE_SOURCE_FILE}
    ${LEVENBERG_SOURCE_FILE}
    ${LINEAR_SOURCE_FILE}
    ${MATRIX_SOURCE_FILE}
    ${MESHDRAW_SOURCE_FILE}
    ${MESHFILTER_SOURCE_FILE}
    ${META_SOURCE_FILE}
    ${MIDMAP_PYRAMID_SOURCE_FILE}
    ${MOMENTS_SOURCE_FILE}
    ${MORPHOLOGIC_SOURCE_FILE}
    ${NOISE_SOURCE_FILE}
    ${ORIENTEDBOX_SOURCE_FILE}
    ${PLANEFIT_SOURCE_FILE}
    ${POLYNOMDISTORTION_SOURCE_FILE}
    ${POLYNOMIAL_SOURCE_FILES}
    ${PROCESSOR6D_SOURCE_FILE}
    ${PROJECTION_SOURCE_FILE}
    ${QUADRIC_SOURCE_FILE}
    ${RANSAC_SOURCE_FILE}
    ${RAYTRACE_SOURCE_FILE}
    ${READERS_SOURCE_FILE}
    ${RECONSTRUCTION_SOURCE_FILE}
    ${RECTIFICATOR_SOURCE_FILES}
    ${RENDERER_SOURCE_FILE}
    ${RGB24BUFFER_SOURCE_FILE}
    ${ROTATION_SOURCE_FILE}
    ${SERIALIZER_SOURCE_FILE}
    ${SIMILARITY_SOURCE_FILE}
    ${SPHERICDIST_SOURCE_FILE}
    ${SSEWRAPPERS_SOURCE_FILE}
    ${TBB_WRAPPER_SOURCE_FILE}
    ${TRIANGULATOR_SOURCE_FILE}
    ${ULTRASOUND_SOURCE_FILES}
    ${UTILS_SOURCE_FILE}
    ${VECTOR_SOURCE_FILE}
    ${VPTREE_SOURCE_FILE}
    ${WURASTERIZER_SOURCE_FILE}
    ${YUV_SOURCE_FILE}
    )

message(STATUS "SOURCE_DIR " ${corecvs_SOURCE_DIR})
message(STATUS "TEST_SOURCE_DIR " ${PROJECT_SOURCE_DIR})

assign_source_group(${HEADERS} ${SOURCES})

add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
    ${RESOURCES}
    )

target_link_libraries(${PROJECT_NAME}
    corecvs
    gtest
    gtest_main
    stdc++fs
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )

copy_directory(${PROJECT_NAME} 
    ${PROJECT_BINARY_DIR}/${DIR_NAME}
    ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    )

# For ctest support. Not necessary to use but nice move for automated testing.
add_test(
    NAME        core-test
    COMMAND     ./$${PROJECT_NAME}
    )