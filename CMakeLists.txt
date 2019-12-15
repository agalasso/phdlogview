#
#  This file is part of phdlogview
# 
#  Copyright (C) 2016 Andy Galasso
# 
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License along
#  with this program; if not, visit the http://fsf.org website.

cmake_minimum_required (VERSION 2.8.11)

# this must appear very early in the file
if (WIN32)
  set(CMAKE_GENERATOR_TOOLSET "v120_xp" CACHE STRING "Platform Toolset" FORCE)
endif()

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
  if (POLICY CMP0043)
    cmake_policy(SET CMP0043 NEW)
  endif()
endif(COMMAND cmake_policy)

if(APPLE)
  set(APPLE_ARCH "x86_64" CACHE STRING "Apple architecture: i386 or x86_64")
  if(APPLE_ARCH MATCHES "^i386")
    message(STATUS "configuring to build a 32-bit phdlogview")
    set(APPLE32 1)
  elseif(APPLE_ARCH MATCHES "^x86_64")
    message(STATUS "configuring to build a 64-bit phdlogview")
    SET(APPLE32 0)
  else()
    message(FATAL_ERROR "APPLE_ARCH must be i386 or x86_64")
  endif()
  if(APPLE32)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.7" CACHE STRING "Minimum OSX deployment version")
    set(CMAKE_OSX_ARCHITECTURES i386 CACHE STRING "build architecture for OSX" FORCE)
  else()
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14" CACHE STRING "Minimum OSX deployment version")
  endif()
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
  set(CMAKE_CXX_STANDARD 11)
endif()

project (phdlogview)

set(srcdir ${CMAKE_SOURCE_DIR})

include_directories(${CMAKE_CURRENT_BINARY_DIR})
#include_directories(${srcdir})

add_definitions(-D_USE_MATH_DEFINES)

if (WIN32)
  set(FIND_LIBRARY_USE_LIB64_PATHS FALSE)
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
"MinSizeRel" "RelWithDebInfo")
endif()

include(CheckCXXCompilerFlag)

#################################################################################
#
# general defines
if(WIN32)
  set(FIND_LIBRARY_USE_LIB64_PATHS FALSE)
  #set(CMAKE_LIBRARY_ARCHITECTURE x86)
elseif(APPLE32)
  check_cxx_compiler_flag(-stdlib=libc++ HAS_LIBCXX11_FLAG)
  if(HAS_LIBCXX11_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  endif()
elseif(NOT APPLE)
  # c++11 options
  check_cxx_compiler_flag(-std=c++11 HAS_CXX11_FLAG)
  check_cxx_compiler_flag(-std=c++0x HAS_CXX0X_FLAG)

  if(HAS_CXX11_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  elseif(HAS_CXX0X_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  endif()

endif()

if(WIN32)
  # wxWidgets
  set(wxWidgets_CONFIGURATION msw)

  set (WXWIN "C:\\wxWidgets-3.0.4")
  if(NOT WXWIN OR NOT EXISTS ${WXWIN})
    message(FATAL_ERROR "The variable WXWIN should be defined and should point to a valid wxWindows installation path.")
  endif()
  
  set(wxWidgets_ROOT_DIR ${WXWIN})
  set(wxWidgets_USE_STATIC ON)
  set(wxWidgets_USE_DEBUG OFF)
  set(wxWidgets_USE_UNICODE ON)
  find_package(wxWidgets REQUIRED COMPONENTS propgrid base core aui adv html net)
  include(${wxWidgets_USE_FILE})
  message(${wxWidgets_USE_FILE})
else()
  if(wxWidgets_PREFIX_DIRECTORY)
    set(wxWidgets_CONFIG_OPTIONS --prefix=${wxWidgets_PREFIX_DIRECTORY})

    find_program(wxWidgets_CONFIG_EXECUTABLE NAMES "wx-config" PATHS
${wxWidgets_PREFIX_DIRECTORY}/bin NO_DEFAULT_PATH)
    if(NOT wxWidgets_CONFIG_EXECUTABLE)
      message(FATAL_ERROR "Cannot find wxWidgets_CONFIG_EXECUTABLE from the
given directory ${wxWidgets_PREFIX_DIRECTORY}")
    endif()
  endif()

  find_package(wxWidgets REQUIRED COMPONENTS aui core base adv html net)
  if(NOT wxWidgets_FOUND)
    message(FATAL_ERROR "WxWidget cannot be found. Please use wx-config
prefix")
  endif()
  #if(APPLE)
  #  set(PHD_LINK_EXTERNAL ${PHD_LINK_EXTERNAL} wx_osx_cocoau_aui-3.0)
  #endif()
  #message("wxLibraries ${wxWidgets_LIBRARIES}")
endif()

set(APP_LINK_EXTERNAL ${APP_LINK_EXTERNAL} ${wxWidgets_LIBRARIES})

set(SRC
  ${srcdir}/AnalysisWin.cpp
  ${srcdir}/AnalysisWin.h
  ${srcdir}/LogViewApp.cpp
  ${srcdir}/LogViewApp.h
  ${srcdir}/LogViewFrame.cpp
  ${srcdir}/LogViewFrame.h
  ${srcdir}/logparser.cpp
  ${srcdir}/logparser.h
  ${srcdir}/phdlogview.ico
  ${srcdir}/phdlogview.rc
  ${srcdir}/small.ico
)

set (FBSRC
  ${CMAKE_CURRENT_BINARY_DIR}/LogViewFrameBase.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/LogViewFrameBase.h
)

set (HAVE_WXFB TRUE CACHE BOOL "whether to use wxformbuilder")

if (HAVE_WXFB)

  if (WIN32)
    set (WXFB "C:\\Program Files (x86)\\wxFormBuilder\\wxFormBuilder.exe")
    set (WXFB_GENERATE "/g")
  else()
    set (WXFB wxformbuilder)
    set (WXFB_GENERATE "-g")
  endif()

  ADD_CUSTOM_COMMAND(
    OUTPUT ${FBSRC}
    COMMAND ${WXFB} ${WXFB_GENERATE} ${srcdir}/phdlogview.fbp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/phdlogview.fbp
  )
else()
  # no wxFormBuilder, use pre-generated files
  configure_file(${srcdir}/generated/LogViewFrameBase.cpp ${CMAKE_CURRENT_BINARY_DIR}/LogViewFrameBase.cpp COPYONLY)
  configure_file(${srcdir}/generated/LogViewFrameBase.h ${CMAKE_CURRENT_BINARY_DIR}/LogViewFrameBase.h COPYONLY)
endif()

set (ALL_SRC
  ${SRC}
  ${FBSRC}
)

if(WIN32)
  add_executable(phdlogview WIN32 ${ALL_SRC})
elseif(APPLE)
  add_executable(phdlogview MACOSX_BUNDLE ${ALL_SRC})
elseif(UNIX)
  add_executable(phdlogview ${ALL_SRC})
endif()

# ===== GSL =====
if(WIN32)
  set(gsl_ver "2.4.0.8788")
  set(gsl_dir ${CMAKE_BINARY_DIR}/gsl-msvc-x86.${gsl_ver})
  if(NOT EXISTS ${gsl_dir})
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_SOURCE_DIR}/3rdparty/gsl-msvc-x86.${gsl_ver}.zip --format=zip
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
  endif()
  add_definitions(-DGSL_DLL)
  include_directories(${gsl_dir}/build/native)
#  set(gsl_lib_dir ${gsl_dir}/build/native/dll/Win32/${CMAKE_BUILD_TYPE})  # requies vcruntime140d.dll
  set(gsl_lib_dir ${gsl_dir}/build/native/dll/Win32/Release)
  set(APP_LINK_EXTERNAL ${APP_LINK_EXTERNAL} ${gsl_lib_dir}/gsl.lib)
  add_custom_command(TARGET phdlogview POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${gsl_lib_dir}/gsl.dll"
      "${gsl_lib_dir}/cblas.dll"
      "${CMAKE_SOURCE_DIR}/3rdparty/msvcp120.dll"
      "${CMAKE_SOURCE_DIR}/3rdparty/msvcr120.dll"
      "${CMAKE_SOURCE_DIR}/3rdparty/vcruntime140.dll"
      $<TARGET_FILE_DIR:phdlogview>)
else()
  find_package(GSL REQUIRED)
  if(APPLE)
    # force static loining on OSX to avoid redistributing GSL .dylib files
    # this is a hack... what's the real way to do this in CMake?
    string(REPLACE ".dylib" ".a" GSL_LIBRARIES "${GSL_LIBRARIES}")
    set(APP_LINK_EXTERNAL ${APP_LINK_EXTERNAL} ${GSL_LIBRARIES})
    include_directories(${GSL_INCLUDE_DIR})
  else()
    target_link_libraries(phdlogview GSL::gsl GSL::gslcblas)
  endif()
endif()

target_compile_definitions( phdlogview PRIVATE "${wxWidgets_DEFINITIONS}" "HAVE_TYPE_TRAITS")
target_compile_options(     phdlogview PRIVATE "${wxWidgets_CXX_FLAGS};")
target_link_libraries(phdlogview ${APP_LINK_EXTERNAL})
target_include_directories(phdlogview PRIVATE ${wxWidgets_INCLUDE_DIRS})

add_custom_command(TARGET phdlogview POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/LICENSE.txt"
    "${CMAKE_SOURCE_DIR}/README.txt"
    $<TARGET_FILE_DIR:phdlogview>)

if(UNIX AND NOT APPLE)
  install(TARGETS phdlogview
          RUNTIME DESTINATION bin)
  install(FILES ${CMAKE_SOURCE_DIR}/phdlogview.png
          DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/ )
  install(FILES ${CMAKE_SOURCE_DIR}/phdlogview.desktop
          DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications/ )
endif()
