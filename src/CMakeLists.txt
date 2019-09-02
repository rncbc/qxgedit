set (NAME qxgedit)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

configure_file (cmake_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set (HEADERS
  XGParam.h
  XGParamObserver.h
  XGParamWidget.h
  XGParamSysex.h
  qxgeditXGMasterMap.h
  qxgeditAbout.h
  qxgeditAmpEg.h
  qxgeditCheck.h
  qxgeditCombo.h
  qxgeditDial.h
  qxgeditDrop.h
  qxgeditDrumEg.h
  qxgeditEdit.h
  qxgeditFilter.h
  qxgeditKnob.h
  qxgeditPartEg.h
  qxgeditPitch.h
  qxgeditScale.h
  qxgeditSpin.h
  qxgeditUserEg.h
  qxgeditVibra.h
  qxgeditMidiDevice.h
  qxgeditMidiRpn.h
  qxgeditOptions.h
  qxgeditOptionsForm.h
  qxgeditMainForm.h
  qxgedit.h
)

set (SOURCES
  XGParam.cpp
  XGParamObserver.cpp
  XGParamWidget.cpp
  XGParamSysex.cpp
  qxgeditXGMasterMap.cpp
  qxgeditAmpEg.cpp
  qxgeditCheck.cpp
  qxgeditCombo.cpp
  qxgeditDial.cpp
  qxgeditDrop.cpp
  qxgeditDrumEg.cpp
  qxgeditEdit.cpp
  qxgeditFilter.cpp
  qxgeditKnob.cpp
  qxgeditPartEg.cpp
  qxgeditPitch.cpp
  qxgeditScale.cpp
  qxgeditSpin.cpp
  qxgeditUserEg.cpp
  qxgeditVibra.cpp
  qxgeditMidiDevice.cpp
  qxgeditMidiRpn.cpp
  qxgeditOptions.cpp
  qxgeditOptionsForm.cpp
  qxgeditMainForm.cpp
  qxgedit.cpp
)

set (FORMS
  qxgeditOptionsForm.ui
  qxgeditMainForm.ui
)

set (RESOURCES
  qxgedit.qrc
)

qt5_wrap_ui (UI_SOURCES ${FORMS})
qt5_wrap_cpp (MOC_SOURCES ${HEADERS})
qt5_add_resources (QRC_SOURCES ${RESOURCES})

if (WIN32)
  set (RC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/win32/${NAME}.rc)
  set (RES_FILE ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.res.obj)
  find_program (WINDRES_EXECUTABLE NAMES windres mingw32-windres i686-mingw32-windres)
  if (MINGW)
    exec_program (${WINDRES_EXECUTABLE}
      ARGS "-i ${RC_FILE} -o ${RES_FILE} --include-dir=${CMAKE_CURRENT_SOURCE_DIR}/images")
    list (APPEND SOURCES ${RES_FILE})
  else ()
    list (APPEND SOURCES ${RC_FILE})
  endif ()
endif ()

if (APPLE)
  set (ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/images/${NAME}.icns)
  list (APPEND SOURCES ${ICON_FILE})
  set (MACOSX_BUNDLE_ICON_FILE ${NAME}.icns)
  set_source_files_properties (${ICON_FILE} PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources)
endif ()


add_executable (${NAME}
  ${UI_SOURCES}
  ${MOC_SOURCES}
  ${QRC_SOURCES}
  ${SOURCES}
)

set_target_properties (${NAME} PROPERTIES CXX_STANDARD 11)

if (WIN32)
  set_target_properties (${NAME} PROPERTIES WIN32_EXECUTABLE true)
endif ()

if (APPLE)
  set_target_properties (${NAME} PROPERTIES MACOSX_BUNDLE true)
endif ()

target_link_libraries (${NAME} PRIVATE Qt5::Widgets)

if (CONFIG_XUNIQUE)
  target_link_libraries (${NAME} PRIVATE Qt5::Network)
endif ()

if (CONFIG_ALSA_SEQ)
  target_link_libraries (${NAME} PRIVATE ${ALSA_LIBRARIES})
endif ()


if (UNIX AND NOT APPLE)
  install (TARGETS ${NAME} RUNTIME
     DESTINATION ${CMAKE_INSTALL_BINDIR})
  install (FILES ${NAME}.desktop
     DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
  install (FILES images/${NAME}.png
     DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps)
  install (FILES appdata/${NAME}.appdata.xml
     DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
endif ()

if (WIN32)
   install (TARGETS ${NAME} RUNTIME
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif ()
