INCPATH += ../src

HEADERS += ../src/qxgeditAbout.h \
           ../src/qxgeditOptions.h \
           ../src/qxgeditMainForm.h \
           ../src/qxgeditOptionsForm.h

SOURCES += ../src/main.cpp \
           ../src/qxgeditOptions.cpp \
           ../src/qxgeditMainForm.cpp \
           ../src/qxgeditOptionsForm.cpp

FORMS    = ../src/qxgeditMainForm.ui \
           ../src/qxgeditOptionsForm.ui

RESOURCES += ../icons/qxgedit.qrc

TEMPLATE = app
CONFIG  += qt thread warn_on release
LANGUAGE = C++

win32 {
	CONFIG  += console
	INCPATH += C:\usr\local\include
	LIBS    += -LC:\usr\local\lib
}

LIBS += -lasound
