CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
PROJECT(raycart)

INCLUDE(FetchContent)
IF(POLICY CMP0135)
  CMAKE_POLICY(SET CMP0135 NEW)
  SET(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
ENDIF()

option(BUILD_HOST "Build host target" ON)
option(BUILD_CARTS "Build carts target" ON)

if(BUILD_HOST)
  add_subdirectory(host)
endif()

if(BUILD_CARTS AND NOT EMSCRIPTEN)
  add_subdirectory(carts)
endif()