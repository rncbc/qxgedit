set (NAME skulpturestyle)

include_directories (
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${QT_INCLUDES}
)

link_directories (
  ${CMAKE_CURRENT_BINARY_DIR}
  ${QT_LIBRARY_DIR}
)

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

qt5_wrap_cpp (MOC_SOURCES ${HEADERS})

add_library (${NAME} MODULE
  ${SOURCES}
  ${OTHER_FILES}
)

target_compile_options (${NAME} PRIVATE -std=c++11)

target_link_libraries (${NAME} PUBLIC Qt5::Widgets)


if (UNIX AND NOT APPLE)
  install (TARGETS ${NAME} LIBRARY
     DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt5/plugins/styles)
endif ()
