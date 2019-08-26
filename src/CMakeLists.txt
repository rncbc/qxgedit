
include_directories (
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${QT_INCLUDES}
)

link_directories (
  ${CMAKE_CURRENT_BINARY_DIR}
  ${QT_LIBRARY_DIR}
)

configure_file (cmake_config.h ${CMAKE_CURRENT_BINARY_DIR}/config.h)

configure_file (qxgedit.desktop ${CMAKE_CURRENT_BINARY_DIR}/qxgedit.desktop)

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

add_executable (qxgedit
  ${UI_SOURCES}
  ${MOC_SOURCES}
  ${QRC_SOURCES}
  ${SOURCES}
)

target_link_libraries (qxgedit PRIVATE Qt5::Widgets)

if (CONFIG_XUNIQUE)
  target_link_libraries (qxgedit PRIVATE Qt5::Network)
endif ()

if (CONFIG_ALSA_SEQ)
  target_link_libraries (qxgedit PRIVATE ${ALSA_LIBRARIES})
endif ()


if (UNIX AND NOT APPLE)
  install (TARGETS qxgedit RUNTIME
     DESTINATION ${CMAKE_INSTALL_BINDIR})
  install (FILES ${CMAKE_CURRENT_BINARY_DIR}/qxgedit.desktop
     DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
  install (FILES images/qxgedit.png
     DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps)
  install (FILES appdata/qxgedit.appdata.xml
     DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
endif ()
