# try use global config 
exists(../../../config.pri) {
    ROOT_DIR=../../..
    #message(Using global config)
} else { 
    message(Using local config)
    ROOT_DIR=..
}
!win32 {                                            # it dues to the "mocinclude.tmp" bug on win32!
    ROOT_DIR=$$PWD/$$ROOT_DIR
}
include($$ROOT_DIR/config.pri)
 
TEMPLATE = subdirs

with_opencv:!macx {
    #!contains(SUBDIRS, ocv-homo):                  # such operator doesn't work!
    SUBDIRS *= ocv-homo                             # adds project only if it doesn't exist
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
    similarity \
	eigen \
	convert \
