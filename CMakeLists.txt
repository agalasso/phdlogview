cmake_minimum_required (VERSION 2.6)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project (phdlogview)

set(srcdir ${CMAKE_SOURCE_DIR})

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_definitions(-D_USE_MATH_DEFINES)

if (WIN32)
  set(FIND_LIBRARY_USE_LIB64_PATHS FALSE)
endif()

if(WIN32)
  # wxWidgets
  set(wxWidgets_CONFIGURATION msw)

  set (WXWIN "C:\\wxWidgets-3.0.2")
  if(NOT WXWIN OR NOT EXISTS ${WXWIN})
    message(FATAL_ERROR "The variable WXWIN should be defined and should point to a valid wxWindows installation path.")
  endif()
  
  set(wxWidgets_ROOT_DIR ${WXWIN})
  set(wxWidgets_USE_STATIC ON)
  set(wxWidgets_USE_DEBUG ON)
  set(wxWidgets_USE_UNICODE OFF)
  find_package(wxWidgets REQUIRED COMPONENTS propgrid base core aui adv html net)
  include(${wxWidgets_USE_FILE})
  message(${wxWidgets_USE_FILE})
endif()

set(APP_LINK_EXTERNAL ${APP_LINK_EXTERNAL} ${wxWidgets_LIBRARIES})

set(SRC
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

ADD_CUSTOM_COMMAND(
  OUTPUT ${FBSRC}
  COMMAND "C:\\Program Files (x86)\\wxFormBuilder\\wxFormBuilder.exe" /g ${srcdir}/phdlogview.fbp
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/phdlogview.fbp
)

set (ALL_SRC
  ${SRC}
  ${FBSRC}
)

add_executable(phdlogview WIN32 ${ALL_SRC})

target_compile_definitions( phdlogview PRIVATE "${wxWidgets_DEFINITIONS}" "HAVE_TYPE_TRAITS")
target_compile_options(     phdlogview PRIVATE "${wxWidgets_CXX_FLAGS};")
target_link_libraries(phdlogview ${APP_LINK_EXTERNAL})
target_include_directories(phdlogview PRIVATE ${wxWidgets_INCLUDE_DIRS})
