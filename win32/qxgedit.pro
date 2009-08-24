INCPATH += ../src

HEADERS += ../src/XGParam.h \
           ../src/XGParamObserver.h \
           ../src/XGParamWidget.h \
           ../src/qxgeditXGParamMap.h \
           ../src/qxgeditAbout.h \
           ../src/qxgeditCheck.h \
           ../src/qxgeditCombo.h \
           ../src/qxgeditDial.h \
           ../src/qxgeditDrop.h \
           ../src/qxgeditDrumEg.h \
           ../src/qxgeditFilter.h \
           ../src/qxgeditKnob.h \
           ../src/qxgeditPartEg.h \
           ../src/qxgeditPitch.h \
           ../src/qxgeditSpin.h \
           ../src/qxgeditVibra.h \
           ../src/qxgeditMidiDevice.h \
           ../src/qxgeditOptions.h \
           ../src/qxgeditOptionsForm.h \
           ../src/qxgeditMainForm.h

SOURCES += ../src/XGParam.cpp \
           ../src/XGParamObserver.cpp \
           ../src/XGParamWidget.cpp \
           ../src/qxgeditXGParamMap.cpp \
           ../src/qxgeditCheck.cpp \
           ../src/qxgeditCombo.cpp \
           ../src/qxgeditDial.cpp \
           ../src/qxgeditDrop.cpp \
           ../src/qxgeditDrumEg.cpp \
           ../src/qxgeditFilter.cpp \
           ../src/qxgeditKnob.cpp \
           ../src/qxgeditPartEg.cpp \
           ../src/qxgeditPitch.cpp \
           ../src/qxgeditSpin.cpp \
           ../src/qxgeditVibra.cpp \
           ../src/qxgeditMidiDevice.cpp \
           ../src/qxgeditOptions.cpp \
           ../src/qxgeditOptionsForm.cpp \
           ../src/qxgeditMainForm.cpp \
           ../src/main.cpp

FORMS    = ../src/qxgeditOptionsForm.ui \
           ../src/qxgeditMainForm.ui

RESOURCES += ../icons/qxgedit.qrc

TEMPLATE = app
CONFIG  += qt thread warn_on debug
LANGUAGE = C++

win32 {
	CONFIG  += console
	INCPATH += C:\usr\local\include
	LIBS    += -LC:\usr\local\lib
}

LIBS += -lasound
