target_sources(corecvs
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/fixtureCamera.h     
        ${CMAKE_CURRENT_LIST_DIR}/cameraFixture.h     
        ${CMAKE_CURRENT_LIST_DIR}/fixtureScene.h      
        ${CMAKE_CURRENT_LIST_DIR}/sceneFeaturePoint.h 
        ${CMAKE_CURRENT_LIST_DIR}/cameraPrototype.h   
        ${CMAKE_CURRENT_LIST_DIR}/fixtureScenePart.h  
        ${CMAKE_CURRENT_LIST_DIR}/wildcardablePointerPair.h 


    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/fixtureCamera.cpp     
        ${CMAKE_CURRENT_LIST_DIR}/fixtureScene.cpp      
        ${CMAKE_CURRENT_LIST_DIR}/sceneFeaturePoint.cpp 
        ${CMAKE_CURRENT_LIST_DIR}/cameraFixture.cpp     
        ${CMAKE_CURRENT_LIST_DIR}/cameraPrototype.cpp   
        ${CMAKE_CURRENT_LIST_DIR}/fixtureScenePart.cpp  
        ${CMAKE_CURRENT_LIST_DIR}/wildcardablePointerPair.cpp 
    )
