cmake_minimum_required(VERSION 3.5)

project(VulkanRenderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Quick Widgets REQUIRED)

add_library(vkrenderer
    #VkMainWindow.h
    #Vk3DRenderer.h
    VkIbWindow.h
    VkIbWindow.cpp
    IbEngine.h
    IbEngine.cpp
    MeshDecoratedRenderable.h
    MeshDecoratedRenderable.cpp
)

message(STATUS "Adding subdirectory: ignimbrite")
set(IGNIMBRITE_WITH_VULKAN ON CACHE BOOL "" FORCE)
set(IGNIMBRITE_WITH_GLFW OFF CACHE BOOL "" FORCE)
set(IGNIMBRITE_WITH_QT ON CACHE BOOL "" FORCE)
set(IGNIMBRITE_WITH_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(ignimbrite)
target_link_libraries(vkrenderer PUBLIC Ignimbrite)
target_link_libraries(vkrenderer PRIVATE tinyobjloader)
target_link_libraries(vkrenderer PRIVATE stb-image)
target_link_libraries(vkrenderer PRIVATE IgnimbriteMaterials)
#target_include_directories(vkrenderer PUBLIC ignimbrite/ignimbrite/include)

if (IGNIMBRITE_WITH_VULKAN)
    target_link_libraries(vkrenderer PUBLIC VulkanDevice)
endif()

target_link_libraries(vkrenderer PUBLIC Qt5::Core Qt5::Quick Qt5::Widgets)
target_link_libraries(vkrenderer PUBLIC corecvs)

