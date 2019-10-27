project (Flowtest)


set (FLOWTEST_NAME flowtest )

set (FLOWTEST_SOURCES
    main_flowtest.cpp
)

add_executable(${FLOWTEST_NAME} ${FLOWTEST_SOURCES})

target_link_libraries(${FLOWTEST_NAME} stdc++fs corecvs)

