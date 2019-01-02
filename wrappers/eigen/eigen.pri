EIGEN_PATH=$$(EIGEN_PATH)

isEmpty(EIGEN_PATH) {
    EIGEN_PATH=$$(EIGEN3_DIR)
}

isEmpty(EIGEN_PATH) {
    win32 {
        EIGEN_PATH=$$PWD/../../3dparty/eigen
    }
    else {
        exists(/usr/include/eigen/Eigen/Dense) {
            EIGEN_PATH=/usr/include/eigen
        }

        exists(/usr/include/eigen3/Eigen/Dense) {
            EIGEN_PATH=/usr/include/eigen3
        }

        isEmpty(EIGEN_PATH) {
            EIGEN_PATH=$$PWD/../../3dparty/eigen
        }
    }
}

exists($$EIGEN_PATH/Eigen/Dense) {
	!build_pass: message(Using Eigen from <$$EIGEN_PATH>)

	INCLUDEPATH = $$EIGEN_PATH $$INCLUDEPATH
	DEFINES += WITH_EIGEN
}
else {
	!build_pass: message(The Eigen not found. Please set EIGEN_PATH properly)
}
