# try use global config 
exists(../../../config.pri) {
    #message(Using global config)
    include(../../../config.pri)
} else { 
    message(Using local config)
    include(../config.pri)
}
 
TEMPLATE = subdirs

with_opencv:!macx {
    #!contains(SUBDIRS, ocv-homo):      # such operator doesn't work!
    SUBDIRS *= ocv-homo                 # adds project only if it doesn't exist
    SUBDIRS *= openCV
}

SUBDIRS += \
    convolve \
    face_recognition1 \
    face_recognition \
    integral \
    klt_cycle \
    midmap_pyramid \
    rectificator \
    rectificator1 \
    train_vj \
    matrix \
    buffer \
    fileformats \
    arithmetics \
    ssewrappers \
    fastkernel \
    assignment \
    cornerdetector \
    cholesky \
    kalman \
    automotive \
    sphericdist \
    color \
    geometry \
    moments \
    draw \
    affine \
    derivative \
    vector \
    tbb_wrapper \
    homography \
    ransac \
    levenberg \
    rgb24buffer \
    morphologic \
    gradient \
    serializer \
    histogram \
    deform \
    polynomDistortion \
    fastkernel_profile \
    logger \
    cameramodel \
    triangulator \
    cloud \
    distortion \
