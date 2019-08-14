cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(window LANGUAGES C)

if(NOT TARGET_OS)
  string(TOLOWER ${CMAKE_SYSTEM_NAME} TARGET_OS)
endif()

option(PLATFORM "Platform target")
if (${PLATFORM} MATCHES "OFF")
  string(TOUPPER ${CMAKE_SYSTEM_NAME} PLATFORM_UPPER)
  set(LIBWINDOW_OS_${PLATFORM_UPPER} "ON")
else()
  string(TOUPPER ${PLATFORM} PLATFORM_UPPER)
  set(LIBWINDOW_OS_${PLATFORM_UPPER} "ON")
endif()

# Platform
if(TARGET_OS MATCHES "linux")
  # Dependencies
  find_library(LIB_XCB     libxcb.a     PATHS "./deps" NO_DEFAULT_PATH)
  find_library(LIB_XCB_SHM libxcb-shm.a PATHS "./deps" NO_DEFAULT_PATH)
  find_library(LIB_XAU     libXau.a     PATHS "./deps" NO_DEFAULT_PATH)
  find_library(LIB_XDMCP   libXdmcp.a   PATHS "./deps" NO_DEFAULT_PATH)
  list(APPEND PLATFORM_LIBS
    ${LIB_XCB}
    ${LIB_XCB_SHM}
    ${LIB_XAU}
    ${LIB_XDMCP}
    )

  # Sources
  list(APPEND PLATFORM_SOURCES "./src/window_linux.c")
elseif(TARGET_OS MATCHES "windows")
  message(FATAL_ERROR "not implemented yet")
else()
  message(FATAL_ERROR "unsupported platform `${TARGET_OS}`")
endif()

# Library
add_library(window ${PLATFORM_SOURCES})

if(TARGET_OS MATCHES "linux")
  target_compile_definitions(window PUBLIC TARGET_OS_LINUX)
elseif(TARGET_OS MATCHES "windows")
  target_compile_definitions(window PUBLIC TARGET_OS_WINDOWS)
endif()

# Options
add_dependencies(window sized_types)
target_link_libraries(window sized_types ${PLATFORM_LIBS})
target_include_directories(window PUBLIC "./src")
target_compile_options(window
  PRIVATE "-std=c90"
  PRIVATE "-pedantic-errors"
  PRIVATE "-Wall"
  PRIVATE "-Wconversion"
  )
