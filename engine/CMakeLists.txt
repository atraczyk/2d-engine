cmake_minimum_required(VERSION 3.15)

project(engine)

include(GNUInstallDirs)

# Add preprocessor definitions to avoid common issues
add_definitions(
    -DNOMINMAX
    -D_CRT_SECURE_NO_WARNINGS
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SOURCE
    ${SOURCE_DIR}/game.cpp
    ${SOURCE_DIR}/camera.cpp
    ${SOURCE_DIR}/entity.cpp
    ${SOURCE_DIR}/input.cpp
    ${SOURCE_DIR}/menu.cpp
    ${SOURCE_DIR}/objects.cpp
    ${SOURCE_DIR}/point.cpp
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
    ${SOURCE_DIR}/program.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE})

# Find required packages
find_package(glew CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

# Add include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    ${SOURCE_DIR}
    ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}
)

set(EXTRA_LIBS "")
if(WIN32)
    list(APPEND EXTRA_LIBS
        UxTheme
        hid
        setupapi
    )
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC
    GLEW::GLEW
    OpenGL::GL
    SDL2::SDL2
    SDL2::SDL2main
    ${EXTRA_LIBS}
)

# Resource deployment
set(RESOURCE_DIRS
    textures
    shaders
    levels
    sprites
    sounds
)

# Install binary to CMAKE_INSTALL_BINDIR (usually "bin")
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install resource folders to same dir as engine.exe
foreach(DIR IN LISTS RESOURCE_DIRS)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/${DIR}
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        FILES_MATCHING PATTERN "*.*"
    )
endforeach()

# Keep assets runnable from the build tree as `<exe_dir>/resources/...`.
foreach(DIR IN LISTS RESOURCE_DIRS)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/resources/${DIR}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/${DIR}
    )
endforeach()

# Copy top-level resource files used by startup.
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/resources/config.ini
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/config.ini
)

# Optional: install README and LICENSE next to exe
install(FILES
    ${CMAKE_SOURCE_DIR}/README.md
    ${CMAKE_SOURCE_DIR}/LICENSE
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    OPTIONAL
)

install(FILES
    ${CMAKE_SOURCE_DIR}/resources/config.ini
    DESTINATION ${CMAKE_INSTALL_BINDIR}/resources
    OPTIONAL
)
