cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(window LANGUAGES C)

option(PLATFORM "Platform target")
if (${PLATFORM} MATCHES "OFF")
  string(TOUPPER ${CMAKE_SYSTEM_NAME} PLATFORM_UPPER)
  set(LIBWINDOW_OS_${PLATFORM_UPPER} "ON")
else()
  string(TOUPPER ${PLATFORM} PLATFORM_UPPER)
  set(LIBWINDOW_OS_${PLATFORM_UPPER} "ON")
endif()

# Dependencies
if (LIBWINDOW_OS_LINUX)
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
  list(APPEND PLATFORM_SOURCES "./src/window_linux.c")
elseif(LIBWINDOW_OS_WINDOWS)
  message(FATAL_ERROR "not implemented yet")
endif()

# Library
add_library(window ${PLATFORM_SOURCES})
add_dependencies(window sized_types)
target_link_libraries(window sized_types ${PLATFORM_LIBS})
target_include_directories(window PUBLIC "./src")
target_compile_options(window
  PRIVATE "-std=c90"
  PRIVATE "-pedantic-errors"
  PRIVATE "-Wall"
  PRIVATE "-Wconversion"
  )
