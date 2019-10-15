target_sources(corecvs
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/frames.h
	${CMAKE_CURRENT_LIST_DIR}/cameraControlParameters.h 
	${CMAKE_CURRENT_LIST_DIR}/dummyVideoEncoderInterface.h 
	${CMAKE_CURRENT_LIST_DIR}/imageCaptureInterface.h 
	
	${CMAKE_CURRENT_LIST_DIR}/file/abstractFileCapture.h 
	${CMAKE_CURRENT_LIST_DIR}/file/abstractFileCaptureSpinThread.h 
	${CMAKE_CURRENT_LIST_DIR}/file/fileCapture.h 
	${CMAKE_CURRENT_LIST_DIR}/file/imageFileCaptureInterface.h 
	${CMAKE_CURRENT_LIST_DIR}/file/precCapture.h 
	
	${CMAKE_CURRENT_LIST_DIR}/decoders/aLowCodec.h 
	${CMAKE_CURRENT_LIST_DIR}/decoders/decoupleYUYV.h 
	${CMAKE_CURRENT_LIST_DIR}/decoders/mjpegDecoder.h 
	${CMAKE_CURRENT_LIST_DIR}/decoders/mjpegDecoderLazy.h 

    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/frames.cpp
	${CMAKE_CURRENT_LIST_DIR}/cameraControlParameters.cpp 
	${CMAKE_CURRENT_LIST_DIR}/dummyVideoEncoderInterface.cpp 
	${CMAKE_CURRENT_LIST_DIR}/imageCaptureInterface.cpp 
	
	${CMAKE_CURRENT_LIST_DIR}/file/abstractFileCapture.cpp 
	${CMAKE_CURRENT_LIST_DIR}/file/abstractFileCaptureSpinThread.cpp 
	${CMAKE_CURRENT_LIST_DIR}/file/fileCapture.cpp 
	${CMAKE_CURRENT_LIST_DIR}/file/imageFileCaptureInterface.cpp 
	${CMAKE_CURRENT_LIST_DIR}/file/precCapture.cpp 
	
	${CMAKE_CURRENT_LIST_DIR}/decoders/aLowCodec.cpp 
	${CMAKE_CURRENT_LIST_DIR}/decoders/decoupleYUYV.cpp 
	${CMAKE_CURRENT_LIST_DIR}/decoders/mjpegDecoder.cpp 
	${CMAKE_CURRENT_LIST_DIR}/decoders/mjpegDecoderLazy.cpp 
)
