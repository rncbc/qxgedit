INCPATH += ../src

HEADERS += ../src/XGParam.h \
           ../src/XGParamWidget.h \
           ../src/qxgeditAbout.h \
           ../src/qxgeditAmpEg.h \
           ../src/qxgeditDial.h \
           ../src/qxgeditFilter.h \
           ../src/qxgeditKnob.h \
           ../src/qxgeditPitch.h \
           ../src/qxgeditSpin.h \
           ../src/qxgeditVibra.h \
           ../src/qxgeditMidiDevice.h \
           ../src/qxgeditOptions.h \
           ../src/qxgeditOptionsForm.h \
           ../src/qxgeditMainForm.h

SOURCES += ../src/XGParam.cpp \
           ../src/XGParamWidget.cpp \
           ../src/qxgeditAmpEg.cpp \
           ../src/qxgeditDial.cpp \
           ../src/qxgeditFilter.cpp \
           ../src/qxgeditKnob.cpp \
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
