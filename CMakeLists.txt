cmake_minimum_required(VERSION 3.0)
PROJECT(Concurrency CXX)
if(${CMAKE_VERSION} VERSION_LESS 3.8)
    message("CMake current version doesn't support C++ 17 Standard \n"
            "Setting C++17 flag manually. You may considere updating you CMake version")
    set(CMAKE_C_COMPILER gcc-7)
    set(CMAKE_CXX_COMPILER g++-7)
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
    message("Setting C++ 17 Flags manually done !")
else()
    message("your current CMake version support C++ 17")
    set (CMAKE_CXX_STANDARD 17)
    set(CMAKE_C_COMPILER gcc-7)
    set(CMAKE_CXX_COMPILER g++-7)
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}-${VERSION_PATCH})

# configure file
message("CMAKE_BINARY_DIR : " ${CMAKE_BINARY_DIR})
configure_file("config.h.in" "${CMAKE_BINARY_DIR}/config.h")
include_directories("${CMAKE_BINARY_DIR}")
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} include)
set(TARGET Concurrency)
if(NOT INSTALL_PATH)
    set(INSTALL_PATH "$ENV{HOME}/.local/share")
    set(INSTALL_PREFIX "${INSTALL_PATH}")
    message("INSTALL PATH :" ${INSTALL_PATH})
    message("INSTALL_PATH variable not set. Using default installation path :" ${INSTALL_PREFIX})
else()
    set(INSTALL_PREFIX ${INSTALL_PATH}/${TARGET})
    message("INSTALL_PATH variable set. Using user  defined installation path :" ${INSTALL_PREFIX})
endif()
set(TARGET_NAME_VERSION "${TARGET}-${VERSION}")
set(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX})
set(INSTALL_LIB_PATH ${INSTALL_PREFIX}/${TARGET}/lib)
set(INSTALL_INCLUDE_PATH ${INSTALL_LIB_PATH}/${TARGET_NAME_VERSION}/include)
set(TARGET_LIB "${INSTALL_LIB_PATH}/${TARGET_NAME_VERSION}/lib${TARGET}.a")
set(FIND_TARGET_MODULE "Find${TARGET}")
message("CMAKE_INSTALL_PREFIX AFTER : " ${CMAKE_INSTALL_PREFIX})
include_directories(include)
set(PROJECT_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
set (HEADERS ${PROJECT_INCLUDE_DIRS}/active_object.h ${PROJECT_INCLUDE_DIRS}/safequeue.h ${PROJECT_INCLUDE_DIRS}/threadpool.h)
set(SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_library(${TARGET} STATIC ${SOURCES})
set_target_properties(${TARGET} PROPERTIES 
    VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}
    SOVERSION ${VERSION_MAJOR}
)
#target_include_directories(${TARGET} PUBLIC ${PROJECT_INCLUDE_DIRS})

target_include_directories(
    ${TARGET} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src
)
#target_sources(TARGET INTERFACE ${PROJECT_SOURCE_DIR}/src/logpolicy.cpp)
#install(TARGETS ${TARGET} DESTINATION ${INSTALL_LIB_PATH})


install(TARGETS ${TARGET} EXPORT ${TARGET}-targets
    DESTINATION ${INSTALL_LIB_PATH}/${TARGET_NAME_VERSION}
    LIBRARY DESTINATION ${INSTALL_LIB_PATH}/${TARGET_NAME_VERSION}
)

#install(DIRECTORY ${PROJECT_INLUDE_DIRS} DESTINATION ${INSTALL_INCLUDE_PATH})
install(EXPORT ${TARGET}-targets DESTINATION ${INSTALL_LIB_PATH}/${TARGET_NAME_VERSION})
export(TARGETS ${TARGET} FILE ${FIND_TARGET_MODULE}.cmake)

configure_file(
   "${PROJECT_SOURCE_DIR}/pkg/concurrency-config.cmake.in"
    "${INSTALL_LIB_PATH}/${TARGET_NAME_VERSION}/cmake/${FIND_TARGET_MODULE}.cmake" @ONLY
)

configure_file(
    "${PROJECT_SOURCE_DIR}/pkg/concurrency-config-version.cmake.in"
    "${INSTALL_LIB_PATH}/${TARGET_NAME_VERSION}/cmake/${FIND_TARGET_MODULE}-version.cmake" @ONLY
)

install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/include
    DESTINATION "${INSTALL_LIB_PATH}/${TARGET_NAME_VERSION}"
    FILES_MATCHING PATTERN "*.h"
)

install(
    FILES ${PROJECT_BINARY_DIR}/config.h
    DESTINATION "${INSTALL_INCLUDE_PATH}"
)
#install(TARGETS ${TARGET} DESTINATION ${INSTALL_LIB_PATH})

#install(FILES ${HEADERS} DESTINATION ${INSTALL_INCLUDE_PATH})
