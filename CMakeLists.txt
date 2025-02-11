cmake_minimum_required(VERSION 3.7)
project(Melt)

set(CMAKE_CXX_STANDARD 17)
set(SDL2_LIB "/Library/Frameworks/SDL2.framework/SDL2")
set(SDL2_IMAGE_LIB "/Library/Frameworks/SDL2_image.framework/SDL2_image")

set(IMGUI_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/vendor/imgui)
file(GLOB IMGUI_SOURCES ${IMGUI_INCLUDE_DIR}/*.cpp)
file(GLOB IMGUI_HEADERS ${IMGUI_INCLUDE_DIR}/*.h)

find_package(SDL2 REQUIRED COMPONENTS SDL2)
find_package(SDL2_image REQUIRED)

include_directories(${PROJECT_NAME} "include")
include_directories(${PROJECT_NAME} "vendor/imgui")
include_directories(${PROJECT_NAME} "vendor/nativefiledialog-extended/src/include")
include_directories(${PROJECT_NAME} "vendor/yaml-cpp/include")
include_directories(${PROJECT_NAME} "/Library/Frameworks/SDL2.framework")
include_directories(${PROJECT_NAME} "/Library/Frameworks/SDL2_image.framework")

#include(FetchContent)
#
#FetchContent_Declare(
#        yaml-cpp
#        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
#        GIT_TAG master  # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
#)
#FetchContent_GetProperties(yaml-cpp)
#
#if(NOT yaml-cpp_POPULATED)
#    message(STATUS "Fetching yaml-cpp...")
#    FetchContent_Populate(yaml-cpp)
#    add_subdirectory(${yaml-cpp_SOURCE_DIR} ${yaml-cpp_BINARY_DIR})
#endif()

add_subdirectory("vendor/nativefiledialog-extended")
add_subdirectory("vendor/yaml-cpp")

add_executable(Melt main.cpp
        Engine.cpp
        include/Engine.h
        EntityManager.cpp
        include/EntityManager.h
        EntityComponent.cpp
        include/EntityComponent.h
        EntitySystem.cpp
        include/EntitySystem.h
        include/EntityRegistry.h
        TileGrid.cpp
        include/TileGrid.h
        Tile.cpp
        include/Tile.h
        TileEditor.cpp
        include/TileEditor.h
        ${IMGUI_SOURCES}
        ${IMGUI_HEADERS}
        Utility.cpp
        include/Utility.h
        Sprite.cpp
        include/Sprite.h
        include/Timer.h
        TileSpriteSheetInfo.cpp
        include/TileSpriteSheetInfo.h
        InputManager.cpp
        include/InputManager.h
        EntityInspector.cpp
        include/EntityInspector.h
        SceneManager.cpp
        include/SceneManager.h)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIB})
target_link_libraries(${PROJECT_NAME} ${SDL2_IMAGE_LIB})
target_link_libraries(${PROJECT_NAME} nfd)
target_link_libraries(${PROJECT_NAME} yaml-cpp::yaml-cpp)
