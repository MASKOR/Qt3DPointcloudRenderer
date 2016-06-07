project(Qt3DPointcloudRenderer)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt53DInput REQUIRED)
find_package(Qt53DQuick REQUIRED)
find_package(Qt53DRender REQUIRED)
find_package(Qt53DQuickRender REQUIRED)

find_package(PCL REQUIRED COMPONENTS common io apps filters kdtree search features surface keypoints registration octree sample_consensus segmentation tracking)
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

include_directories(${PROJECT_SOURCE_DIR}/src/)
include_directories(${PROJECT_SOURCE_DIR}/include/)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qpointcloud.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qpointcloudgeometry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qpointfield.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qpointcloudreader.cpp
  )
set(HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include/qpointcloud.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/qpointcloudgeometry.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/qpointfield.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/qpointcloudreader.h
  )

set(BUILD_EXAMPLE ON CACHE BOOL "Build the example project")

if(BUILD_EXAMPLE)
    add_subdirectory(example)
endif(BUILD_EXAMPLE)
  
add_library(Qt3DPointcloudRenderer ${SOURCE} ${HEADER})

target_link_libraries(Qt3DPointcloudRenderer ${PCL_LIBRARIES})

qt5_use_modules(Qt3DPointcloudRenderer Qml Widgets Quick 3DCore 3DQuick 3DRender 3DQuickRender)
