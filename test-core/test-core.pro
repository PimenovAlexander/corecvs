TARGET = core                                           # default "test-core" is incorrect. The "test.pri" will set it properly to the "test_core"
include(test.pri)

OTHER_FILES +=  \
    gentest.sh  \
    valgrind.sh \

# Never to be fixed
OTHER_FILES += \
    cppunit_test/cppunit_test.cpp \                     # TODO: "cppunit" dep
    snooker/main_test_snooker.cpp \                     # TODO: not the test code!

# Need to be fixed
OTHER_FILES += \
   #ocv-homo/main_test_ocv-homo.cpp \                   # TODO: main + opencv deps
   #openCV/main_test_openCV.cpp \                       # TODO: main + opencv deps

contains(DEFINES, WITH_FFTW) {
SOURCES += \
     fftw/main_test_fftw.cpp \
}

!win32 {
  LIBS += -ldl
}

SOURCES += \
    main.cpp \
    \
    affine/main_test_affine.cpp \
#    aLowCodec/main_test_aLowCodec.cpp \
    arithmetics/main_test_arithmetics.cpp \
    assignment/main_test_assignment.cpp \
    automotive/main_test_automotive.cpp \
    buffer/main_test_buffer.cpp \
    cameramodel/main_test_cameramodel.cpp \
    cholesky/main_test_cholesky.cpp \
    cloud/main_test_cloud.cpp \
    color/main_test_color.cpp \
    commandline/main_test_commandline.cpp \
    convolve/main_test_convolve.cpp \
#    cornerdetector/main_test_cornerdetector.cpp \
#    debayer/main_test_debayer.cpp \
    deform/main_test_deform.cpp \
    derivative/main_test_derivative.cpp \
    distortion/main_test_distortion.cpp \                 # TODO: need to be fixed soon
    draw/main_test_draw.cpp \
    eigen/main_test_eigen.cpp \
   #face_recognition/main_test_face_recognition.cpp \     # TODO: absent input data!
   #face_recognition1/main_test_face_recognition1.cpp \   # TODO: absent input data!
    fastkernel/main_test_fastkernel.cpp \
   #fastkernel_double/main_test_fastkernel_double.cpp \   # moved to perf-tests
   #fastkernel_profile/main_test_fastkernel_profile.cpp \ # moved to perf-tests   
    fileformats/main_test_fileformats.cpp \
   #filter_blocks/main_test_filter_blocks.cpp \           # TODO: can't build
#    gaussianSolution/main_test_gaussianSolution.cpp \     # TODO: check it...
    geometry/main_test_geometry.cpp \
    gradient/main_test_gradient.cpp \
    histogram/main_test_histogram.cpp \
    homography/main_test_homography.cpp \
    integral/main_test_integral.cpp \
    kalman/main_test_kalman.cpp \
    klt_cycle/main_test_klt_cycle.cpp \
    levenberg/main_test_levenberg.cpp \
    logger/main_test_logger.cpp \
    linear/main_test_linear.cpp \
    matrix/main_test_matrix.cpp \                       # TODO: Windows: assert at matrix\main_test_matrix.cpp:385 - Internal problem with double and stdout
    midmap_pyramid/main_test_midmap_pyramid.cpp \
    moments/main_test_moments.cpp \
    morphologic/main_test_morphologic.cpp \
#    polynomDistortion/main_test_polynomDistortion.cpp \ # TODO: check it...
    ransac/main_test_ransac.cpp \
    readers/main_test_readers.cpp \
    rectificator/main_test_rectificator.cpp \
    rectificator1/main_test_rectificator1.cpp \
    rgb24buffer/main_test_rgb24buffer.cpp \
    serializer/main_test_serializer.cpp \
    similarity/main_test_similarity.cpp \
    sphericdist/main_test_sphericdist.cpp \
    ssewrappers/main_test_ssewrappers.cpp \
    tbb_wrapper/main_test_tbb_wrapper.cpp \
   #train_vj/main_train_vj.cpp                          # TODO: read missed file via param
    triangulator/main_test_triangulator.cpp \
    vector/main_test_vector.cpp \
#    yuv/main_test_yuv.cpp \
#    cameracalibration/main_test_camera_structs.cpp \
    conic/main_test_conic.cpp \
#    calstructs/main_test_calstructs.cpp \
    polynomial/main_test_polynomial.cpp \
    meshdraw/main_test_meshdraw.cpp \
#    reconstruction/main_test_reconstruction.cpp \
    meta/main_test_meta.cpp \
    function/main_test_function.cpp \
    deform/test_deform.cpp \
    camerafixture/main_test_camerafixture.cpp \
    renderer/main_test_renderer.cpp \
    meshdraw/test_polymeshdraw.cpp \
    raytrace/main_test_raytrace.cpp \
    json/main_test_json.cpp \
    utils/main_test_utils.cpp \
    quadric/main_test_quadric.cpp \
    planefit/main_test_planefit.cpp \
    noise/main_test_noise.cpp \
    wuRasterizer/main_test_wuRasterizer.cpp \
