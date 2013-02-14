# skulpture.pro
#
NAME = skulpturestyle

TARGET = $${NAME}
TEMPLATE = lib
CONFIG	+= shared plugin

include(skulpture.pri)

HEADERS	+= \
	sk_factory.h \
	skulpture_p.h \
	skulpture.h

SOURCES	+= skulpture.cpp

!lessThan(QT_MAJOR_VERSION, 5) {
	OTHER_FILES += skulpture.json
	QT += widgets
}

unix {

	# variables
	OBJECTS_DIR = .obj
	MOC_DIR     = .moc
	UI_DIR      = .ui

	isEmpty(PREFIX) {
		PREFIX = /usr/local
	}

	TARGET_ARCH = $$system(uname -m)
	contains(TARGET_ARCH, x86_64) {
		TARGET_PREFIX = $${PREFIX}/lib64
	} else {
		TARGET_PREFIX = $${PREFIX}/lib
	}

	lessThan(QT_MAJOR_VERSION, 5) {
		TARGET_SUFFIX = /qt4/plugins/styles
	} else {
		TARGET_SUFFIX = /qt5/plugins/styles
	}

	TARGET_PATH = $${TARGET_PREFIX}$${TARGET_SUFFIX}

	isEmpty(QMAKE_EXTENSION_SHLIB) {
		QMAKE_EXTENSION_SHLIB = so
	}

	TARGET_FILES = lib$${TARGET}.$${QMAKE_EXTENSION_SHLIB}

	INSTALLS += target

	target.path  = $${TARGET_PATH}
	target.files = $${TARGET_FILES}

	QMAKE_CLEAN += $${TARGET_FILES}
}
