// qxgeditOptions.h
//
/****************************************************************************
   Copyright (C) 2005-2025, rncbc aka Rui Nuno Capela. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#ifndef __qxgeditOptions_h
#define __qxgeditOptions_h

#include <QSettings>
#include <QStringList>

class QWidget;


//-------------------------------------------------------------------------
// qxgeditOptions - Prototype settings class (singleton).
//

class qxgeditOptions
{
public:

	// Constructor.
	qxgeditOptions();
	// Default destructor.
	~qxgeditOptions();

	// The settings object accessor.
	QSettings& settings();

	// Explicit I/O methods.
	void loadOptions();
	void saveOptions();

	// Command line arguments parser.
	bool parse_args(const QStringList& args);
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
	void show_error(const QString& msg);
#else
	// Command line usage helper.
	void print_usage(const QString& arg0);
#endif

	// Startup supplied session file(s).
	QStringList sessionFiles;

	// Display options...
	bool    bConfirmReset;
	bool    bConfirmRemove;
	bool    bCompletePath;
	float   fRandomizePercent;
	int     iBaseFontSize;
	QString sStyleTheme;
	QString sColorTheme;

	// View options...
	bool    bMenubar;
	bool    bStatusbar;
	bool    bToolbar;

	// Default options...
	QString sSessionDir;
	QString sPresetDir;

	// Recent file list.
	int iMaxRecentFiles;
	QStringList recentFiles;

	// MIDI specific options.
	QStringList midiInputs;
	QStringList midiOutputs;

	// (QS300) USER VOICE Specific options.
	bool bUservoiceAutoSend;

	// Widget geometry persistence helper prototypes.
	void saveWidgetGeometry(QWidget *pWidget, bool bVisible = false);
	void loadWidgetGeometry(QWidget *pWidget, bool bVisible = false);

	// Singleton instance accessor.
	static qxgeditOptions *getInstance();

private:

	// Settings member variables.
	QSettings m_settings;

	// The singleton instance.
	static qxgeditOptions *g_pOptions;
};


#endif  // __qxgeditOptions_h


// end of qxgeditOptions.h
