cmake_minimum_required (VERSION 2.9)
project (file_to_obj CXX C)

set(TARGET_NAME "file_to_obj")

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   message(FATAL_ERROR "In-source builds are not allowed.")
endif()

#dependancies

#NOTHING hurray!!! (well runtime...)

set(CMAKE_INSTALL_OPENMP_LIBRARIES TRUE)
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
include(InstallRequiredSystemLibraries)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_executable(${TARGET_NAME} main.cpp )

install(TARGETS ${TARGET_NAME} DESTINATION .)
install(FILES assets/EmbedFileFTO.cmake DESTINATION .)

add_subdirectory(tests/c)
add_subdirectory(tests/cpp)