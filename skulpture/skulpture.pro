TEMPLATE = lib
LANGUAGE = C++

CONFIG	+= plugin warn_on debug

HEADERS	+= \
	sk_effects.h \
	sk_factory.h \
	skulpture_p.h \
	skulpture.h

SOURCES	+= skulpture.cpp

TARGET   = skulpturestyle

win32 {
	debug:DESTDIR = ../win32/debug/styles
	release:DESTDIR = ../win32/release/styles
} else {
	DESTDIR = ../styles
}
