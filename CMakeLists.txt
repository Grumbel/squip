# squip - Squirrel Utility Library
# Copyright (C) 2020 Ingo Ruhnke <grumbel@gmail.com>
#
# This software is provided 'as-is', without any express or implied
# warranty.  In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.

cmake_minimum_required(VERSION 3.6)
project(squip VERSION 0.0.0)

include(mk/cmake/TinyCMMC.cmake)

find_package(squirrel 3.2 REQUIRED)

file(GLOB SQUIP_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  include/squip/*.hpp)

file(GLOB SQUIP_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/*.cpp)

add_library(squip STATIC
  ${SQUIP_SOURCES}
  )
target_include_directories(squip PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  )
# target_link_libraries(squip INTERFACE glm::glm)
set_target_properties(squip PROPERTIES PUBLIC_HEADER
  "${SQUIP_HEADERS}"
  )

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/squip.pc.in"
  "${CMAKE_CURRENT_BINARY_DIR}/squip.pc"
  @ONLY
  )

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/squip.pc"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
  )

if(BUILD_TESTS)
  enable_testing()
  find_package(GTest REQUIRED)

  # build automatic tests
  file(GLOB TEST_SQUIP_SOURCES tests/*_test.cpp)
  add_executable(test_squip ${TEST_SQUIP_SOURCES})
  target_compile_options(test_squip PRIVATE ${TINYCMMC_WARNINGS_CXX_FLAGS})
  target_link_libraries(test_squip
    GTest::GTest
    GTest::Main
    squip
    squirrel
    )

  add_test(NAME test_squip
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND test_squip
    )
endif()

tinycmmc_export_and_install_library(squip)

# EOF #
