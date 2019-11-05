project (Flowtest)


set (FLOWTEST_NAME flowtest )

set (SRC_FILES
    main_flowtest.cpp
)

if (OpenCV_LIBS)
include(../../wrappers/opencv/sourcelist.cmake)
endif()

add_executable(${FLOWTEST_NAME} ${SRC_FILES} ${HDR_FILES})

target_link_libraries(${FLOWTEST_NAME} stdc++fs -pthread corecvs)




