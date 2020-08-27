cmake_minimum_required (VERSION 3.13)

project (engine)

set (SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SOURCE
    ${SOURCE_DIR}/game.cpp
    ${SOURCE_DIR}/camera.cpp
    ${SOURCE_DIR}/entity.cpp
    ${SOURCE_DIR}/input.cpp
    ${SOURCE_DIR}/menu.cpp
    ${SOURCE_DIR}/objects.cpp
    ${SOURCE_DIR}/point.cpp
    ${SOURCE_DIR}/graphics.cpp
    ${SOURCE_DIR}/rectangle.cpp
    ${SOURCE_DIR}/rendertarget.cpp
    ${SOURCE_DIR}/sound.cpp
    ${SOURCE_DIR}/time.cpp
    ${SOURCE_DIR}/world.cpp
    ${SOURCE_DIR}/text.cpp
    ${SOURCE_DIR}/textfile.cpp
    ${SOURCE_DIR}/texture.cpp
    ${SOURCE_DIR}/tile.cpp
    ${SOURCE_DIR}/utils.cpp
    ${SOURCE_DIR}/vector.cpp
)

set(CMAKE_CXX_STANDARD 14)

add_executable (${PROJECT_NAME} WIN32 ${SOURCE})

hunter_add_package(glew)
find_package(glew CONFIG REQUIRED)

find_package(OpenGL REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC ${SOURCE_DIR})

set(EXTRA_LIBS
    UxTheme
    hid
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    glew::glew
    ${EXTRA_LIBS}
)
