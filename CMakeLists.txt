cmake_minimum_required(VERSION 3.26)
project(etu-opengl LANGUAGES C CXX)

set(EXEC_NAME hackmore)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXTERNAL_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(SRC_DIR         ${CMAKE_CURRENT_SOURCE_DIR}/src)

option(ETUGL_BUILD_EXAMPLES "ETUOpenGL build examples" OFF)
add_subdirectory(${EXTERNAL_DIR}/etu-opengl)
add_subdirectory(${EXTERNAL_DIR}/json-loader)

file(GLOB_RECURSE CPP_SOURCES ${SRC_DIR}/*.cpp)

add_executable(${EXEC_NAME} ${CPP_SOURCES})
target_link_libraries(${EXEC_NAME} PUBLIC etu-opengl nlohmann_json::nlohmann_json)
target_include_directories(${EXEC_NAME} PUBLIC ${SUBDIR_PATH}) 

include(${EXTERNAL_DIR}/etu-opengl/cmake/utils.cmake)
copy_resources(${EXEC_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
copy_resources(${EXEC_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/assets)
link_shaders(${EXEC_NAME} ${EXTERNAL_DIR}/etu-opengl)
