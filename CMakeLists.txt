project(Qt3DPointcloudRenderer)
cmake_minimum_required(VERSION 3.0.2)

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

set(WITH_PCL ON  CACHE BOOL "Enable pcl")
set(WITH_LAS OFF CACHE BOOL "Enable las")

if(WITH_PCL)
    find_package(PCL REQUIRED COMPONENTS common io)
    link_directories(${PCL_LIBRARY_DIRS})
endif(WITH_PCL)

if(WITH_LAS)
    # LibLAS is used for easy integration with windows. Thus it is not included
    # with an additional find-script, but quick-and-dirty by integrating LAS into
    # the the project. Retain libLAS copyright notice in your project (BSD License).

    set(HAVE_GDAL FALSE CACHE BOOL "Enable GDAL for LibLAS")
    set(HAVE_LIBGEOTIFF FALSE CACHE BOOL "Enable libGeoTIFF for LibLAS")
    set(HAVE_LASZIP TRUE CACHE BOOL "Enable Laszip for LibLAS")
    set(LIBLAS_PATH CACHE PATH "Path to libLAS folder, containing \"include\" and \"src\"")
    set(LIBLASZIP_PATH CACHE PATH "Path to LASzip folder VERSION v2.2.0! (newer versions are incompatible)")

    file(GLOB_RECURSE LASZIP_SOURCES ${LIBLASZIP_PATH}/include/*.hpp
                                     ${LIBLASZIP_PATH}/include/*.h
                                     ${LIBLASZIP_PATH}/src/*.hpp
                                     ${LIBLASZIP_PATH}/src/*.h
                                     ${LIBLAS_PATH}/src/*.cpp )

    file(GLOB_RECURSE LIBLAS_SOURCES ${LIBLAS_PATH}/include/*.hpp
                                     ${LIBLAS_PATH}/include/*.h
                                     ${LIBLAS_PATH}/src/*.hpp
                                     ${LIBLAS_PATH}/src/*.h
                                     ${LIBLAS_PATH}/src/*.cpp )
    if(NOT HAVE_GDAL)
      file(GLOB_RECURSE GT_SRC ${LIBLAS_PATH}/src/gt_*.cpp
                               ${LIBLAS_PATH}/src/gt_*.h)
      list(REMOVE_ITEM LIBLAS_SOURCES "${GT_SRC}")
    endif(NOT HAVE_GDAL)

    if(NOT HAVE_LIBGEOTIFF)
      file(GLOB_RECURSE GEO_SRC ${LIBLAS_PATH}/src/tifvsi.cpp)
      list(REMOVE_ITEM LIBLAS_SOURCES "${GEO_SRC}")
    endif(NOT HAVE_LIBGEOTIFF)
endif(WITH_LAS)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qpointcloud.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qpointcloudgeometry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qpointfield.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qpointcloudreader.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/xyzcsvreader.cpp
  )
set(HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include/qpointcloud.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/qpointcloudgeometry.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/qpointfield.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/qpointcloudreader.h
#    ${CMAKE_CURRENT_SOURCE_DIR}/include/xyzcsvreader.h
  )

set(BUILD_EXAMPLE ON CACHE BOOL "Build the example project")

if(BUILD_EXAMPLE)
    add_subdirectory(example)
endif(BUILD_EXAMPLE)

add_library(Qt3DPointcloudRenderer ${SOURCE} ${HEADER} ${LIBLAS_SOURCES} ${LASZIP_SOURCES})

target_include_directories(Qt3DPointcloudRenderer PUBLIC  ${PROJECT_SOURCE_DIR}/include/
                                                  PRIVATE ${PROJECT_SOURCE_DIR}/src/ )

if(WITH_PCL)
    target_link_libraries(Qt3DPointcloudRenderer ${PCL_LIBRARIES} stdc++)
    target_include_directories(${PROJECT_NAME} PRIVATE PRIVATE ${PCL_INCLUDE_DIRS})
    #target_compile_definitions(${PROJECT_NAME} PRIVATE ${PCL_DEFINITIONS}) causes command-line error in cmake from ubuntu 16.04
    target_compile_definitions(${PROJECT_NAME} PUBLIC WITH_PCL=1)
endif(WITH_PCL)

if(WITH_LAS)
    target_include_directories(${PROJECT_NAME} PRIVATE ${LIBLAS_PATH}/include/)
    target_compile_definitions(${PROJECT_NAME} PUBLIC WITH_LAS=1)
    if(HAVE_LASZIP)
        target_include_directories(${PROJECT_NAME} PRIVATE ${LIBLASZIP_PATH}/include/
                                                           ${LIBLASZIP_PATH}/include/laszip
                                                           ${LIBLASZIP_PATH}/src/)
    endif(HAVE_LASZIP)
endif(WITH_LAS)

qt5_use_modules(Qt3DPointcloudRenderer Qml Widgets Quick 3DCore 3DQuick 3DRender 3DQuickRender)
