# qxgedit.pro
#
NAME = qxgedit

TARGET = $${NAME}
TEMPLATE = app

include(src.pri)

#DEFINES += DEBUG

HEADERS += config.h \
	XGParam.h \
	XGParamObserver.h \
	XGParamWidget.h \
	XGParamSysex.h \
	qxgeditXGMasterMap.h \
	qxgeditAbout.h \
	qxgeditAmpEg.h \
	qxgeditCheck.h \
	qxgeditCombo.h \
	qxgeditDial.h \
	qxgeditDrop.h \
	qxgeditDrumEg.h \
	qxgeditEdit.h \
	qxgeditFilter.h \
	qxgeditKnob.h \
	qxgeditPartEg.h \
	qxgeditPitch.h \
	qxgeditScale.h \
	qxgeditSpin.h \
	qxgeditUserEg.h \
	qxgeditVibra.h \
	qxgeditMidiDevice.h \
	qxgeditMidiRpn.h \
	qxgeditOptions.h \
	qxgeditOptionsForm.h \
	qxgeditMainForm.h

SOURCES += \
	XGParam.cpp \
	XGParamObserver.cpp \
	XGParamWidget.cpp \
	XGParamSysex.cpp \
	qxgeditXGMasterMap.cpp \
	qxgeditAmpEg.cpp \
	qxgeditCheck.cpp \
	qxgeditCombo.cpp \
	qxgeditDial.cpp \
	qxgeditDrop.cpp \
	qxgeditDrumEg.cpp \
	qxgeditEdit.cpp \
	qxgeditFilter.cpp \
	qxgeditKnob.cpp \
	qxgeditPartEg.cpp \
	qxgeditPitch.cpp \
	qxgeditScale.cpp \
	qxgeditSpin.cpp \
	qxgeditUserEg.cpp \
	qxgeditVibra.cpp \
	qxgeditMidiDevice.cpp \
	qxgeditMidiRpn.cpp \
	qxgeditOptions.cpp \
	qxgeditOptionsForm.cpp \
	qxgeditMainForm.cpp \
	qxgedit.cpp

FORMS += \
	qxgeditOptionsForm.ui \
	qxgeditMainForm.ui

RESOURCES += qxgedit.qrc


unix {

	# variables
	OBJECTS_DIR = .obj
	MOC_DIR     = .moc
	UI_DIR      = .ui

	isEmpty(PREFIX) {
		PREFIX = /usr/local
	}

	BINDIR = $${PREFIX}/bin
	DATADIR = $${PREFIX}/share

	DEFINES += DATADIR=\"$${DATADIR}\"

	# make install
	INSTALLS += target desktop icon appdata

	target.path = $${BINDIR}

	desktop.path = $${DATADIR}/applications
	desktop.files += $${NAME}.desktop

	icon.path = $${DATADIR}/icons/hicolor/32x32/apps
	icon.files += images/$${NAME}.png

	appdata.path = $${DATADIR}/appdata
	appdata.files += appdata/$${NAME}.appdata.xml
}


# QT5 support
!lessThan(QT_MAJOR_VERSION, 5) {
	QT += widgets
	unix:!macx {
		QT += x11extras
	}
}
