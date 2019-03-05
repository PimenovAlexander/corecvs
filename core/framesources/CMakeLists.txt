target_sources(corecvs
    PUBLIC
        framesources/frames.h
	framesources/cameraControlParameters.h 
	framesources/dummyVideoEncoderInterface.h 
	framesources/imageCaptureInterface.h 
	
	framesources/file/abstractFileCapture.h 
	framesources/file/abstractFileCaptureSpinThread.h 
	framesources/file/fileCapture.h 
	framesources/file/imageFileCaptureInterface.h 
	framesources/file/precCapture.h 
	
	framesources/decoders/aLowCodec.h 
	framesources/decoders/decoupleYUYV.h 
	framesources/decoders/mjpegDecoder.h 
	framesources/decoders/mjpegDecoderLazy.h 

    PRIVATE
        framesources/frames.cpp
	framesources/cameraControlParameters.cpp 
	framesources/dummyVideoEncoderInterface.cpp 
	framesources/imageCaptureInterface.cpp 
	
	framesources/file/abstractFileCapture.cpp 
	framesources/file/abstractFileCaptureSpinThread.cpp 
	framesources/file/fileCapture.cpp 
	framesources/file/imageFileCaptureInterface.cpp 
	framesources/file/precCapture.cpp 
	
	framesources/decoders/aLowCodec.cpp 
	framesources/decoders/decoupleYUYV.cpp 
	framesources/decoders/mjpegDecoder.cpp 
	framesources/decoders/mjpegDecoderLazy.cpp 
)
