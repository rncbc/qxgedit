# qxgedit.pro
#
TEMPLATE = subdirs
SUBDIRS = src

lessThan(QT_MAJOR_VERSION, 6) {
	SUBDIRS += skulpture
}

