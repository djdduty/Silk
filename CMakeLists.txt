cmake_minimum_required(VERSION 2.6)
project(Silk)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

set(BUILD_STATIC TRUE CACHE BOOL "TRUE to build static libraries, FALSE to build shared libraries (preferred)")
set(BUILD_GAME TRUE CACHE BOOL "TRUE to build the game, FALSE to not build it")
set(BUILD_TEST TRUE CACHE BOOL "TRUE to build the test game, FALSE to not build it")

set(GLFWDIR "" CACHE PATH "Path to glfw dir")
set(GLEWDIR "" CACHE PATH "Path to glew dir")

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake-extras/")

set(GLFWDIR "${PROJECT_SOURCE_DIR}/dependencies/glfw")
set(GLEWDIR "${PROJECT_SOURCE_DIR}/dependencies/glew")

find_package(OpenGL REQUIRED)

include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/Silk/src/ ${GLFWDIR}/include ${OPENGL_INCLUDE_DIR} ${GLEWDIR}/include )

ADD_SUBDIRECTORY(dependencies)
find_package(X11)

ADD_DEFINITIONS(
    -std=c++11 # Or -std=c++0x
    # Other flags
)

add_subdirectory(${PROJECT_SOURCE_DIR}/Silk)
add_subdirectory(${PROJECT_SOURCE_DIR}/TestClient)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/Silk/src/" DESTINATION include/Silk FILES_MATCHING PATTERN "*.h")
