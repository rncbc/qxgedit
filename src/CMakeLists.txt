# project (qxgedit)

set (CMAKE_INCLUDE_CURRENT_DIR ON)

set (CMAKE_AUTOUIC ON)
set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/config.h)
  file (REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/config.h)
endif ()
configure_file (config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

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
  qxgeditPaletteForm.h
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
  qxgeditPaletteForm.cpp
  qxgeditMainForm.cpp
  qxgedit.cpp
)

set (FORMS
  qxgeditOptionsForm.ui
  qxgeditPaletteForm.ui
  qxgeditMainForm.ui
)

set (RESOURCES
  qxgedit.qrc
)

if (WIN32)
  set (RC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/win32/${PROJECT_NAME}.rc)
  set (RES_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.res.obj)
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
  set (ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/images/${PROJECT_NAME}.icns)
  list (APPEND SOURCES ${ICON_FILE})
  set (MACOSX_BUNDLE_ICON_FILE ${PROJECT_NAME}.icns)
  set_source_files_properties (${ICON_FILE} PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources)
endif ()


add_executable (${PROJECT_NAME}
  ${HEADERS}
  ${SOURCES}
  ${FORMS}
  ${RESOURCES}
)

# Add some debugger flags.
if (CONFIG_DEBUG AND UNIX AND NOT APPLE)
  set (CONFIG_DEBUG_OPTIONS -g -fsanitize=address -fno-omit-frame-pointer)
  target_compile_options (${PROJECT_NAME} PRIVATE ${CONFIG_DEBUG_OPTIONS})
  target_link_options (${PROJECT_NAME} PRIVATE ${CONFIG_DEBUG_OPTIONS})
endif ()

set_target_properties (${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

if (WIN32)
  set_target_properties (${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE true)
endif ()

if (APPLE)
  set_target_properties (${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE true)
endif ()

target_link_libraries (${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Svg)

if (CONFIG_XUNIQUE)
  target_link_libraries (${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Network)
endif ()


if (CONFIG_ALSA_MIDI)
  target_link_libraries (${PROJECT_NAME} PRIVATE PkgConfig::ALSA)
endif ()

if (CONFIG_RTMIDI)
  target_link_libraries (${PROJECT_NAME} PRIVATE PkgConfig::RTMIDI)
endif ()


if (UNIX AND NOT APPLE)
  install (TARGETS ${PROJECT_NAME} RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR})
  install (FILES images/${PROJECT_NAME}.png
    RENAME org.rncbc.${PROJECT_NAME}.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps)
  install (FILES images/${PROJECT_NAME}.svg
    RENAME org.rncbc.${PROJECT_NAME}.svg
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)
  install (FILES appdata/org.rncbc.${PROJECT_NAME}.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
  install (FILES appdata/org.rncbc.${PROJECT_NAME}.metainfo.xml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
  install (FILES man1/${PROJECT_NAME}.1
    DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
  install (FILES man1/${PROJECT_NAME}.fr.1
    DESTINATION ${CMAKE_INSTALL_MANDIR}/fr/man1 RENAME ${PROJECT_NAME}.1)
  install (FILES palette/KXStudio.conf palette/Wonton\ Soup.conf
    DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/palette)
endif ()

if (WIN32)
  install (TARGETS ${PROJECT_NAME} RUNTIME
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif ()
