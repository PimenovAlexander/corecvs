target_sources(corecvs
    PUBLIC
        cameracalibration/flatPatternCalibrator.h    
        cameracalibration/calibrationLocation.h             
        cameracalibration/cameraConstraints.h        
        cameracalibration/calibrationDrawHelpers.h          
        cameracalibration/cameraModel.h              
        cameracalibration/projection/projectionModels.h         
        cameracalibration/projection/pinholeCameraIntrinsics.h  
        cameracalibration/projection/equidistantProjection.h    
        cameracalibration/projection/equisolidAngleProjection.h 
        cameracalibration/projection/stereographicProjection.h
        cameracalibration/projection/projectionFactory.h
        cameracalibration/ilFormat.h
        cameracalibration/projection/omnidirectionalProjection.h


    PRIVATE
        cameracalibration/calibrationLocation.cpp      
        cameracalibration/flatPatternCalibrator.cpp    
        cameracalibration/cameraConstraints.cpp        
        cameracalibration/calibrationDrawHelpers.cpp   
        cameracalibration/cameraModel.cpp              
        cameracalibration/projection/projectionModels.cpp         
        cameracalibration/projection/pinholeCameraIntrinsics.cpp  
        cameracalibration/projection/equidistantProjection.cpp    
        cameracalibration/projection/equisolidAngleProjection.cpp 
        cameracalibration/projection/stereographicProjection.cpp
        cameracalibration/projection/projectionFactory.cpp
        cameracalibration/ilFormat.cpp
        cameracalibration/projection/omnidirectionalProjection.cpp

    )
