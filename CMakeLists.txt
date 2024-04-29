cmake_minimum_required (VERSION 3.15)

project (QXGEdit
  VERSION 0.9.91
  DESCRIPTION "Qt XG Editor"
  HOMEPAGE_URL "https://qxgedit.sourceforge.io"
  LANGUAGES C CXX)

set (PROJECT_TITLE "${PROJECT_NAME}")
string (TOLOWER "${PROJECT_TITLE}" PROJECT_NAME)

set (PROJECT_COPYRIGHT "Copyright (C) 2005-2024, rncbc aka Rui Nuno Capela. All rights reserved.")
set (PROJECT_DOMAIN    "rncbc.org")

execute_process (
  COMMAND git describe --tags --dirty --abbrev=6
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_DESCRIBE_OUTPUT
  RESULT_VARIABLE GIT_DESCRIBE_RESULT
  OUTPUT_STRIP_TRAILING_WHITESPACE)
if (GIT_DESCRIBE_RESULT EQUAL 0)
  set (GIT_VERSION "${GIT_DESCRIBE_OUTPUT}")
  string (REGEX REPLACE "^[^0-9]+" "" GIT_VERSION "${GIT_VERSION}")
  string (REGEX REPLACE "^1_"      "" GIT_VERSION "${GIT_VERSION}")
  string (REGEX REPLACE "^[_vV]+"  "" GIT_VERSION "${GIT_VERSION}")
  string (REGEX REPLACE "-g"   "git." GIT_VERSION "${GIT_VERSION}")
  string (REGEX REPLACE "[_|-]"   "." GIT_VERSION "${GIT_VERSION}")
  execute_process (
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_REVPARSE_OUTPUT
    RESULT_VARIABLE GIT_REVPARSE_RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if (GIT_REVPARSE_RESULT EQUAL 0 AND NOT GIT_REVPARSE_OUTPUT STREQUAL "main")
    set (GIT_VERSION "${GIT_VERSION} [${GIT_REVPARSE_OUTPUT}]")
  endif ()
  set (PROJECT_VERSION "${GIT_VERSION}")
endif ()


if (CMAKE_BUILD_TYPE MATCHES "Debug")
  set (CONFIG_DEBUG 1)
  set (CONFIG_BUILD_TYPE "debug")
else ()
  set (CONFIG_DEBUG 0)
  set (CONFIG_BUILD_TYPE "release")
  set (CMAKE_BUILD_TYPE "Release")
endif ()

set (CONFIG_PREFIX "${CMAKE_INSTALL_PREFIX}")

include (GNUInstallDirs)
set (CONFIG_BINDIR  "${CONFIG_PREFIX}/${CMAKE_INSTALL_BINDIR}")
set (CONFIG_LIBDIR  "${CONFIG_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set (CONFIG_DATADIR "${CONFIG_PREFIX}/${CMAKE_INSTALL_DATADIR}")
set (CONFIG_MANDIR  "${CONFIG_PREFIX}/${CMAKE_INSTALL_MANDIR}")


# Enable ALSA Sequencer native support option.
option (CONFIG_ALSA_MIDI "Use ALSA MIDI Sequencer native interface (default=yes)" 1)

# Enable RtMidi cross-platform library support option.
option (CONFIG_RTMIDI  "Use RtMidi library interface (default=no)" 0)


# Enable unique/single instance.
option (CONFIG_XUNIQUE "Enable unique/single instance (default=yes)" 1)

# Enable debugger stack-trace option (assumes --enable-debug).
option (CONFIG_STACKTRACE "Enable debugger stack-trace (default=no)" 0)

# Enable Wayland support option.
option (CONFIG_WAYLAND "Enable Wayland support (EXPERIMENTAL) (default=no)" 0)

# Enable Qt6 build preference.
option (CONFIG_QT6 "Enable Qt6 build (default=yes)" 1)


# Fix for new CMAKE_REQUIRED_LIBRARIES policy.
if (POLICY CMP0075)
  cmake_policy (SET CMP0075 NEW)
endif ()

# Check for Qt...
if (CONFIG_QT6)
  find_package (Qt6 QUIET)
  if (NOT Qt6_FOUND)
    set (CONFIG_QT6 0)
  endif ()
endif ()

if (CONFIG_QT6)
  find_package (QT QUIET NAMES Qt6)
else ()
  find_package (QT QUIET NAMES Qt5)
endif ()

find_package (Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Svg)

if (CONFIG_XUNIQUE)
  find_package (Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network)
endif ()

find_package (Qt${QT_VERSION_MAJOR}LinguistTools)

include (CheckIncludeFile)
include (CheckIncludeFiles)
include (CheckIncludeFileCXX)
include (CheckFunctionExists)
include (CheckLibraryExists)

# Checks for header files.
if (UNIX AND NOT APPLE)
  check_include_files ("fcntl.h;unistd.h;signal.h" HAVE_SIGNAL_H)
endif ()


# Find package modules
include (FindPkgConfig)


# Toggle whether for native ALSA or RtMmidi libraries.
if (CONFIG_RTMIDI)
  set (CONFIG_ALSA_MIDI 0)
endif ()

# Check for ALSA libraries.
if (CONFIG_ALSA_MIDI)
  pkg_check_modules (ALSA REQUIRED IMPORTED_TARGET alsa)
  if (NOT ALSA_FOUND)
    message (FATAL_ERROR "*** ALSA library not found.")
  endif ()
endif ()

# Check for RtMIDI libraries.
if (CONFIG_RTMIDI)
  pkg_check_modules (RTMIDI REQUIRED IMPORTED_TARGET rtmidi)
  if (NOT RTMIDI_FOUND)
    message (FATAL_ERROR "*** RtMidi library not found.")
  endif ()
endif ()


add_subdirectory (skulpture)
add_subdirectory (src)


# Configuration status
macro (SHOW_OPTION text value)
  if (${value})
    message ("${text}: yes")
  else ()
    message ("${text}: no")
  endif ()
endmacro ()

message   ("\n  ${PROJECT_TITLE} ${PROJECT_VERSION} (Qt ${QT_VERSION})")
message   ("\n  Build target . . . . . . . . . . . . . . . . . . .: ${CONFIG_BUILD_TYPE}\n")
show_option ("  ALSA MIDI Sequencer native support . . . . . . . ." CONFIG_ALSA_MIDI)
show_option ("  RtMidi cross-platform support (EXPERIMENTAL) . . ." CONFIG_RTMIDI)
message     ("")
show_option ("  Unique/Single instance support . . . . . . . . . ." CONFIG_XUNIQUE)
show_option ("  Debugger stack-trace (gdb) . . . . . . . . . . . ." CONFIG_STACKTRACE)
message   ("\n  Install prefix . . . . . . . . . . . . . . . . . .: ${CONFIG_PREFIX}\n")
