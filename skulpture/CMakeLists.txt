cmake_minimum_required(VERSION 3.10)

set (NAME skulpturestyle)

set (CMAKE_INCLUDE_CURRENT_DIR ON)

set (CMAKE_AUTOUIC ON)
set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)

find_package (QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package (Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set (HEADERS
  sk_plugin.h
  sk_factory.h
  skulpture_p.h
  skulpture.h
)

set (SOURCES
  skulpture.cpp
)

set (OTHER_FILES
  skulpture.json
)

set_property (SOURCE ${SOURCES} PROPERTY SKIP_AUTOMOC ON)

add_library (${NAME} MODULE
  ${HEADERS}
  ${SOURCES}
  ${OTHER_FILES}
)

set_target_properties (${NAME} PROPERTIES CXX_STANDARD 11)

target_link_libraries (${NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if (UNIX AND NOT APPLE AND NOT CONFIG_DEBUG)
  add_custom_command(TARGET ${NAME} POST_BUILD
    COMMAND strip lib${NAME}.so)
endif ()

if (UNIX AND NOT APPLE)
  install (TARGETS ${NAME} LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt${QT_VERSION_MAJOR}/plugins/styles)
endif ()
