project(Qt3DPointcloudRenderer_example)
cmake_minimum_required(VERSION 2.8)

# Note, that this is pcl-free

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt53DCore REQUIRED)
find_package(Qt53DInput REQUIRED)
find_package(Qt53DLogic REQUIRED)
find_package(Qt53DQuick REQUIRED)
find_package(Qt53DRender REQUIRED)
find_package(Qt53DQuickInput REQUIRED)
find_package(Qt53DQuickRender REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/src/)

QT5_ADD_RESOURCES(RESOURCES_RCC qml.qrc)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)
set(QML
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/main.qml
)
file(GLOB_RECURSE SHADER
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/*
)
file(GLOB_RECURSE DATA
    ${CMAKE_CURRENT_SOURCE_DIR}/data/*
)

add_executable(Qt3DPointcloudRenderer_example ${SOURCE} ${RESOURCES_RCC} ${QML} ${SHADER} ${data})

target_link_libraries(Qt3DPointcloudRenderer_example Qt3DPointcloudRenderer)

qt5_use_modules(Qt3DPointcloudRenderer_example
                                               Gui
                                               Widgets
                                               Qml
                                               Quick
                                               3DCore
                                               3DInput
                                               3DLogic
                                               3DQuick
                                               3DRender
                                               3DQuickInput
                                               3DQuickRender)

execute_process(
  COMMAND "${CMAKE_COMMAND}" "-E" "create_symlink" ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
)
