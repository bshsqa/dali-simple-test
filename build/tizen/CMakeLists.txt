cmake_minimum_required(VERSION 3.8.2)
cmake_policy(SET CMP0012 NEW)
if(POLICY CMP0177)
  cmake_policy(SET CMP0177 NEW)
endif()

project(dali-simple-test CXX)

set(APP_NAME accessibility-refactoring-test.example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_DEBUG "Enable debug build options" OFF)
option(ENABLE_EXPORTALL "Export all symbols" OFF)
option(ENABLE_PKG_CONFIGURE "Use pkg-config to find DALi libraries" ON)
option(TIZEN "Build as a Tizen application" OFF)

set(ROOT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(PREFIX ${CMAKE_INSTALL_PREFIX})

if(DEFINED DALI_APP_DIR)
  set(BINDIR ${DALI_APP_DIR})
else()
  set(BINDIR ${PREFIX}/bin)
endif()

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(ENABLE_DEBUG ON)
endif()

if(ENABLE_DEBUG)
  message(STATUS "CMAKE_BUILD_TYPE: Debug")
  set(ENABLE_EXPORTALL ON)
else()
  message(STATUS "CMAKE_BUILD_TYPE: Release")
endif()

if(ENABLE_PKG_CONFIGURE)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(REQUIRED_PKGS REQUIRED
    dali2-core
    dali2-adaptor
    dali2-toolkit
  )

  foreach(flag ${REQUIRED_PKGS_CFLAGS})
    set(REQUIRED_CFLAGS "${REQUIRED_CFLAGS} ${flag}")
  endforeach()

  set(REQUIRED_LIBS ${REQUIRED_PKGS_LDFLAGS})
endif()

if(UNIX)
  if(NOT ${ENABLE_EXPORTALL})
    add_definitions("-DHIDE_DALI_INTERNALS")
  endif()

  set(DALI_SIMPLE_TEST_CFLAGS "-Wall -Wno-deprecated -fPIE")
  if(NOT ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    set(DALI_SIMPLE_TEST_CFLAGS "${DALI_SIMPLE_TEST_CFLAGS} -Werror")
  endif()
  list(APPEND REQUIRED_LIBS -pie)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${REQUIRED_CFLAGS} ${DALI_SIMPLE_TEST_CFLAGS}")

include_directories(
  ${ROOT_SRC_DIR}
)

link_directories(${LIB_DIR})

add_executable(${APP_NAME}
  ${ROOT_SRC_DIR}/src/accessibility-refactoring-test.example.cpp
)

target_link_libraries(${APP_NAME}
  ${REQUIRED_LIBS}
)

install(TARGETS ${APP_NAME} DESTINATION ${BINDIR})

message(" Folder DALI_APP_DIR : [${BINDIR}]")
message(" Current Build Tizen : [${TIZEN}]")
