cmake_minimum_required (VERSION 2.9)
project (supahincluder CXX C)

set(TARGET_NAME "supahincluder")

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   message(FATAL_ERROR "In-source builds are not allowed.")
endif()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

#dependancies

#NOTHING hurray!!! (well runtime...)

set(CMAKE_INSTALL_OPENMP_LIBRARIES TRUE)
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
include(InstallRequiredSystemLibraries)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(SRCS
  main.cpp
)

set(HDRS

)

add_executable(${TARGET_NAME}
  ${SRCS} ${HDRS}
)

target_link_libraries(${TARGET_NAME})
install(TARGETS ${TARGET_NAME} DESTINATION .)