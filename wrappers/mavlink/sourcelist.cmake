set (HDR_FILES
        ${HDR_FILES}
        ${CMAKE_CURRENT_LIST_DIR}/checksum.h
        ${CMAKE_CURRENT_LIST_DIR}/mavlink_conversions.h
        ${CMAKE_CURRENT_LIST_DIR}/mavlink_get_info.h
        ${CMAKE_CURRENT_LIST_DIR}/mavlink_helpers.h
        ${CMAKE_CURRENT_LIST_DIR}/mavlink_sha256.h
        ${CMAKE_CURRENT_LIST_DIR}/mavlink_types.h
        ${CMAKE_CURRENT_LIST_DIR}/message.hpp
        ${CMAKE_CURRENT_LIST_DIR}/msgmap.hpp
        ${CMAKE_CURRENT_LIST_DIR}/protocol.h
        ${CMAKE_CURRENT_LIST_DIR}/minimal/gtestsuite.hpp
        ${CMAKE_CURRENT_LIST_DIR}/minimal/mavlink.h
        ${CMAKE_CURRENT_LIST_DIR}/minimal/mavlink_msg_heartbeat.h
        ${CMAKE_CURRENT_LIST_DIR}/minimal/mavlink_msg_heartbeat.hpp
        ${CMAKE_CURRENT_LIST_DIR}/minimal/mavlink_msg_protocol_version.h
        ${CMAKE_CURRENT_LIST_DIR}/minimal/mavlink_msg_protocol_version.hpp
        ${CMAKE_CURRENT_LIST_DIR}/minimal/minimal.h
        ${CMAKE_CURRENT_LIST_DIR}/minimal/minimal.hpp
        ${CMAKE_CURRENT_LIST_DIR}/minimal/testsuite.h
        ${CMAKE_CURRENT_LIST_DIR}/minimal/version.h
        )

include_directories(${CMAKE_CURRENT_LIST_DIR})
